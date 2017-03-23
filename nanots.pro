#-------------------------------------------------
#
# Project created by QtCreator 2017-03-08T23:21:23
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nanots
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    timesheeteditor.cpp

HEADERS  += mainwindow.h \
    timesheeteditor.h \
    main.h

FORMS    += mainwindow.ui \
    timesheeteditor.ui
