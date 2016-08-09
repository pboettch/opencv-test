cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "-Wall")

# add OpenCV-path to find the cmake-module
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/share/opencv)
find_package(OpenCV REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(GStreamer REQUIRED)

# local ulog :-( - shame! FIXME
add_library(ulog INTERFACE)
target_include_directories(ulog INTERFACE ulog/)

# import pomp
add_subdirectory(../libpomp pomp-dir EXCLUDE_FROM_ALL)


# create libmuta here
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../libmuta/src/muta.c)
    message(FATAL_ERROR "This project needs libmuta available at ${CMAKE_CURRENT_SOURCE_DIR}/../")
endif()

add_library(muta ../libmuta/src/muta.c)
target_include_directories(muta PUBLIC ../libmuta/include)

target_link_libraries(muta
    PUBLIC gstreamer
    PRIVATE ulog pomp
)


# create gstreamer-parrot-user-plugin here
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../gst-parrot-userfilter)
    message(FATAL_ERROR "This project needs the gst-parrot-userfilter available at ${CMAKE_CURRENT_SOURCE_DIR}/../")
endif()
add_library(gst-parrot-userfilter MODULE ../gst-parrot-userfilter/userfilter.c)
target_link_libraries(gst-parrot-userfilter PRIVATE gstreamer pomp muta)
target_include_directories(gst-parrot-userfilter PRIVATE ../gst-parrot-userfilter)
target_compile_definitions(gst-parrot-userfilter PRIVATE GST_USE_UNSTABLE_API)





# face-detect-library with the 'well-defined' interface
add_library(face-detect face-detect.cpp)
target_include_directories(face-detect PUBLIC .)
target_link_libraries(face-detect
    PRIVATE ${OpenCV_LIBS}
    PUBLIC gstreamer)
target_compile_features(face-detect PRIVATE cxx_range_for) # C++11

# face-detect-test using the webcam and OpenCV to read from it
add_executable(webcam-opencv webcam-opencv.cpp)
target_link_libraries(webcam-opencv ${OpenCV_LIBS} face-detect)
target_compile_features(webcam-opencv PRIVATE cxx_range_for) # C++11

# face-detect-test using the webcam and v4l2 to read from it
add_executable(webcam-v4l2 webcam-v4l2.c)
target_link_libraries(webcam-v4l2 face-detect)


# face-detect using pomp
add_executable(face-detect-main face-detect-main.c)
target_link_libraries(face-detect-main pomp face-detect muta)
