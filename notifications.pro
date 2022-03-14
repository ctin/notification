#-------------------------------------------------
#
# Project created by QtCreator 2017-04-09T16:05:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 6): QT += widgets

TARGET = notifications
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS



SOURCES += main.cpp\
        mainwindow.cpp \
        Notifications/NotificationWidget.cpp \
        Notifications/NotificationLayout.cpp \
        Result/Result.cpp \
        Operation/Operation.cpp

HEADERS  += mainwindow.h \
        Notifications/NotificationWidget.h \
        Notifications/NotificationLayout.h \
        Result/Result.h \
        Operation/Operation.h

FORMS    += mainwindow.ui
