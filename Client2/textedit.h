/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QMap>
#include <QColor>
#include <QMapIterator>
#include <QObject>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QScrollBar>
#include <QPainter>
#include "qpainter.h"
#include <QStringList>
#include <QDockWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include "qlabel.h"
#include "QUtente.h"
#include "QUser.h"
#include "WorkerSocketClient.h"
#include <QTextEdit>

//Includo classe per CRDT e rappresentazione dell'operazione sul documento
#include "CRDT.h"
#include "DocOperation.h"

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;
QT_END_NAMESPACE

#define BUFFER_DIMENSION 50
#define SIGNAL_LIMIT 200

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent, WorkerSocketClient *wscP,quint16 siteId, QUtente utente);
    void load(const QString &f);
    CRDT* getStrutturaCRDT();
    void loadCRDTIntoEditor(CRDT crdt);
    void restoreQTextEdit();
    void removeActions();

public:
     void setCurrentFileName(const QString &fileName);
     ~TextEdit() override;
     void updateUserInfo(QUtente utente);
     void cleanTextEdit();

signals:
        void formatChanged(const QTextCharFormat &format);
        void SigChiudiDoc(QString nomeFile);
        void SigOpDocLocale(DocOperation docOp);
        void SigOpDocLocaleBuffered(QList<DocOperation> opList);
        void SigOpChiHaInseritoCosa();
        void updateRecDocs();

public slots:
    void CRDTInsertRemove(int pos, int rem, int add);
    void updateTreeWidget(bool checked);
    void updateRemoteCursor();
    void opDocRemota(DocOperation operation);
    void opDocRemotaBuffered(QList<DocOperation> opList);
    void questoUserHaApertoIlDoc(QUser usr);
    void questoUserHaChiusoIlDoc(QUser usr);
    void esitoOpChiHaInseritoCosa(QList<QUser> users);

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();
    void clipboardDataChanged();
    void printPreview(QPrinter *);
    void pressedButtonTrigger(bool checked);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    void launchProfileEditor();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void remoteUserConnected(QString username);
    void remoteCursorMoved(QString username, int pos);
    void remoteUserDisconnected(QString username);
    void comunicaCRDTInserimentoLocale(QTextEdit* txe,QTextCursor* cursor, int pos, int numInserted,CRDT* algCRDT);
    void comunicaCRDTRimozioneLocale(int pos, int numRemoved,CRDT* algCRDT);
    void comunicaCRDTCambioFormat(QTextCharFormat format, int pos, int numCar,CRDT* algCRDT);
    void enteringColorMode();
    void quittingColorMode();
    void segnalaMovimentoCursore(QTextCursor cursor);

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *colorMode;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif
    const QString rsrc = ":/images/win";
    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QBoxLayout *userInfo;
    QComboBox *comboSize;
    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;
    QTextCursor *cursor;
    QMap<quint16, QTextCursor*> *cursorMap;
    QMap<quint16,QLabel*> *labelMap;
    QTreeWidget *usersTree;
    QDockWidget *dockUsersTree;
    QUtente utente;
    QToolBar *userInfoTb;
    QWidget *spacerWidgetUserInfo;
    QImage profileImage;
    QLabel *profileImageLabel;
    QLabel *usernameLabel;
    QToolBar *tbEdit;
    QMenu *menuEdit;
    QToolBar *tbFile;
    QToolBar *tbText;
    QToolBar *tbColor;
    QToolBar *tbFormat;
    //Aggiungo CRDT
    CRDT* algoritmoCRDT;
    quint16 siteId;
    //Aggiungo una lista di utenti online
    QList<QUser> *onlineUsers;
    //Lista utenti offline
    QList<QUser> *offlineUsers;
    //modalità scrittura a colori
    bool colorWriting;
    //Default format
    QTextCharFormat defaultFmt;
    WorkerSocketClient *wscP;
};

#endif // TEXTEDIT_H
