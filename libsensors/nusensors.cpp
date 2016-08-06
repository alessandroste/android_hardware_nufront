/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>

#include <poll.h>
#include <pthread.h>

#include <linux/input.h>

#include <cutils/atomic.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/fdt.h>

#include "nusensors.h"
#include "KionixSensor.h"
#include "LightSensor.h"
#include "AmiSensor.h"
#include "Dmard06Sensor.h"

#define FDT_ACCELEROMETER  "android.hw.gsensor"
#define FDT_LIGHT  "android.hw.L-sensors"
#define FDT_COMPASS  "android.hw.compass"

/*****************************************************************************/

struct sensors_poll_context_t {
    struct sensors_poll_device_t device; // must be first

        sensors_poll_context_t();
        ~sensors_poll_context_t();
    int activate(int handle, int enabled);
    int setDelay(int handle, int64_t ns);
    int pollEvents(sensors_event_t* data, int count);

private:
    enum {
        accel_kxtj9         = 0,
        light_cm3212        = 1,
        compass_ami30x	    = 2,
        accel_dmard06       = 3,
        numSensorDrivers,
        numFds,
    };

    static const size_t wake = numFds - 1;
    static const char WAKE_MESSAGE = 'W';
    struct pollfd mPollFds[numFds];
    int mWritePipeFd;
    SensorBase* mSensors[numSensorDrivers];
    int ret;

    int handleToDriver(int handle) const {
        char buffer[FDT_PROP_VALUE_MAX_LENGTH];
        switch (handle) {
            case ID_A:
              if(fdt_get(FDT_ACCELEROMETER, buffer, "kxtj9")==FDT_ERROR){
                 ALOGE("fdt_get FDT_ACCELEROMETER:%s",strerror(errno));
                 return -1;
              }

              if(!strcmp(buffer,"kxtj9"))
                 return accel_kxtj9;
              else if(!strcmp(buffer,"dmard06"))
                 return accel_dmard06;
              else
                 return -1;

            case ID_L:
              if(fdt_get(FDT_LIGHT, buffer, "cm3212")==FDT_ERROR){
                 ALOGE("fdt_get FDT_LIGHT:%s",strerror(errno));
                 return -1;
              }
              if(!strcmp(buffer,"cm3212"))
                return light_cm3212;
              else
                return -1;

           case ID_MAGNETIC_FIELD:
           case ID_ORIENTATION:
              if(fdt_get(FDT_COMPASS, buffer, "ami30x")==FDT_ERROR){
                 ALOGE("fdt_get FDT_COMPASS:%s",strerror(errno));
                 return -1;
              }
              if(!strcmp(buffer,"ami30x"))
                return compass_ami30x;
              else
                return -1;
        }
        return -EINVAL;
    }
};

/*****************************************************************************/

sensors_poll_context_t::sensors_poll_context_t()
{
    mSensors[accel_kxtj9] = new KionixSensor();
    mPollFds[accel_kxtj9].fd = mSensors[accel_kxtj9]->getFd();
    mPollFds[accel_kxtj9].events = POLLIN;
    mPollFds[accel_kxtj9].revents = 0;
    
    mSensors[light_cm3212] = new LightSensor();
    mPollFds[light_cm3212].fd = mSensors[light_cm3212]->getFd();
    mPollFds[light_cm3212].events = POLLIN;
    mPollFds[light_cm3212].revents = 0;

    mSensors[compass_ami30x] = new AmiSensor();
    mPollFds[compass_ami30x].fd = mSensors[compass_ami30x]->getFd();
    mPollFds[compass_ami30x].events = POLLIN;
    mPollFds[compass_ami30x].revents = 0;
	
    mSensors[accel_dmard06] = new Dmard06Sensor();
    mPollFds[accel_dmard06].fd = mSensors[accel_dmard06]->getFd();
    mPollFds[accel_dmard06].events = POLLIN;
    mPollFds[accel_dmard06].revents = 0;
    int wakeFds[2];
    int result = pipe(wakeFds);
    ALOGE_IF(result<0, "error creating wake pipe (%s)", strerror(errno));
    fcntl(wakeFds[0], F_SETFL, O_NONBLOCK);
    fcntl(wakeFds[1], F_SETFL, O_NONBLOCK);
    mWritePipeFd = wakeFds[1];

    mPollFds[wake].fd = wakeFds[0];
    mPollFds[wake].events = POLLIN;
    mPollFds[wake].revents = 0;
}

sensors_poll_context_t::~sensors_poll_context_t() {
    for (int i=0 ; i<numSensorDrivers ; i++) {
        delete mSensors[i];
    }
    close(mPollFds[wake].fd);
    close(mWritePipeFd);
}

int sensors_poll_context_t::activate(int handle, int enabled) {
    int index = handleToDriver(handle);
    if (index < 0) return index;
    int err =  mSensors[index]->enable(handle, enabled);
    if (enabled && !err) {
        const char wakeMessage(WAKE_MESSAGE);
        int result = write(mWritePipeFd, &wakeMessage, 1);
        ALOGE_IF(result<0, "error sending wake message (%s)", strerror(errno));
    }
    return err;
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns) {

    int index = handleToDriver(handle);
    if (index < 0) return index;
    return mSensors[index]->setDelay(handle, ns);
}

int sensors_poll_context_t::pollEvents(sensors_event_t* data, int count)
{
    int nbEvents = 0;
    int n = 0;

    do {
        // see if we have some leftover from the last poll()
        for (int i=0 ; count && i<numSensorDrivers ; i++) {
            SensorBase* const sensor(mSensors[i]);
            if ((mPollFds[i].revents & POLLIN) || (sensor->hasPendingEvents())) {
                int nb = sensor->readEvents(data, count);
                if (nb < count) {
                    // no more data for this sensor
                    mPollFds[i].revents = 0;
                }
                count -= nb;
                nbEvents += nb;
                data += nb;
            }
        }

        if (count) {
            // we still have some room, so try to see if we can get
            // some events immediately or just wait if we don't have
            // anything to return
            n = poll(mPollFds, numFds, nbEvents ? 0 : -1);
            if (n<0) {
                ALOGE("poll() failed (%s)", strerror(errno));
                return -errno;
            }
            if (mPollFds[wake].revents & POLLIN) {
                char msg;
                int result = read(mPollFds[wake].fd, &msg, 1);
                ALOGE_IF(result<0, "error reading from wake pipe (%s)", strerror(errno));
                ALOGE_IF(msg != WAKE_MESSAGE, "unknown message on wake queue (0x%02x)", int(msg));
                mPollFds[wake].revents = 0;
            }
        }
        // if we have events and space, go read them
    } while (n && count);

    return nbEvents;
}

/*****************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    if (ctx) {
        delete ctx;
    }
    return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
        int handle, int enabled) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
        int handle, int64_t ns) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->setDelay(handle, ns);
}

static int poll__poll(struct sensors_poll_device_t *dev,
        sensors_event_t* data, int count) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->pollEvents(data, count);
}

/*****************************************************************************/

int init_nusensors(hw_module_t const* module, hw_device_t** device)
{
    int status = -EINVAL;

    sensors_poll_context_t *dev = new sensors_poll_context_t();
    memset(&dev->device, 0, sizeof(sensors_poll_device_t));

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version  = 0;
    dev->device.common.module   = const_cast<hw_module_t*>(module);
    dev->device.common.close    = poll__close;
    dev->device.activate        = poll__activate;
    dev->device.setDelay        = poll__setDelay;
    dev->device.poll            = poll__poll;

    *device = &dev->device.common;
    status = 0;
    return status;
}
