#-------------------------------------------------
#
# Project created by QtCreator 2017-09-24T10:59:20
#
#-------------------------------------------------

QT       += core gui sql printsupport testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FURS_Management
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug, debug|release) {
    #This is a debug build
    DEFINES += QT_TESTS
} else {
    #This is a release build
}

SOURCES += main.cpp\
        furs_main_window.cpp \
    application.cpp \
    database_management.cpp \
    generate_letter.cpp

HEADERS  += furs_main_window.h \
    constants.h \
    application.h \
    database_management.h \
    generate_letter.h \
    furs_test.h

FORMS    += furs_main_window.ui
