#-------------------------------------------------
#
# Project created by QtCreator 2016-11-07T22:27:51
#
#-------------------------------------------------

QT       += widgets network xmlpatterns testlib concurrent

TARGET = tst_backuprestoretests
CONFIG   += console
CONFIG   += testcase
CONFIG   -= app_bundle
TEMPLATE = app

SOURCES += tst_backuprestoretests.cpp
SOURCES += ../App/backuprestore.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

MOC_DIR = moc
INCLUDEPATH += $$MOC_DIR ../App
