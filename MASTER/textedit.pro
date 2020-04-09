QT += network
QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = textedit

HEADERS         = textedit.h \
                  CRDT.h \
                  Char.h \
                  DocOperation.h \
                  QUser.h \
                  QUtente.h \
                  WorkerDoc.h \
                  clickablelabel.h \
                  editprofile.h \
                  loginDialog.h \
                  newprofiledialog.h \
                  recentdocsdialogs.h
SOURCES         = textedit.cpp \
                  CRDT.cpp \
                  Char.cpp \
                  DocOperation.cpp \
                  QUser.cpp \
                  QUtente.cpp \
                  WorkerDoc.cpp \
                  clickablelabel.cpp \
                  editprofile.cpp \
                  loginDialog.cpp \
                  main.cpp \
                  newprofiledialog.cpp \
                  recentdocsdialogs.cpp

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

EXAMPLE_FILES = textedit.qdoc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/textedit
INSTALLS += target

FORMS +=

DISTFILES +=
