#-------------------------------------------------
#
# Project created by QtCreator 2011-06-15T14:18:47
#
#-------------------------------------------------

QT       += core gui

TARGET = ArduEye_UI
TEMPLATE = app


SOURCES += main.cpp\
    commwrapper.cpp \
    ArduEye_UI.cpp

HEADERS  += commwrapper.h \
    Datasets.h \
    ArduEye_UI.h

FORMS    += \
    ArduEyeUI.ui

RESOURCES += \
    ArduEye.qrc

INCLUDEPATH  += ../qextserialport_lean
QMAKE_LIBDIR += ../qextserialport-build-desktop/build



CONFIG(debug, debug|release):LIBS  += -lqextserialportd
else:LIBS  += -lqextserialport
win32:LIBS += -lsetupapi
