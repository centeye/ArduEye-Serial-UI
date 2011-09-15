#-------------------------------------------------
#
# Project created by QtCreator 2011-07-19T14:49:44
#
#-------------------------------------------------

QT       += core gui

TARGET = $$qtLibraryTarget($$TARGET)
TEMPLATE = lib
CONFIG += designer plugin debug_and_release
QTDIR_build:DESTDIR     = $$QT_BUILD_TREE/plugins/designer


SOURCES += paintwidgetplugin.cpp\
        paintwidget.cpp

HEADERS  += paintwidget.h\
    paintwidgetplugin.h

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
