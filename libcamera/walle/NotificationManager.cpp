/*
 * Copyright (C) 2008 The Android Open Source Project
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
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>

#define LOG_TAG "NotificationManager"
#include <cutils/log.h>

#include "NotificationManager.h"
#include "FaceCapture.h"

NotificationManager *NotificationManager::sInstance = NULL;
bool NotificationManager::isSokcetInited = false;

NotificationManager * NotificationManager::getInstance(const char* socketName)
{
    if (!sInstance){
        sInstance = new NotificationManager(socketName);
    }
    return sInstance;
}

NotificationManager::NotificationManager(const char* socket) :
                            NuSocketListener(socket, true) {
}

NotificationManager::~NotificationManager(){
    if(mListener != NULL){
        isSokcetInited = false;
        stopListener();
        delete mListener;
        mListener = NULL;
    }
}

void NotificationManager::startNotifyThread(){
    if(!isSokcetInited) {
        mListener = NULL;
        registerListener(new NuCommandListener());
        startListener();
        isSokcetInited = true;
    }
}

void NotificationManager::stopNotifyThread(){
    //stopListener();
}

void NotificationManager::sendNotification(int msg)
{
    //LOGD("---test---,sendNotification(%d)", msg);
    sendBroadcast(&msg, sizeof(msg));
}

void NotificationManager::registerListener(NuCommandListener *listener)
{
    mListener = listener;
}

bool NotificationManager::onDataAvailable(NuSocketClient *cli)
{
    int socket = cli->getSocket();
    ssize_t count;

    count = TEMP_FAILURE_RETRY(read(socket, mBuffer, sizeof(mBuffer)));
    if (count < 0 || count > 4) {
        SLOGE("recvmsg failed (%s)", strerror(errno));
        return false;
    } else if (count == 0){
        LOGD("count = 0, client closed!");
        return false;
    }
    

    int *value = (int *)&mBuffer[0];
    onCommand(*value); 

    return true;
}

void NotificationManager::onCommand(const int mode)
{
    if (mListener != NULL){
        mListener->onCommand(mode);
    }
}

void NuCommandListener::onCommand(int mode){
    LOGD("NuCommandListener->onCommand(), Set walle gesture ctrl mode:%d.", mode);
    setOperationModel(mode);
}

