#-------------------------------------------------
#
# Project created by QtCreator 2014-01-02T10:58:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QuadricoptèreV4
TEMPLATE = app

include(qextserialport/src/qextserialport.pri)

SOURCES += main.cpp \
    receivedata.cpp \
    senddata.cpp \
    liaisonserie.cpp \
    interface.cpp

HEADERS  += \
    receivedata.h \
    senddata.h \
    Globales.h \
    liaisonserie.h \
    interface.h

FORMS    +=
