cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)
add_subdirectory(radioui)
add_subdirectory(core)
add_subdirectory(snd)
add_subdirectory(util)
add_subdirectory(antenna)
add_subdirectory(common)
add_subdirectory(wsstub)

