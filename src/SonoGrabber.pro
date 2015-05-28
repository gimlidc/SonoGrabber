#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T15:06:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mytest
TEMPLATE = app

INCLUDEPATH += ../igtlink/include/igtl
LIBS += "-L../igtlink/lib/igtl" -lOpenIGTLink

SOURCES += main.cpp\
        mainwindow.cpp \
    igtlinkclient.cpp

HEADERS  += mainwindow.h \
    igtlinkclient.h

FORMS    += mainwindow.ui
