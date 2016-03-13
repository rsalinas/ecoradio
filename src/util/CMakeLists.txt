cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(snd)

add_library(ecoradio_util SHARED
    fifo.cpp
    rssparser.cpp
    util.cpp
    streamsrc.cpp
    http.cpp
     )

target_link_libraries(ecoradio_util Qt5::Core Qt5Sql ao asound shout
    mpg123 vorbisenc Qt5::WebSockets Qt5Xml Qt5::Network)

#add_executable(myogg123 ogg123.c)
#target_link_libraries(myogg123 ogg)
