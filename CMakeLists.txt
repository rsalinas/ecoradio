project(ecoradio2)
cmake_minimum_required(VERSION 2.8)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-std=c++11)

add_subdirectory(src)
add_subdirectory(test)

