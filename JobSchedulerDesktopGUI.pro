#-------------------------------------------------
#
# Project created by QtCreator 2016-04-07T15:59:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JobSchedulerDesktopGUI
TEMPLATE = app


SOURCES += main.cpp\        
    WindowsEventParser/common.cpp \
    WindowsEventParser/windowseventparser.cpp \    
    core/schedulemodel.cpp \
    ui/jobschedulermain.cpp \
    core/schedulercommon.cpp

HEADERS  += WindowsEventParser/common.h \
    WindowsEventParser/windowseventparser.h \    
    core/schedulemodel.h \
    ui/jobschedulermain.h \
    core/schedulercommon.h

FORMS    += ui/jobschedulermain.ui

LIBS += -lWevtapi

#DISTFILES += \
#    holidays.json

RESOURCES += \
    main.qrc
