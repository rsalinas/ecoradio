cmake_minimum_required(VERSION 2.8)

project(ecoradio_common)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

add_library(${PROJECT_NAME} SHARED
	common.h
    program.cpp
     )

target_link_libraries(${PROJECT_NAME} 
	Qt5::Core 
	) 

