cmake_minimum_required(VERSION 2.8)

find_package(Qt5Core)


project(ecoradio_common)

add_library(${PROJECT_NAME} SHARED
	common.h
    program.cpp
     )

target_link_libraries(${PROJECT_NAME} 
	Qt5::Core 
	) 

