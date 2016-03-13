cmake_minimum_required(VERSION 2.8)

project(antenna)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(..)

add_executable(${PROJECT_NAME} antenna.cpp)
target_link_libraries(${PROJECT_NAME} ecoradio_snd ogg)
