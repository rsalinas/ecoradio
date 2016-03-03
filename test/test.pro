
QMAKE_CXXFLAGS += -std=c++11

QT += core
QT -= gui

TARGET = test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDE += /usr/include/gtest/
INCLUDE += /usr/src/gtest/

SOURCES += test.cpp \
    /usr/src/gtest/src/gtest.cc



