/*
* Copyright (C) Bosch Sensortec GmbH 2011
* Copyright (C) 2008 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
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
#include <unistd.h>
#include <sys/select.h>
#include "Dmard06Sensor.h"

//#define DEBUG_SENSOR     1

#define CONVERT                     (GRAVITY_EARTH / 32.0f)
#define CONVERT_X                   (-CONVERT)
#define CONVERT_Y                   (-CONVERT)
#define CONVERT_Z                   (CONVERT)

#define SENSOR_NAME                    "DMT_compass"
#define Dmard06_DEV                    "/dev/dmard06"
#define CLASS_PATH                  "/sys/class/accelemeter/dmard06"  

/*****************************************************************************/

Dmard06Sensor::Dmard06Sensor()
: SensorBase(Dmard06_DEV, SENSOR_NAME),
      mEnabled(0),
      mInputReader(8),
      mHasPendingEvent(false)
{
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = ID_A;
    mPendingEvent.type = SENSOR_TYPE_ACCELEROMETER;
    mPendingEvent.acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
    
   int flags = 0;
   if (!Read_sysfs_input_attr(CLASS_PATH, "enable_acc", (char *)&flags,1)) {
#ifdef DEBUG_SENSOR
           LOGD("Dmard06Sensor flag: %d\n",flags);
#endif
       flags = flags - 0x30;   //ASCII
        if (flags) {
            mEnabled = 1;
        }
   }
}

Dmard06Sensor::~Dmard06Sensor() {
}

int Dmard06Sensor::set_sysfs_input_attr(char *class_path, const char *attr, char *value, int len)
{
   char path[256];
   int fd;

   if (class_path == NULL || *class_path == '\0'
       || attr == NULL || value == NULL || len < 1) {
       return -EINVAL;
   }
   snprintf(path, sizeof(path), "%s/%s", class_path, attr);
   path[sizeof(path) - 1] = '\0';
   fd = open(path, O_RDWR);
   if (fd < 0) {
       return -errno;
   }
   if (write(fd, value, len) < 0) {
       close(fd);
       return -errno;
   }
   close(fd);

   return 0;
}

int Dmard06Sensor::Read_sysfs_input_attr(char *class_path,
               const char *attr, char *value, int len)
{
   char path[256];
   int fd;

   if (class_path == NULL || *class_path == '\0'
       || attr == NULL || value == NULL || len < 1) {
       return -EINVAL;
   }

   snprintf(path, sizeof(path), "%s/%s", class_path, attr);
   path[sizeof(path) - 1] = '\0';
   fd = open(path, O_RDWR);
   if (fd < 0) {
       return -errno;
   }
   if (read(fd, value, len) < 0) {
       close(fd);
       return -errno;
   }
   close(fd);

   return 0;
}



int Dmard06Sensor::enable(int32_t handle, int en)
{
    int newState = en ? 1 : 0;
   char buffer[20];

    if (newState != mEnabled) {
       int bytes = sprintf(buffer, "%d\n", en);

       if(!set_sysfs_input_attr(CLASS_PATH,"enable_acc",buffer,bytes)){
            LOGD("Dmard06Sensor read enable_acc");
           mEnabled = newState;
       }
    }

    return 0;
}


int Dmard06Sensor::setDelay(int32_t handle, int64_t ns)
{
    if (ns < 0)
        return -EINVAL;

       char buffer[20];
   int ms=ns/1000;
   int bytes = sprintf(buffer, "%d\n", ms);
   set_sysfs_input_attr(CLASS_PATH,"delay_acc",buffer,bytes);
   return 0;
}

bool Dmard06Sensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int Dmard06Sensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            switch (event->code) {
                case EVENT_TYPE_ACCEL_X:   //ABS_X
                    mPendingEvent.acceleration.x = event->value * CONVERT_X;
                    break;
                case EVENT_TYPE_ACCEL_Y:   //ABS_Y
                    mPendingEvent.acceleration.y = event->value * CONVERT_Y;
                    break;
                case EVENT_TYPE_ACCEL_Z:
                    mPendingEvent.acceleration.z = event->value * CONVERT_Z; 
                    break;
            }
        } else if (type == EV_SYN) {
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                *data++ = mPendingEvent;
                count--;
                numEventReceived++;
            }
#ifdef DEBUG_SENSOR
           LOGD("Dmard06 Sensor data: t x,y,x: %f %f, %f, %f\n",
                   mPendingEvent.timestamp / 1000000000.0,
                           mPendingEvent.acceleration.x,
                           mPendingEvent.acceleration.y,
                           mPendingEvent.acceleration.z);
#endif
        } else {
            LOGE("Dmard06Sensor: unknown event (type=%d, code=%d)",
                    type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

