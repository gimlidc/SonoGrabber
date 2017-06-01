#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T15:06:53
#
#-------------------------------------------------

QT       += core gui svg
QT      += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SonoGrabber
TEMPLATE = app

IGTL = $$PWD/../../OpenIGTLink
IGTL_BUILD = $$IGTL/Build

#INCLUDEPATH += $$PWD/../igtlink/include/igtl
#LIBS += "-L$$PWD/../igtlink/lib/igtl" -lOpenIGTLink

INCLUDEPATH += $$IGTL/Source $$IGTL/Source/igtlutil $$IGTL_BUILD $$PWD/../igtlink/include/igtl
#LIBS += "-L$$IGTL_BUILD/bin" -lOpenIGTLink -lws2_32

SOURCES += main.cpp\
        mainwindow.cpp \
    igtlinkclient.cpp \
    worker.cpp \
    sessionnamegenerator.cpp \
    sessionparams.cpp \
    imageprocessor.cpp \
    writer.cpp \
#    breastselector.cpp \
    startsequence.cpp \
    breastgraph.cpp \
    breastselector.cpp \
#    position.cpp
    sonoimage.cpp \
    transform.cpp

HEADERS  += mainwindow.h \
    igtlinkclient.h \
    worker.h \
    sessionnamegenerator.h \
    sessionparams.h \
    imageprocessor.h \
    writer.h \
    startimagewindow.h \
#    breastselector.h \
    startsequence.h \
    breastgraph.h \
    side.h \
    breastselector.h \
#    position.h
    sonoimage.h \
    icorner.h \
    transform.h \
    frozen.h

FORMS    += mainwindow.ui \
    startsequence.ui

DISTFILES += ../sonoGrabber.ini

win32:CONFIG(release, debug|release): LIBS += -L$$IGTL_BUILD/bin/ -lOpenIGTLink -lws2_32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$IGTL_BUILD/bin/ -lOpenIGTLink -lws2_32
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/igtl/ -lOpenIGTLink

#INCLUDEPATH += $$PWD/../../../../../usr/local/lib/igtl
#DEPENDPATH += $$PWD/../../../../../usr/local/lib/igtl

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$IGTL_BUILD/bin/libOpenIGTLink.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$IGTL_BUILD/bin/libOpenIGTLink.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/release/OpenIGTLink.lib
##else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/debug/OpenIGTLink.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/libOpenIGTLink.a


DISTFILES += ../sonoGrabber.ini

RESOURCES += \
    images.qrc
