QT = core

QT += core network

CONFIG += c++17 cmdline

DEFINES +=LOG4QT_STATIC

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DEFINES +=LOG4QT_STATIC

INCLUDEPATH += $$PWD/../../3rd/Log4Qt/src
include($$PWD/../../3rd/Log4Qt/src/log4qt/log4qt.pri)

INCLUDEPATH += $$PWD/../../Log

HEADERS += \
    ../../Log/logger.hpp
