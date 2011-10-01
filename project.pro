TARGET = screenmate
VERSION = 1.0.0
TEMPLATE = app
CONFIG += qt release

MOC_DIR = tmp
OBJECTS_DIR = tmp
RCC_DIR = tmp

RESOURCES = application.qrc
SOURCES += main.cpp \
    mainwindow.cpp \
    groundengine.cpp
HEADERS += mainwindow.h \
    groundengine.h
