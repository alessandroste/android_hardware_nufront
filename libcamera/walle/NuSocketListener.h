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
#ifndef _NU_SOCKETLISTENER_H
#define _NU_SOCKETLISTENER_H

#include <pthread.h>

#include "NuSocketClient.h"

class NuSocketListener {
    int                     mSock;
    const char              *mSocketName;
    NuSocketClientCollection  *mClients;
    pthread_mutex_t         mClientsLock;
    bool                    mListen;
    int                     mCtrlPipe[2];
    pthread_t               mThread;

public:
    NuSocketListener(const char *socketName, bool listen);
    NuSocketListener(int socketFd, bool listen);

    virtual ~NuSocketListener();
    int startListener();
    int stopListener();

    void sendBroadcast(const void *msg, int len);
protected:
    virtual bool onDataAvailable(NuSocketClient *c ) = 0;

private:
    static void *threadStart(void *obj);
    void runListener();
};
#endif
