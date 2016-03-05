project(test)
cmake_minimum_required(VERSION 2.8.2)
add_subdirectory(/usr/src/gtest gtest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

enable_testing()
include_directories(../src)
add_executable(test test.cpp)
add_executable(test-snd test-snd.cpp)
add_executable(sound_playback sound_playback.cpp)
target_link_libraries(test gtest gtest_main ecoradio)

target_link_libraries(test-snd gtest gtest_main ecoradio)
target_link_libraries(sound_playback asound)
add_executable(ao_example ao_example.c)
target_link_libraries(ao_example ao dl m)
add_test(AllTestsInFoo test)
add_test(AllTestsInFoo test-snd)



add_executable(alsa-record-example alsa-record-example.c)
target_link_libraries(alsa-record-example asound)
