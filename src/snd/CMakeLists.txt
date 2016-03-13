cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(..)

add_library(ecoradio_snd SHARED
    sources/alsarec.cpp
    sources/mpg123wrap.cpp
    sources/soundsource.cpp
    sinks/aosink.cpp
    sinks/ogg_encoder.cpp
    sinks/sink.h
    mixer.cpp
    wavwriter.cpp
    oggfwd.cpp
    format.h
     )

target_link_libraries(ecoradio_snd Qt5::Core Qt5Sql ao asound shout
    mpg123 vorbisenc Qt5::WebSockets Qt5Xml Qt5::Network ecoradio_util)
