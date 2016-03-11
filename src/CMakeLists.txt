cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)
add_subdirectory(radioui)

add_library(ecoradio SHARED scheduler.cpp
    alarm.cpp
    sources/alsarec.cpp
    sources/mpg123wrap.cpp
    sources/soundsource.cpp
    streamsrc.cpp
    sinks/aosink.cpp
    sinks/ogg_encoder.cpp
    sinks/sink.h
    ecoradio.cpp
    file_scan.cpp
    format.h
    mixer.cpp
    oggfwd.cpp
    program.cpp
    util.cpp
    websockserver.cpp
    icecast_monitor.cpp
    rssparser.cpp
    fifo.cpp
    wavwriter.cpp
    interfaces/common.h
     )

add_executable(main main.cpp)

target_link_libraries(ecoradio Qt5::Core Qt5Sql ao asound shout
    mpg123 vorbisenc Qt5::WebSockets Qt5Xml Qt5::Network)
target_link_libraries(main ecoradio)


add_executable(antenna antenna.cpp)
target_link_libraries(antenna ecoradio ogg)

#add_executable(myogg123 ogg123.c)
#target_link_libraries(myogg123 ogg)
