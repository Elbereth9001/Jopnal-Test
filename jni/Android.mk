LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jopnal-example

LOCAL_SRC_FILES := main.cpp

LOCAL_SHARED_LIBRARIES := jopnal
LOCAL_WHOLE_STATIC_LIBRARIES := sfml-main

include $(BUILD_SHARED_LIBRARY)

$(call import-module, jopnal)