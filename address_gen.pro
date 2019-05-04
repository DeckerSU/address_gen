#-------------------------------------------------
#
# Project created by QtCreator 2019-05-04T18:41:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = address_gen
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


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$PWD/depends_build/include
#LIBS += -L$$PWD/depends_build/lib -lbitcoin-system -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_iostreams -lboost_locale -lboost_log -lboost_program_options -lboost_regex -lboost_system -lboost_thread -lpthread -lrt -ldl
LIBS += -L$$PWD/depends_build/lib -Wl,-Bstatic -lbitcoin-system -lsecp256k1 -Wl,-Bdynamic -lgmp -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_iostreams -lboost_locale -lboost_log -lboost_program_options -lboost_regex -lboost_system -lboost_thread -lpthread -lrt -ldl
CONFIG += c++11
