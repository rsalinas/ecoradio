cmake_minimum_required(VERSION 2.8)

find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(..)

add_executable(ecoradio
    main.cpp
    )

target_link_libraries(ecoradio
    Qt5::Core
    ecoradio_core
    ecoradio_util
)

add_library(ecoradio_core
    alarm.cpp
    ecoradio.cpp
    file_scan.cpp
    icecast_monitor.cpp
    scheduler.cpp
    websockserver.cpp
    programplayer.cpp
)

target_link_libraries(ecoradio_core
    Qt5::Core
    ecoradio_snd
    ecoradio_common
)
