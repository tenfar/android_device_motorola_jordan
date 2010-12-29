LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),umts_jordan)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := exploid.c

LOCAL_MODULE := exploid
LOCAL_MODULE_TAGS := eng

LOCAL_STATIC_LIBRARIES := \
	libbusybox \
	libclearsilverregex \
	libm \
	libcutils \
	libc

LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)


endif
