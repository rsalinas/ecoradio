cmake_minimum_required(VERSION 2.8)

project(wsstub)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(..)

add_library(${PROJECT_NAME} SHARED
    radiostub.cpp
     )     
target_link_libraries(${PROJECT_NAME} Qt5::WebSockets ecoradio_common)
