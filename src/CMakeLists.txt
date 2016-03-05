cmake_minimum_required(VERSION 2.8)

find_package(Qt5Widgets REQUIRED)

add_library(ecoradio SHARED scheduler.cpp soundsource.cpp ogg_encoder.cpp
    pcm_player.cpp file_scan.cpp decoder.cpp alsarec.cpp)

add_executable(main main.cpp)

target_link_libraries(ecoradio Qt5::Widgets Qt5::Core Qt5Sql ao asound)
target_link_libraries(main ecoradio)
