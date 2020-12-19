#-------------------------------------------------
#
# Project created by QtCreator 2018-03-12T16:04:13
#
#-------------------------------------------------

QT       += core gui sql serialport network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appAies2020
TEMPLATE = app


SOURCES += main.cpp\
    cbdd.cpp \
    ccapteurpres.cpp \
    ccapteurtemp.cpp \
    cconfig.cpp \
    cecran.cpp \
    cgpio.cpp \
    cir.cpp \
    cled.cpp \
    cpa.cpp \
    crs232c.cpp \
    ctelecommanders232samsung.cpp \
    biblis/cadafruit_sgp30.cpp \
    biblis/ci2c.cpp \
    cihm.cpp

HEADERS  += \
    cbdd.h \
    ccapteurpres.h \
    ccapteurtemp.h \
    cconfig.h \
    cecran.h \
    cgpio.h \
    cir.h \
    cled.h \
    cpa.h \
    crs232c.h \
    ctelecommanders232samsung.h \
    biblis/cadafruit_sgp30.h \
    biblis/ci2c.h \
    cihm.h

FORMS    += \
    cihm.ui
