#-------------------------------------------------
#
# Project created by QtCreator 2017-08-02T21:18:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = x-y
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    picturebox.cpp

HEADERS  += widget.h \
    picturebox.h

FORMS    += widget.ui

unix{
    INCLUDEPATH += /usr/local/include
}

win32{
    RC_FILE += icon.rc
}

RESOURCES += \
    pic.qrc
