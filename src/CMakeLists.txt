cmake_minimum_required(VERSION 2.8)


find_package(Qt5Widgets REQUIRED)
#add_subdirectory(sources)

add_library(ecoradio SHARED scheduler.cpp soundsource.cpp ogg_encoder.cpp
    pcm_player.cpp file_scan.cpp alsarec.cpp oggfwd.cpp mpg123wrap.cpp util.cpp sink.h aosink.cpp)

add_executable(main main.cpp)

target_link_libraries(ecoradio Qt5::Widgets Qt5::Core Qt5Sql ao asound shout
    mpg123)
target_link_libraries(main ecoradio)


add_executable(antenna antenna.cpp)
target_link_libraries(antenna ecoradio ogg)

#add_executable(myogg123 ogg123.c)
#target_link_libraries(myogg123 ogg)


