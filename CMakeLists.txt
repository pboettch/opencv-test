cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "-Wall")

# add OpenCV-path to find the cmake-module
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/share/opencv)
find_package(OpenCV REQUIRED)

# face-detect-library with the 'well-defined' interface
add_library(face-detect face-detect.cpp)
target_link_libraries(face-detect PRIVATE ${OpenCV_LIBS})
target_compile_features(face-detect PRIVATE cxx_range_for) # C++11

# face-detect-test using the webcam and OpenCV to read from it
add_executable(webcam-opencv webcam-opencv.cpp)
target_link_libraries(webcam-opencv ${OpenCV_LIBS} face-detect)
target_compile_features(webcam-opencv PRIVATE cxx_range_for) # C++11

# face-detect-test using the webcam and v4l2 to read from it
add_executable(webcam-v4l2 webcam-v4l2.c)
target_link_libraries(webcam-v4l2 face-detect)
