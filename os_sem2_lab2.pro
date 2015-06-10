TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -std=c99

SOURCES += src/main.c \
    src/filesystemdriver.c \
    src/fsdevice.c

include(deployment.pri)
qtcAddDeployment()

DISTFILES += \
    doc/exceptions.txt

HEADERS += \
    src/bitmap.h \
    src/filesystemdriver.h \
    src/fsdevice.h

