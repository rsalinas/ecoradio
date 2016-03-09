project(test)
cmake_minimum_required(VERSION 2.8.2)
add_subdirectory(/usr/src/gtest gtest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

enable_testing()
include_directories(../src)
add_executable(test test.cpp testutil.cpp)
add_executable(test-snd test-snd.cpp testutil.cpp)
add_executable(sound_playback sound_playback.cpp)
add_executable(test-sched test-sched.cpp testutil.cpp)
add_executable(test-rss test-rss.cpp )

add_executable(ao_example ao_example.c)

add_executable(alsa-record-example alsa-record-example.c)
target_link_libraries(alsa-record-example asound)

target_link_libraries(test gtest ecoradio)
target_link_libraries(test-snd gtest ecoradio)
target_link_libraries(sound_playback asound)
target_link_libraries(test-sched ecoradio gtest)
target_link_libraries(test-rss gtest  Qt5::Core ecoradio)
target_link_libraries(ao_example ao dl m)

add_test(AllTestsInFoo test)
add_test(AllTestsInFoo test-snd)
add_test(AllTestsInFoo test-sched)
#add_test(AllTestsInFoo test-rss)

