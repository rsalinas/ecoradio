project(test)
cmake_minimum_required(VERSION 2.8.2)
add_subdirectory(/usr/src/gtest gtest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

enable_testing()
add_executable(test test.cpp)
include_directories(../src)
target_link_libraries(test gtest gtest_main ecoradio)

add_test(AllTestsInFoo test)

