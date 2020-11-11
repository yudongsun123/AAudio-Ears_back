LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := Ears_back
LOCAL_SRC_FILES := cpp/com_example_erfan_MainActivity.cpp\
				   cpp/StreamEngine.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cpp/include
LOCAL_LDLIBS := -laaudio -landroid -llog
include $(BUILD_SHARED_LIBRARY)