cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "-Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/share/opencv)
find_package(OpenCV REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBS})
target_compile_features(main PRIVATE cxx_range_for)

add_executable(face face.cpp)
target_link_libraries(face ${OpenCV_LIBS})
target_compile_features(face PRIVATE cxx_range_for)
