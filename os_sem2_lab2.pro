TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -std=c99

SOURCES += main.c \
    filesystemdriver.c

include(deployment.pri)
qtcAddDeployment()

DISTFILES += \
    exceptions.txt

HEADERS += \
    bitmap.h \
    filesystemdriver.h

