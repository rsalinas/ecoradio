cmake_minimum_required(VERSION 2.8)

project(ecoradio_util)

#find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(snd)

add_library(${PROJECT_NAME} SHARED
    fifo.cpp
    rssparser.cpp
    util.cpp
    streamsrc.cpp
    http.cpp
    telegram.cpp
     )

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5Sql ao asound shout
    mpg123 vorbisenc Qt5::WebSockets Qt5Xml Qt5::Network)
