cmake_minimum_required(VERSION 2.8)

project(radioui)
find_package(Qt5WebSockets REQUIRED)
find_package(Qt5Widgets)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories (${CMAKE_CURRENT_BINARY_DIR})
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


include_directories(.)
include_directories(..)



QT5_WRAP_CPP(Cmake_hdr_moc radioconsole.h  newprogramdialog.h connectdialog.h)
QT5_WRAP_UI(Cmake_form_hdr radioconsole.ui newprogramdialog.ui connectdialog.ui)

add_library(${PROJECT_NAME}
    radioconsole.cpp
    newprogramdialog.cpp
    connectdialog.cpp
    radioconsole.ui
    newprogramdialog.ui
    connectdialog.ui
    ${Cmake_form_hdr}
    ${Cmake_hdr_moc}
)


target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    wsstub
)


add_executable(radioui_main
    main.cpp
)

target_link_libraries(radioui_main ${PROJECT_NAME})
message("AUTOGEN_TARGETS_FOLDER" ${AUTOGEN_TARGETS_FOLDER})

