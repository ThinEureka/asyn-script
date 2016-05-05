LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libAsynscript_static

FILE_LIST := $(wildcard $(LOCAL_PATH)/../../source/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../source

include $(BUILD_STATIC_LIBRARY)
