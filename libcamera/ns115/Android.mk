# Copyright (C) 2011 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifeq ($(TARGET_BOARD_PLATFORM),ns115)

ifeq ($(TARGET_PRODUCT_NAME),HDMI_STICKER)
ifneq ($(BOARD_USE_USB_CAMERA), true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_CFLAGS += -fno-short-enums -Wno-error=strict-aliasing -DQEMU_HARDWARE
LOCAL_SHARED_LIBRARIES:= \
    libbinder \
    libutils \
    libcutils \
    libcamera_client \
    libui \
    libhardware \
	libfdt

# JPEG conversion libraries and includes.
LOCAL_SHARED_LIBRARIES += \
  libjpeg \
  libskia \
  libandroid_runtime \
  libh1enc

LOCAL_C_INCLUDES += external/jpeg \
                    external/skia/include/core/ \
		            frameworks/base/core/jni/android/graphics \
                    hardware/nufront/media_115/vpu/encode/inc \
                    system/core/include/
LOCAL_SRC_FILES := \
    NuCameraV4L2.cpp \
    NuCameraHardware.cpp \
    JpegCompressor.cpp \
    Converters.cpp \
    Effect.cpp \
    ExifEncoder.cpp \
    CameraTest.cpp

ifeq ($(BOARD_USE_USB_CAMERA),true)
LOCAL_MODULE := camera.default.ori
else
LOCAL_MODULE := camera.default
endif

LOCAL_MODULE_TAGS := eng 
include $(BUILD_SHARED_LIBRARY)

endif
endif
endif
