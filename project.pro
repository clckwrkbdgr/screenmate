TARGET = screenmate
VERSION = 2.0.0
CONFIG = qt resources release

RCC_DIR = tmp
OBJECTS_DIR = tmp
MOC_DIR = tmp

RESOURCES = application.qrc
SOURCES = main.cpp mainwindow.cpp mate.cpp
HEADERS = mainwindow.h mate.h

QMAKE_EXTRA_TARGETS += run
run.depends = $(TARGET)
run.commands = ./$(TARGET)
