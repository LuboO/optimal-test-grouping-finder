TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/main.cpp \
    src/mysqldbreader.cpp \
    src/testgroup.cpp

LIBS += \
    -lboost_system \
    -lmysqlcppconn

HEADERS += \
    src/atomictest.h \
    src/mysqldbreader.h \
    src/testgroup.h
