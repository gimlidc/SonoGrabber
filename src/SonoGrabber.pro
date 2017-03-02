#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T15:06:53
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SonoGrabber
TEMPLATE = app

INCLUDEPATH += $$PWD/../../../../../usr/local/include/igtl /home/schier/OpenIGTLink/Source /home/schier/OpenIGTLink/Source/igtlutil
#INCLUDEPATH += $$PWD/../igtlink/include/igtl
LIBS += "-L$$PWD/../../../../../usr/local/lib/igtl" -lOpenIGTLink

SOURCES += main.cpp\
        mainwindow.cpp \
    igtlinkclient.cpp \
    worker.cpp \
    sessionnamegenerator.cpp \
    sessionparams.cpp \
    imageprocessor.cpp \
    writer.cpp \
    startimagewindow.cpp \
#    breastselector.cpp \
    startsequence.cpp

HEADERS  += mainwindow.h \
    igtlinkclient.h \
    worker.h \
    sessionnamegenerator.h \
    sessionparams.h \
    imageprocessor.h \
    writer.h \
    startimagewindow.h \
#    breastselector.h \
    startsequence.h

FORMS    += mainwindow.ui \
    startsequence.ui

DISTFILES += ../sonoGrabber.ini \
    images/drawing.png

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/igtl/release/ -lOpenIGTLink
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/igtl/debug/ -lOpenIGTLink
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/igtl/ -lOpenIGTLink

INCLUDEPATH += $$PWD/../../../../../usr/local/lib/igtl
DEPENDPATH += $$PWD/../../../../../usr/local/lib/igtl

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/release/libOpenIGTLink.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/debug/libOpenIGTLink.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/release/OpenIGTLink.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/debug/OpenIGTLink.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/igtl/libOpenIGTLink.a

RESOURCES += \
    images.qrc
