#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T15:06:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SonoGrabber
TEMPLATE = app

INCLUDEPATH += $$PWD/../igtlink/include/igtl
LIBS += "-L$$PWD/../igtlink/lib/igtl" -lOpenIGTLink

SOURCES += main.cpp\
        mainwindow.cpp \
    igtlinkclient.cpp \
    worker.cpp \
    sessionnamegenerator.cpp \
    sessionparams.cpp \
    imageprocessor.cpp \
    writer.cpp

HEADERS  += mainwindow.h \
    igtlinkclient.h \
    worker.h \
    sessionnamegenerator.h \
    sessionparams.h \
    imageprocessor.h \
    writer.h

FORMS    += mainwindow.ui

DISTFILES += ../sonoGrabber.ini
