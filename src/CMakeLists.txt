cmake_minimum_required(VERSION 2.8)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebSockets REQUIRED)
#add_subdirectory(sources)

add_library(ecoradio SHARED scheduler.cpp
    alarm.cpp
    alsarec.cpp
    aosink.cpp
    ecoradio.cpp
    file_scan.cpp
    format.h
    mixer.cpp
    mpg123wrap.cpp
    ogg_encoder.cpp
    oggfwd.cpp
    program.cpp
    sink.h
    soundsource.cpp
    util.cpp
    websockserver.cpp
     )

add_executable(main main.cpp)

target_link_libraries(ecoradio Qt5::Widgets Qt5::Core Qt5Sql ao asound shout
    mpg123 vorbisenc Qt5::WebSockets )
target_link_libraries(main ecoradio)


add_executable(antenna antenna.cpp)
target_link_libraries(antenna ecoradio ogg)

#add_executable(myogg123 ogg123.c)
#target_link_libraries(myogg123 ogg)
