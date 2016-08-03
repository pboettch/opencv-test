LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := parrot-snap-facedetect-opencv
LOCAL_DESCRIPTION := library which does the magic
LOCAL_CATEGORY_PATH := video

LOCAL_SRC_FILES := face-detect.cpp
LOCAL_CXXFLAGS := -std=c++11

LOCAL_LIBRARIES := opencv

include $(BUILD_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := webcam-opencv
LOCAL_DESCRIPTION := Small opencv testing app with face-detection
LOCAL_CATEGORY_PATH := video

LOCAL_SRC_FILES := webcam-opencv.cpp
LOCAL_CXXFLAGS := -std=c++11

LOCAL_LIBRARIES := opencv parrot-snap-facedetect-opencv

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := webcam-v4l2
LOCAL_DESCRIPTION := Small v4l2 with face-detection
LOCAL_CATEGORY_PATH := video

LOCAL_SRC_FILES := webcam-v4l2.c

LOCAL_LIBRARIES := parrot-snap-facedetect-opencv

include $(BUILD_EXECUTABLE)
