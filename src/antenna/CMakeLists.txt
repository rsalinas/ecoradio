cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(..)

add_executable(antenna antenna.cpp)
target_link_libraries(antenna ecoradio_snd ogg)
