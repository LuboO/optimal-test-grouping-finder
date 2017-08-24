TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/main.cpp \
    src/mysqldbreader.cpp \
    src/testgroup.cpp \
    src/constants.cpp

LIBS += \
    -lboost_system \
    -lmysqlcppconn

HEADERS += \
    src/atomictest.h \
    src/mysqldbreader.h \
    src/testgroup.h \
    src/failcountfreqeval.h \
    src/constants.h \
    src/chisquaretwosample.h
