cmake_minimum_required(VERSION 3.5)

project(pBp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(SRC main.cpp)

add_executable(pBp ${SRC})

target_link_libraries(pBp ${OpenCV_LIBRARIES})
