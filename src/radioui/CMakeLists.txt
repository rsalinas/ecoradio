cmake_minimum_required(VERSION 2.8)

project(radioui)

find_package(Qt5WebSockets REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets)
include_directories(..)

#QT5_WRAP_CPP(Cmake_hdr_moc radioconsole.h  newprogramdialog.h)
QT5_WRAP_UI(Cmake_form_hdr radioconsole.ui newprogramdialog.ui)


# Tell CMake to create the helloworld executable
add_executable(radioui
    main.cpp
    radioconsole.cpp
    newprogramdialog.cpp
    radiostub.cpp
    radioconsole.ui
    newprogramdialog.ui
    ${Cmake_form_hdr}
)
#    ${Cmake_hdr_moc} ${Cmake_form_hdr})

# Use the Widgets module from Qt 5.
target_link_libraries(radioui Qt5::Widgets Qt5::WebSockets  Qt5::Network)
