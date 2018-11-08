cmake_minimum_required(VERSION 2.8)

project(pro)

find_package(OpenCV REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} ) 
add_definitions(-std=c++11)
add_executable(pro pro.cpp)
target_link_libraries(pro ${OpenCV_LIBS})
