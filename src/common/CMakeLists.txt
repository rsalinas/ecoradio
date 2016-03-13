cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

add_library(ecoradio_common SHARED
	common.h
    program.cpp
     )

target_link_libraries(ecoradio_common Qt5::Core ) 

