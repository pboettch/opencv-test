LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := face
LOCAL_DESCRIPTION := Small opencv testing app with face-detection
LOCAL_CATEGORY_PATH := video

LOCAL_SRC_FILES := face.cpp
LOCAL_CXXFLAGS := -std=c++11

LOCAL_LIBRARIES := opencv

include $(BUILD_EXECUTABLE)
