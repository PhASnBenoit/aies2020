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
    biblis/ci2c.cpp \
    cihm.cpp \
    cadafruit_sgp30.cpp \
    cthreadcapteurs.cpp \
    ccapteurgazfumeemq_2.cpp \
    csharedmemory.cpp

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
    biblis/ci2c.h \
    cihm.h \
    cadafruit_sgp30.h \
    cthreadcapteurs.h \
    ccapteurgazfumeemq_2.h \
    csharedmemory.h

FORMS    += \
    cihm.ui
