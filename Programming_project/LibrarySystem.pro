QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = LibrarySystem
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    addbookdialog.cpp \
    adduserdialog.cpp \
    borrowdialog.cpp \
    returndialog.cpp

HEADERS += \
    mainwindow.h \
    addbookdialog.h \
    adduserdialog.h \
    borrowdialog.h \
    returndialog.h \
    models.h
