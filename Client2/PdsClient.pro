QT       += core gui
QT       += network
QT       += widgets

requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = textedit


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
    BlockReader.cpp \
    BlockWriter.cpp \
    ClickableLabel.cpp \
    EditProfile.cpp \
    LoginDialog.cpp \
    NewProfileDialog.cpp \
    RecentDocsDialogs.cpp \
    TextEdit.cpp \
    main.cpp \
    Char.cpp \
    CRDT.cpp \
    DocOperation.cpp \
    QUser.cpp  \
    QUtente.cpp \
    WorkerSocketClient.cpp \
    ServerSelection.cpp \
    workerdocclient.cpp

HEADERS += \
    Char.h \
    CRDT.h \
    BlockReader.h \
    BlockWriter.h \
    ClickableLabel.h \
    DocOperation.h \
    EditProfile.h \
    LoginDialog.h \
    NewProfileDialog.h \
    QUser.h \
    QUtente.h \
    RecentDocsDialogs.h \
    TextEdit.h \
    WorkerSocketClient.h \
    ServerSelection.h \
    workerdocclient.h

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
    CONFIG -= app_bundle
}

EXAMPLE_FILES = textedit.qdoc

# Default rules for deployment.
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/textedit
INSTALLS += target

