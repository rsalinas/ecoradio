cmake_minimum_required(VERSION 2.8)

find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(ecoradio SHARED mixer.cpp scheduler.cpp soundsource.cpp ogg_encoder.cpp
    pcm_player.cpp file_scan.cpp decoder.cpp)

add_executable(main main.cpp)

target_link_libraries(ecoradio Qt5::Widgets Qt5::Core Qt5Sql ao)
target_link_libraries(main ecoradio)
