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
#ifndef _NOTIFICATION_LISTENER_H
#define _NOTIFICATIONL_ISTENER_H

#include "NuSocketListener.h"
class NuCommandListener {
    public:
     virtual void onCommand(const int len);
     virtual ~NuCommandListener(){};
};

class NotificationManager : public NuSocketListener {
public:
    virtual ~NotificationManager();

    static NotificationManager * getInstance(const char* socket);
    void startNotifyThread();
    void stopNotifyThread();
    void sendNotification(int msg);
    void registerListener(NuCommandListener *listener);
protected:
    virtual bool onDataAvailable(NuSocketClient *c);
private:
    char mBuffer[1024];
    static NotificationManager *sInstance;
    static bool isSokcetInited;
    NuCommandListener *mListener;
    NotificationManager(const char* socket);
    void onCommand(const int mode);
};

#endif
