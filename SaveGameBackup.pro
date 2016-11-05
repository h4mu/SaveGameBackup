#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T20:43:32
#
#-------------------------------------------------

QT       += core gui xmlpatterns concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SaveGameBackup
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    directoryscanner.cpp \
    filesystembasepathfinder.cpp \
    registrybasepathfinder.cpp \
    settingsprovider.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    directoryscanner.h \
    filesystembasepathfinder.h \
    registrybasepathfinder.h \
    settingsprovider.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
