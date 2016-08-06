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
ifneq ($(TARGET_PRODUCT_NAME),PROTOTYPE)
ifneq ($(CAMERA_DEVICE_TYPE),WALLE)

ifeq ($(BOARD_USE_USB_CAMERA), true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_CFLAGS += -fno-short-enums -DQEMU_HARDWARE
LOCAL_SHARED_LIBRARIES:= \
    libbinder \
    libutils \
    libcutils \
    libcamera_client \
    libui \
    libhardware \
	libfdt \
        libstagefright_foundation \
	libstagefright	\
	libcameraservice \
	libhardwarerenderercamera

ifeq ($(BOARD_FLASH_IOCTL),true)
LOCAL_CFLAGS += -DNU_FLASH_CTRL_IOCTL
endif

ifeq ($(BOARD_USES_CAMERA_OV2643),true)
LOCAL_CFLAGS += -DCAMERA_OV2643
endif

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
LOCAL_C_INCLUDES += \
        $(TOP)/frameworks/av/media/libstagefright/nufront/hardwarerenderer_camera \
        $(TOP)/frameworks/av/media/libstagefright/include \
        $(TOP)/frameworks/native/include/media/openmax \
        $(TOP)/frameworks/av/services/camera/libcameraservice/
LOCAL_SRC_FILES := \
    NuCameraV4L2.cpp \
    NuCameraHardware.cpp \
    JpegCompressor.cpp \
    Converters.cpp \
    Effect.cpp \
    ExifEncoder.cpp

ifeq ($(BOARD_USE_USB_CAMERA),true)
LOCAL_MODULE := camera.default
else
LOCAL_MODULE := camera.default.ori
endif

LOCAL_MODULE_TAGS := eng 
include $(BUILD_SHARED_LIBRARY)

endif
endif
endif
endif
