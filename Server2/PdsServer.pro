QT       += core gui
QT       += network
QT       += widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        Char.cpp \
        CRDT.cpp \
        DocOperation.cpp \
        QUser.cpp \
        QUtente.cpp \
        Server.cpp \
        WorkerDoc.cpp \
        BlockReader.cpp \
        BlockWriter.cpp \
        WorkerSocket.cpp \
        QTcpServerMio.cpp \
        QUtenteServer.cpp \
        operationbroadcaster.cpp \

HEADERS += \
        Char.h \
        CRDT.h \
        BlockReader.h \
        BlockWriter.h \
        DocOperation.h \
        QUser.h \
        QUtente.h \
        QUtenteServer.h \
        Server.h \
        WorkerDoc.h \
        WorkerSocket.h \
        QTcpServerMio.h \
        operationbroadcaster.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
