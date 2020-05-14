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
#include <QStringList>
#include <QDockWidget>
#include "QUtente.h"
#include "QUser.h"
#include "WorkerSocketClient.h"

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

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent, WorkerSocketClient *wscP,quint16 siteId);

    void load(const QString &f);

    CRDT* getStrutturaCRDT();
    void loadCRDTIntoEditor(CRDT crdt);

public:

     void setCurrentFileName(const QString &fileName);

    signals:
        void formatChanged(const QTextCharFormat &format);
        void SigChiudiDoc(QString nomeFile);
	
public slots:
    //void fileNew(QString fileName);
	void format(const QTextCharFormat &format);
    void CRDTInsertRemove(int pos, int rem, int add);

    void updateTreeWidget(bool checked);


	


    /*
     * manipolazione struttura CRDT conseguente all'esito ricevuto dal server e conseguenti ripercussioni sull'editor
     * NB se esito negativo devo fare UNDO della operazione locale fatta (qui la ricevo nuovamente, così so di quale si tratta)
     * */
    void esitoOpDocLocale(QString esito/*esito*/, DocOperation operation/*rappresentazione operazione sul documento*/);

    /*
     * manipolazione struttura CRDT conseguente all'operazione remota ricevuta dal server e conseguenti ripercussioni sull'editor
     * */
    void opDocRemota(DocOperation operation/*rappresentazione operazione sul documento*/);

    /*
     * mostra tale user come online, ovvero che sta editando ora il documento corrente
     * note:
     * va fatto altro, sulla base di come implementiamo le funzionalità di "mostrami gli username di chi ha inserito cosa" e
     * "mostrami gli username degli utenti ora online"
     */
    void questoUserHaApertoIlDoc(QUser usr);

    /*
     * smette di mostrare tale user come online
     * note:
     * va fatto altro, sulla base di come implementiamo le funzionalità di "mostrami gli username di chi ha inserito cosa" e
     * "mostrami gli username degli utenti ora online"
     */
    void questoUserHaChiusoIlDoc(QUser usr);


    /*
     * usa l'informazione ricevuta per implementare la funzione corrispondente
     * */

    //Non mi serve nessuna lista
    void esitoOpChiHaInseritoCosa(QList<QUser> users);

protected:
    void closeEvent(QCloseEvent *e) override;
    int isSuccess(QString esito);

signals:


    void SigOpDocLocale(DocOperation docOp);

    void SigOpChiHaInseritoCosa();

    void updateRecDocs();


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

    //slot che riceve segnale premuto bottone di Color Mode
    void pressedButtonTrigger(bool checked);

    /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    * */
    void esitoChiudiDoc(QString esito/*esito*/);


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
    void comunicaCRDTCambioFormat(QTextCursor* cursor, int pos, int numCar,CRDT* algCRDT);

    //entrata e uscita dalla color mode
    void enteringColorMode();
    void quittingColorMode();

    //emissione del segnale con DocOperation per movimento cursore
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

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;
    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;
    QTextCursor *cursor;
    QMap<quint16, QTextCursor> *cursorMap;
    QTreeWidget *usersTree;
    QDockWidget *dockUsersTree;
	
	 //Aggiungo CRDT
    CRDT* algoritmoCRDT;
    //Aggiungo una lista di utenti online
    QList<QUser> onlineUsers;

    //Lista utenti offline
    QList<QUser> offlineUsers;

    //modalità scrittura a colori
    bool colorWriting;

    WorkerSocketClient *wscP;

};

#endif // TEXTEDIT_H
