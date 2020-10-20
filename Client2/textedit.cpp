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

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QDebug>
#include "TextEdit.h"
const QString rsrcPath = ":/images/win";


//************************************************************
//
//Creates the TextEdit widget and connects all the signals
//with the respective slots
//
//************************************************************

TextEdit::TextEdit(QWidget *parent, WorkerSocketClient* wscP,quint16 siteId, QUtente utente)
    : QMainWindow(parent)
{
    //Inserisco inizializzazione del CRDT
    algoritmoCRDT = new CRDT(siteId);

    //inizialmente scrittura normale
    colorWriting = false;

    this->wscP = wscP;
    this->utente = utente;
    this->siteId = siteId;

    //Creazione dockWidget utenti online/offline

    usersTree = new QTreeWidget();
    dockUsersTree = new QDockWidget("Users");
    onlineUsers = new QList<QUser>();
    offlineUsers = new QList<QUser>();

    updateTreeWidget(false);
    setParent(parent);
    #ifdef Q_OS_OSX
        setUnifiedTitleAndToolBarOnMac(true);
    #endif
    setWindowTitle(QCoreApplication::applicationName());

    cursorMap = new QMap<quint16, QTextCursor*>();
    labelMap = new QMap<quint16,QLabel*>();
    textEdit = new QTextEdit(this);

    connect(textEdit, &QTextEdit::currentCharFormatChanged, this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &TextEdit::cursorPositionChanged);
    setCentralWidget(textEdit);
    addDockWidget(Qt::RightDockWidgetArea, dockUsersTree);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupFileActions();
    setupEditActions();
    setupTextActions();

    QFont textFont("Arial");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    textEdit->setFontPointSize(12);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());
    defaultFmt.setFont(QFont("Arial"));
    defaultFmt.setForeground(QColor("black"));
    defaultFmt.setFontPointSize(12);
    mergeFormatOnWordOrSelection(defaultFmt);

    connect(textEdit->document(), &QTextDocument::undoAvailable, actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable, actionRedo, &QAction::setEnabled);

    //connetto signal e slot che servono
    connect(textEdit->document(),&QTextDocument::contentsChange, this, &TextEdit::CRDTInsertRemove);

    /*chiusura documento*/
    QObject::connect(this, &TextEdit::SigChiudiDoc, wscP, &WorkerSocketClient::chiudiDoc);


    /*operazione locale sul documento*/
    QObject::connect(this, &TextEdit::SigOpDocLocale, wscP, &WorkerSocketClient::opDocLocale);
    QObject::connect(this, &TextEdit::SigOpDocLocaleBuffered, wscP, &WorkerSocketClient::opDocLocaleBuffered);

    /*operazione remota sul documento*/
    QObject::connect(wscP, &WorkerSocketClient::SigOpDocRemota, this,  &TextEdit::opDocRemota,Qt::QueuedConnection);
    QObject::connect(wscP, &WorkerSocketClient::SigOpDocRemotaBuffered, this,  &TextEdit::opDocRemotaBuffered,Qt::QueuedConnection);

    /*op chi ha inserito cosa*/
    QObject::connect(this, &TextEdit::SigOpChiHaInseritoCosa, wscP, &WorkerSocketClient::opChiHaInseritoCosa);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoOpChiHaInseritoCosa, this,  &TextEdit::esitoOpChiHaInseritoCosa);

    connect(textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &TextEdit::updateRemoteCursor);

    setWindowModified(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(this, SIGNAL(updateRecDocs()), parentWidget(), SLOT(updateRecDocs()));


#ifndef QT_NO_CLIPBOARD
    actionCut->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);
#endif

    setCurrentFileName(QString());

#ifdef Q_OS_MACOS
    // Use dark text on light background on macOS, also in dark mode.
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    textEdit->setPalette(pal);
#endif
    textEdit->setWordWrapMode(QTextOption::WrapAnywhere);
}

TextEdit::~TextEdit(){
    delete this->textEdit;
    delete this->algoritmoCRDT;
    delete this->cursorMap;
    delete this->labelMap;
    delete this->cursor;
    delete this->usersTree;
    delete this->dockUsersTree;
    delete this->onlineUsers;
    delete this->offlineUsers;
}

//***********************************
//
//Returns the value of the locally
//stored CRDT structure
//
//***********************************

CRDT* TextEdit::getStrutturaCRDT(){
    return algoritmoCRDT;
}

//***********************************
//
//Load the content of the CRDT structure
//into the text editor
//
//***********************************

void TextEdit::loadCRDTIntoEditor(CRDT crdt){
    if (algoritmoCRDT!=nullptr){
        delete this->algoritmoCRDT;
        algoritmoCRDT=nullptr;
    }
  algoritmoCRDT = new CRDT(this->siteId,crdt.getListChar()); //salvo nel CRDT la rappresentazione del file
  // devo andare ad aggiornare il contenuto del QTextEdit tramite l'uso di cursori sulla base di quello che c'è scritto nel CRDT
  int currentIndex = 0;
  this->cursor = new QTextCursor(textEdit->textCursor());
  this->cursor->setPosition(currentIndex);
  auto lista = crdt.getListChar();
  disconnect(textEdit->document(),&QTextDocument::contentsChange,
          this, &TextEdit::CRDTInsertRemove );
  disconnect(textEdit, &QTextEdit::cursorPositionChanged,
          this, &TextEdit::cursorPositionChanged);
  for (auto richChar = lista.cbegin(); richChar!=lista.cend(); richChar++ ){
    QString str = "";
    Char ch = *richChar;
    if(ch.getValue()!='\0'){
        str.append(ch.getValue());
        this->cursor->insertText(str,ch.getFormat());
    }
    if(ch.getAlign()!=0){
        this->textEdit->textCursor().setPosition(currentIndex);
        if (ch.getAlign()==1){
            textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        }
        else if (ch.getAlign()==2){
            textEdit->setAlignment(Qt::AlignHCenter);
        }
        else if (ch.getAlign()==3){
            textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        }
        else if (ch.getAlign()==4){
            textEdit->setAlignment(Qt::AlignJustify);
        }
    }
  }
  currentIndex++;
  connect(textEdit->document(),&QTextDocument::contentsChange,
          this, &TextEdit::CRDTInsertRemove);
  connect(textEdit, &QTextEdit::cursorPositionChanged,
          this, &TextEdit::cursorPositionChanged);
}

//**********************************************************
//
//Emits the necessary signals when the red cross is pressed.
//The event is ignored because it'll be managed by another slot
//
//***********************************************************

void TextEdit::closeEvent(QCloseEvent *e)
{
   if(parentWidget() != NULL){
        emit(SigChiudiDoc(this->fileName));
        emit(updateRecDocs());
        e->ignore();
   }
   else
       e->ignore();
}

//************************************
//
//Sets up the File Actions toolbar
//
//************************************

void TextEdit::setupFileActions()
{
    tbFile = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));
    QAction *a;

#ifndef QT_NO_PRINTER
    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    tbFile->addAction(a);
    menu->addSeparator();
#endif
    a = menu->addAction(tr("&Quit"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
}

//************************************
//
//Sets up the Edit Actions toolbar
//
//************************************

void TextEdit::setupEditActions()
{
    tbEdit = addToolBar(tr("Edit Actions"));
    menuEdit = menuBar()->addMenu(tr("&Edit"));
    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menuEdit->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tbEdit->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menuEdit->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tbEdit->addAction(actionRedo);
    menuEdit->addSeparator();

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menuEdit->addAction(cutIcon, tr("Cu&t"), textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tbEdit->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menuEdit->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tbEdit->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menuEdit->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tbEdit->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

//************************************
//
//Sets up the Text Actions toolbar
//
//************************************

void TextEdit::setupTextActions()
{
    tbText = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Format"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tbText->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tbText->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tbText->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);


    // Make sure the alignLeft is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &TextEdit::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tbText->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tbText->addAction(actionTextColor);

    tbText->addSeparator();

    userInfoTb = addToolBar(tr("UserInfo"));
    userInfoTb->setMovable(false);
    spacerWidgetUserInfo = new QWidget(this);
    spacerWidgetUserInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidgetUserInfo->setVisible(true);
    userInfoTb->addWidget(spacerWidgetUserInfo);
    if(!utente.getNomeImg().isEmpty() || !utente.getNomeImg().isNull())
        profileImage = QImage(utente.getNomeImg());
    else
        profileImage = QImage(rsrc+"/colored-edit-profile.png");

    profileImageLabel = new QLabel();
    profileImageLabel->setPixmap(QPixmap::fromImage(profileImage).scaled(50,50, Qt::AspectRatioMode::KeepAspectRatio));
    profileImageLabel->setMargin(5);
    usernameLabel = new QLabel(utente.getUsername());
    usernameLabel->setMargin(5);
    userInfoTb->addWidget(profileImageLabel);
    userInfoTb->addWidget(usernameLabel);

    tbColor = addToolBar(tr("Color Mode"));
    QIcon colorModeIcon = QIcon(rsrcPath + "/colormode.png");
    colorMode = new QAction(colorModeIcon, tr("&ColorMode"), this);
    colorMode->setCheckable(true);
    colorMode->setShortcut(Qt::CTRL + Qt::Key_F);
    colorMode->setPriority(QAction::LowPriority);
    QWidget *spacerWidget2 = new QWidget(this);
    spacerWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget2->setVisible(true);
    tbColor->addWidget(spacerWidget2);
    tbColor->addAction(colorMode);
    tbColor->setMovable(false);


    //connetto il segnale allo slot che fa update del valore booleano
    connect(colorMode, &QAction::triggered, this, &TextEdit::pressedButtonTrigger);

    tbFormat = addToolBar(tr("Format Actions"));
    tbFormat->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tbFormat);
    addToolBar(tbColor);

    comboFont = new QFontComboBox(tbFormat);
    tbFormat->addWidget(comboFont);
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);

    comboSize = new QComboBox(tbFormat);
    comboSize->setObjectName("comboSize");
    tbFormat->addWidget(comboSize);
    comboSize->setEditable(true);
    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
}

//***********************************************
//
//Sets up the current file name based on the content
//of the arg received
//
//***********************************************

void TextEdit::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

//************************************
//
//Slot that allows the user to export
//the file into a PDF.
//
//************************************

void TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}

//*****************************
//
//Slot responsible of setting
//the bold format
//
//*****************************

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//*****************************
//
//Slot responsible of setting
//the underline format
//
//*****************************

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//*****************************
//
//Slot responsible of setting
//the italic format
//
//*****************************

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//*****************************
//
//Slot responsible of setting
//the font family
//
//*****************************

void TextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);}

//*****************************
//
//Slot responsible of setting
//the text size
//
//*****************************

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    if (p.toFloat() > 0){
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
    for (auto lbl:labelMap->values()){
        lbl->setFixedSize(3,pointSize);
    }
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//*****************************
//
//Slot responsible of setting
//the style of the text
//
//*****************************

void TextEdit::textStyle(int styleIndex)
{
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        style = QTextListFormat::ListDecimal;
        break;
    case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();
    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);
        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        textEdit->mergeCurrentCharFormat(fmt);
    }else{
        QTextListFormat listFmt;
        if (cursor.currentList()){
            listFmt = cursor.currentList()->format();
        }else{
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }
    cursor.endEditBlock();
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//*****************************
//
//Slot responsible of setting
//the text color
//
//*****************************

void TextEdit::textColor()
{
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//*****************************
//
//Slot responsible of setting
//the text alignment
//
//*****************************

void TextEdit::textAlign(QAction *a)
{
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    quint16 alignementType=0;
    auto cursorPos = textEdit->textCursor().block().position();
    if (a == actionAlignLeft){
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        alignementType=1;
    }
    else if (a == actionAlignCenter){
        textEdit->setAlignment(Qt::AlignHCenter);
        alignementType=2;
    }
    else if (a == actionAlignRight){
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        alignementType=3;
    }
    else if (a == actionAlignJustify){
        textEdit->setAlignment(Qt::AlignJustify);
        alignementType=4;
    }
    //cursorPos = textEdit->textCursor().position();
    algoritmoCRDT->setCharAlign(alignementType,cursorPos);
    for(auto cursor: cursorMap->values()){
        QRect rect = textEdit->cursorRect(*cursor);
        auto key = cursorMap->key(cursor);
        labelMap->find(key).value()->move(rect.left(),rect.top());
        labelMap->find(key).value()->setFixedSize(3,textEdit->fontPointSize());
    }
    emit SigOpDocLocale(DocOperation(cursorPos,alignementType,this->siteId));
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
}

//***************************************
//
//The slot is responsible of managing the
//signal emitted when the format of the
//current char has changed.
//
//***************************************

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format){

    if(textEdit->document()->characterCount() == 1){
        if(format.fontPointSize() != 0)
            defaultFmt.setFont(format.font());
        defaultFmt.setFontPointSize(12);
        fontChanged(defaultFmt.font());
        colorChanged(defaultFmt.foreground().color());
        mergeFormatOnWordOrSelection(defaultFmt);
    }else{
        fontChanged(format.font());
        colorChanged(format.foreground().color());
    }
}

//********************************************************************************
//
//Funzione per creare una DocOperation che segnala il movimento del cursore
//Riceve come parametro il cursore ed emette un segnale con la DocOperation
//Nella DocOperation i parametri che non vengono utilizzati vengono settati a null
//
//********************************************************************************

void TextEdit::segnalaMovimentoCursore(QTextCursor cursor){
   emit SigOpDocLocale(DocOperation(cursorMoved,Char(),QTextCharFormat(),this->algoritmoCRDT->getSiteID(),cursor.position(),cursor.anchor()));
}

//*****************************************
//
//Slot that manages the position changing
//of a the cursor
//
//*****************************************

void TextEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
    segnalaMovimentoCursore(textEdit->textCursor());
}

//******************************************
//
//Slot which manages the changing of the
//clipboard content.
//
//******************************************

void TextEdit::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}


void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);

    cursor.mergeCharFormat(format);
    //Comunico al CRDT il cambio di formato
    int posCursor = cursor.position();
    int posAnchor = cursor.anchor();
    int changed = abs(cursor.anchor() - cursor.position());
    comunicaCRDTCambioFormat(format,posCursor<posAnchor ? posCursor:posAnchor,changed,algoritmoCRDT);
    textEdit->mergeCurrentCharFormat(format);
}

//**************************************
//
//Slot that manages the changing of the
//font
//
//**************************************

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

//*******************************
//
//Slot that updates the color into
//the color selection icon
//whenever the text color changes
//
//*******************************

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

//***********************************************
//
//Slot that manages the changing in text alignment
//
//***********************************************

void TextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

//************************************************
//
//Slot that communicates to the remote CRDT a local
//insertion
//
//************************************************


void TextEdit::comunicaCRDTInserimentoLocale(QTextEdit* txe,QTextCursor* cursor, int pos, int numInserted,CRDT* algCRDT){
    bool bufferMode = numInserted > SIGNAL_LIMIT;
    QList<DocOperation> opList;
    int numOperationTreated = 0;
    auto inserted = txe->document()->toPlainText().mid(pos,numInserted);
    for (int i=0;i<numInserted;i++){
        QTextCharFormat format = textEdit->currentCharFormat();
        if (format.isEmpty()){
            format = defaultFmt;
        }else{
            if(format.fontFamily().isEmpty())
                format.setFontFamily(defaultFmt.fontFamily());
            if(format.fontPointSize()<=0)
                format.setFontPointSize(defaultFmt.fontPointSize());
        }
        QChar ch = inserted[i];
        DocOperation docOp = algCRDT->localInsert(ch,format,pos+i);
        if (bufferMode){
            opList.append(docOp);
            numOperationTreated++;
            if(numOperationTreated >= BUFFER_DIMENSION || i==numInserted-1){
                emit SigOpDocLocaleBuffered(opList);
                opList.clear();
                numOperationTreated = 0;
            }
        }else
            emit SigOpDocLocale(docOp);
        if (colorWriting){
            format.setForeground(QBrush(QColor("black")));
            auto colors = QColor::colorNames();
            auto *noWhiteyColors = new QStringList();
            auto colorIterator = colors.begin();
            while(colorIterator != colors.end()){
                if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
                    noWhiteyColors->append(*colorIterator);
                }
                colorIterator++;
            }
            colors = *noWhiteyColors;
            delete noWhiteyColors;
            QTextCharFormat fmt;
            fmt.setForeground(QColor(colors[utente.getUserId()]));
            cursor->setPosition(pos+i,QTextCursor::MoveAnchor);
            cursor->movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
            cursor->mergeCharFormat(fmt);
        }
    }
}

//************************************************
//
//Slot that communicates to the remote CRDT a local
//removal
//
//************************************************

void TextEdit::comunicaCRDTRimozioneLocale(int pos, int numRemoved,CRDT* algCRDT){
    bool bufferMode = numRemoved > SIGNAL_LIMIT;
    QList<DocOperation> opList;
    int numOperationTreated = 0;
    for(int i=0;i<numRemoved;i++){
        DocOperation docOp = algCRDT->localErase(pos);
        if (bufferMode){
            opList.append(docOp);
            numOperationTreated++;
            if (docOp.character.getValue()=='\n'){
                opList.append(DocOperation(pos,0,this->siteId));
                numOperationTreated++;
            }
            if(numOperationTreated >= BUFFER_DIMENSION || i==numRemoved-1){
                emit SigOpDocLocaleBuffered(opList);
                opList.clear();
                numOperationTreated = 0;
            }
        }else{
            emit SigOpDocLocale(docOp);
            if (docOp.character.getValue()=='\n'){
                emit SigOpDocLocale(DocOperation(pos,0,this->siteId));
            }
        }
    }
}


//************************************************
//
//Slot that communicates to the remote CRDT a local
//format change
//
//************************************************

void TextEdit::comunicaCRDTCambioFormat(QTextCharFormat format, int pos, int numCar,CRDT* algCRDT){
    if (colorWriting){
        format.setForeground(QBrush(QColor("black")));
    }
    for(int i=0;i<numCar;i++){
        DocOperation docOp = algCRDT->localFormatChange(format,pos+i);
        emit SigOpDocLocale(docOp);
    }
}

//****************************************************
//
//This slot manages in the right way a local insertion
//or removal calling the necessary methods with
//the right parameters
//
//****************************************************

void TextEdit::CRDTInsertRemove(int pos, int rem, int add){
    QTextCursor cursor = textEdit->textCursor();
    if(rem==0 && add>0){
        //AGGIUNTA DI UNO O PIU' CARATTERI
        comunicaCRDTInserimentoLocale(textEdit,&cursor,pos,add,algoritmoCRDT);
    }
    else if(rem>0 && add==0){
        //CANCELLAZIONE DI UNO O PIU' CARATTERI
        comunicaCRDTRimozioneLocale(pos,rem,algoritmoCRDT);
    }
    else if(add>0 && rem>0){
        if(add==rem){
            //DOPO IL PASTE DA PARTE DI UN UTENTE SU UNA SELEZIONE, LA SELEZIONE DEL CURSORE SCOMPARE
            //Controllando questa quindi possiamo capire se si tratta di una di un copia/incolla sopra una selezione oppure di un cambio format.
//            cursor.select(QTextCursor::WordUnderCursor);
            auto selection = cursor.selectedText();
            std::cout<<"Testo selezionato: "<< selection.toStdString().c_str()<<"\n"<<std::flush;
            if((selection.isEmpty())){
                //Si tratta di un cambio di formato, quindi ottengo il formato e lo comunico al CRDT
                //comunicaCRDTCambioFormat(&cursor,pos,add);
            }
            else{
                //Caso in cui si incolla su una selezione una stringa della stessa lunghezza
                //Non gestisco il caso particolare in cui la stringa incollata sia la stessa di quella cancellata
                //Pros: rimuovo controllo           Cons: faccio cancellazione e inserimento anche in quel caso
                comunicaCRDTInserimentoLocale(textEdit,&cursor,pos,add,algoritmoCRDT);
                comunicaCRDTRimozioneLocale(pos+add-1,rem,algoritmoCRDT);            }
        }
        else{
            if(pos==0){
                comunicaCRDTInserimentoLocale(textEdit,&cursor,pos,add,algoritmoCRDT);
                comunicaCRDTRimozioneLocale(pos+add-1,rem,algoritmoCRDT);
            }
            else{
                //Caso in cui si incolla una stringa sopra una selezione
                comunicaCRDTInserimentoLocale(textEdit,&cursor,pos,add,algoritmoCRDT);
                comunicaCRDTRimozioneLocale(pos+add,rem,algoritmoCRDT);
            }
        }
    }
}

//***************************************************************************
//
// manipolazione bufferizzata della struttura CRDT conseguente all'operazione
// remota ricevuta dal server e conseguenti ripercussioni sull'editor
//
//***************************************************************************

void TextEdit::opDocRemotaBuffered(QList<DocOperation> opList){
    while(!opList.isEmpty()){
        DocOperation op = opList.first();
        opList.pop_front();
        opDocRemota(op);
    }
}

//****************************************************
//
// manipolazione struttura CRDT conseguente all'operazione
// remota ricevuta dal server e conseguenti ripercussioni sull'editor
//
//****************************************************

void TextEdit::opDocRemota(DocOperation operation){
   textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
   disconnect(textEdit->document(), &QTextDocument::contentsChange, this, &TextEdit::CRDTInsertRemove );
   switch(operation.type){
    case remoteInsert:
    {
      quint32 index = algoritmoCRDT->remoteInsert(operation.character);
      QTextCursor *cursor = cursorMap->find(operation.siteId).value();
      auto colors = QColor::colorNames();
      auto *noWhiteyColors = new QStringList();
      auto colorIterator = colors.begin();
      while(colorIterator != colors.end()){
          if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
              noWhiteyColors->append(*colorIterator);
          }
          colorIterator++;
      }
      colors = *noWhiteyColors;
      delete noWhiteyColors;
      cursor->setPosition(index);
      cursor->beginEditBlock();
      if (colorWriting == true){
          QTextCharFormat coloredFormat(operation.character.getFormat());
          coloredFormat.setForeground(QBrush(QColor(colors[operation.character.getSiteId()])));
          cursor->mergeCharFormat(coloredFormat);
          cursor->insertText(operation.character.getValue());
          //textEdit->setTextColor(QColor(colors[this->siteId]));
      }
      else{
          cursor->insertText(operation.character.getValue(),operation.character.getFormat());
      }
      cursor->endEditBlock();
      break;
    }
    case remoteDelete:
    {
      quint32 index =algoritmoCRDT->remoteDelete(operation.character);
      QTextCursor *cursor = cursorMap->find(operation.siteId).value();
      cursor->setPosition(index);
      cursor->beginEditBlock();
      cursor->deleteChar();
      cursor->endEditBlock();
      break;
   }
    case changedFormat:
   {
       if (colorWriting == true){
           auto colors = QColor::colorNames();
           auto *noWhiteyColors = new QStringList();
           auto colorIterator = colors.begin();
           while(colorIterator != colors.end()){
               if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
                   noWhiteyColors->append(*colorIterator);
               }
               colorIterator++;
           }
           colors = *noWhiteyColors;
           delete noWhiteyColors;

           QTextCharFormat coloredFormat(operation.character.getFormat());
           coloredFormat.setForeground(QBrush(QColor(colors[operation.character.getSiteId()])));
           operation.character.setFormat(coloredFormat);
       }
      quint32 index = algoritmoCRDT->remoteFormatChange(operation.character);
      QTextCursor *cursor = cursorMap->find(operation.siteId).value();
      cursor->setPosition(index);
      cursor->setPosition(index+1,QTextCursor::KeepAnchor);
      cursor->beginEditBlock();
      cursor->mergeCharFormat(operation.character.getFormat());
      cursor->endEditBlock();
      QRect rect_ = textEdit->cursorRect(*cursor);

      for(auto label : labelMap->toStdMap()){
        label.second->setFixedSize(2,rect_.bottom()-rect_.top());
        label.second->adjustSize();
      }
      break;
   }
   case cursorMoved:
   {
       QTextCursor *cursor = cursorMap->find(operation.siteId).value();
       cursor->setPosition(operation.cursorPos, QTextCursor::MoveAnchor);
       QRect rect = textEdit->cursorRect(*cursor);
       labelMap->find(operation.getSiteId()).value()->move(rect.left(),rect.top());
       labelMap->find(operation.getSiteId()).value()->setFixedSize(2,rect.bottom()-rect.top());
       break;
    }
   case alignementChanged:
   {
       QTextCursor *cursor = cursorMap->find(operation.siteId).value();
       QTextCursor oldCursor = textEdit->textCursor();
       cursor->setPosition(operation.cursorPos);
       textEdit->setTextCursor(*cursor);

       switch(operation.alignementType){
       case 1:
       {
           textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
           break;
       }
       case 2:
       {
           textEdit->setAlignment(Qt::AlignHCenter);
           break;
       }
       case 3:
       {
           textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
           break;
       }
       case 4:
       {
           textEdit->setAlignment(Qt::AlignJustify);
           break;
       }
       }
       textEdit->setTextCursor(oldCursor);

       QRect rect = textEdit->cursorRect(*cursor);
       labelMap->find(operation.getSiteId()).value()->move(rect.left(),rect.top());
       labelMap->find(operation.getSiteId()).value()->setFixedSize(3,textEdit->fontPointSize());
       break;
   }


 }
   connect(textEdit->document(),&QTextDocument::contentsChange,
                  this, &TextEdit::CRDTInsertRemove);
   textEdit->setTextInteractionFlags(Qt::TextEditorInteraction);
}


//*******************************************************
//
//mostra tale user come online, ovvero che sta editando
//ora il documento corrente
//
//********************************************************

void TextEdit::questoUserHaApertoIlDoc(QUser usr){
    onlineUsers->append(usr);
    if(offlineUsers->contains(usr))
        offlineUsers->removeAll(usr);
    updateTreeWidget(colorWriting);

    auto colors = QColor::colorNames();
    auto *noWhiteyColors = new QStringList();
    auto colorIterator = colors.begin();
    while(colorIterator != colors.end()){
        if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
            noWhiteyColors->append(*colorIterator);
        }
        colorIterator++;
    }
    colors = *noWhiteyColors;
    delete noWhiteyColors;

    //Cursore dell'utente appena loggato al documento
    QTextCursor *cursor =  new QTextCursor(textEdit->document());
    cursorMap->insert(usr.getUserId(),cursor);
    //qDebug()<<"Inserito cursore per utente: "<<usr.getUserId();
    QLabel *lbl = new QLabel("",textEdit);
    labelMap->insert(usr.getUserId(),lbl);
    disconnect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);
    cursor->setPosition(0);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);
    QRect rect = textEdit->cursorRect(*cursor);
    //qDebug()<<textEdit->fontPointSize();
    lbl->setFrameStyle(QFrame::VLine | QFrame::Plain);
    lbl->setFixedSize(3,textEdit->fontPointSize());
    lbl->move(rect.left(),rect.top());
    lbl->raise();
   // lbl->setFont(textEdit->font());
    lbl->setMargin(3);
    lbl->setStyleSheet("QLabel { background-color: "+colors[usr.getUserId()]+"; color: "+colors[usr.getUserId()]+"}");
    lbl->show();
}

//*******************************************************
//
//mostra tale user come offline, ovvero che non sta più editando
//il documento corrente
//
//********************************************************


void TextEdit::questoUserHaChiusoIlDoc(QUser usr){
    onlineUsers->removeAll(usr);
    if(!offlineUsers->contains(usr))
        offlineUsers->append(usr);
    updateTreeWidget(colorWriting);
    delete cursorMap->find(usr.getUserId()).value();
    cursorMap->remove(usr.getUserId());
    labelMap->find(usr.getUserId()).value()->clear();
    delete labelMap->find(usr.getUserId()).value();
    labelMap->remove(usr.getUserId());
}

//**********************************************
//
//Slot which manages the enabling of the color mode
//
//**********************************************

void TextEdit:: enteringColorMode(){
    if (colorWriting==false){
        //non dovrebbe mai entrare qui perchè deve essere attivata solo quando la variabile è true
        std::cerr << "Expected variable value:true, while it's false"<<std::flush;
        return;
    }
    actionTextColor->setDisabled(true);
    auto colors = QColor::colorNames();
    auto *noWhiteyColors = new QStringList();
    auto colorIterator = colors.begin();
    while(colorIterator != colors.end()){
        if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
            noWhiteyColors->append(*colorIterator);
        }
        colorIterator++;
    }
    colors = *noWhiteyColors;
    delete noWhiteyColors;
    QTextCursor* colorCursor = new QTextCursor(textEdit->textCursor());
    colorCursor->setPosition(0);

    //Devo disconnettere e riconnettere il segnale per non far arrivare un segnale allo slot
    //che sto modificando il formato del testo
    QObject::disconnect(textEdit->document(),&QTextDocument::contentsChange, this, &TextEdit::CRDTInsertRemove );

    updateTreeWidget(colorWriting); //deve essere true per forza
    for (auto character:algoritmoCRDT->getListChar()){
        QTextCharFormat* format = new QTextCharFormat();
        format->setForeground( QBrush( QColor(colors[character.getSiteId()]) ) );
        colorCursor->movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,1);
        colorCursor->mergeCharFormat(*format);
        colorCursor->clearSelection();
    }

    //setto il colore del cursore corretto in base all'id del mio client
    //in modo che da ora in avanti sul client si scriva con quel colore
    auto cursorFormat = textEdit->textCursor().charFormat();
    cursorFormat.setForeground(QBrush(QColor(colors[this->algoritmoCRDT->getSiteID()])));
    textEdit->setCurrentCharFormat(cursorFormat);

    delete colorCursor;
    QObject::connect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove);
}

//********************************************
//
//Slot which manages the closure of the color
//mode
//
//********************************************

void TextEdit::quittingColorMode(){
    if (colorWriting==true){
        //non dovrebbe mai entrare qui perchè deve essere attivata solo quando la variabile è false
        std::cerr << "Expected variable value:false, while it's true"<<std::flush;
        return;
    }
    actionTextColor->setEnabled(true);
    auto colors = QColor::colorNames();
    auto *noWhiteyColors = new QStringList();
    auto colorIterator = colors.begin();
    while(colorIterator != colors.end()){
        if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
            noWhiteyColors->append(*colorIterator);
        }
        colorIterator++;
    }
    colors = *noWhiteyColors;
    delete noWhiteyColors;

    QTextCursor* colorCursor = new QTextCursor(textEdit->textCursor());
    QObject::disconnect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove );
    //deve essere false per forza
    updateTreeWidget(colorWriting);
    offlineUsers->clear();
    //caso in cui ci troviamo nella color mode e l'utente preme di nuovo il bottone per togliere la modalità
    //torniamo alla modalità in cui tutti i caratteri sono neri
    QTextCharFormat format;
    format.setForeground( QBrush( QColor("black") ) );
    colorCursor->select(QTextCursor::Document);
    colorCursor->mergeCharFormat(format);

    //rimetto il colore del cursore a nero lasciando invariato il resto del formato
    auto cursorFormat = textEdit->textCursor().charFormat();
    cursorFormat.setForeground(QBrush(QColor("black")));
    textEdit->setCurrentCharFormat(cursorFormat);

    delete colorCursor;
    QObject::connect(textEdit->document(),&QTextDocument::contentsChange,
        this, &TextEdit::CRDTInsertRemove);
}

//*******************************************
//
//Populates onlineUsers and offlineUsers
//based on the data received from the server
//
//*******************************************

void TextEdit::esitoOpChiHaInseritoCosa(QList<QUser> users){
    for (auto user : users){
        if (!onlineUsers->contains(user) && user.getUserId()!=this->algoritmoCRDT->getSiteID() && !offlineUsers->contains(user))
            offlineUsers->append(user);
    }
    enteringColorMode();
}

//*************************************
//
//This slot manages the variable
//colorWriting, setting it based on the
//colorMode button state, and emits
//the necessary signals
//
//*************************************

void TextEdit::pressedButtonTrigger(bool checked){
    colorWriting = checked;
    if (checked==true){ // solamente se entriamo nella color Mode
    //emetto segnale per inviare la richiesta al server di ricevere la lista con storico utenti
        emit SigOpChiHaInseritoCosa();
    }
    else{
        quittingColorMode();
    }
}

//***********************************
//
//Aggiorna il dockWidget degli utenti
//online/offline.
//In caso di color mode abilitata
//a fianco di ogni nome utente verrà
//visualizzata un'icona colorata
//con il colore assegnato all'utente
//in modo da creare una legenda colori
//per l'utente che sta modificando il
//documento. Nel caso la color mode sia
//abilitata, vengono mostrati anche
//gli utenti offline che hanno
//modificato precedentemente il documento
//permettendo di avere così uno storico
//completo delle modifiche
//
//***********************************

void TextEdit::updateTreeWidget(bool checked){

    usersTree->clear();
    usersTree->setHeaderHidden(true);

    if(checked){
        auto colors = QColor::colorNames();
        auto *noWhiteyColors = new QStringList();
        auto colorIterator = colors.begin();
        while(colorIterator != colors.end()){
            if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
                noWhiteyColors->append(*colorIterator);
            }
            colorIterator++;
        }
        colors = *noWhiteyColors;
        delete noWhiteyColors;
        usersTree->setColumnCount(1);

        QList<QString> *usernameOnline = new QList<QString>();
        QList<QColor> *colorList = new QList<QColor>();
        QList<QTreeWidgetItem*> *itemUsernameOnline = new QList<QTreeWidgetItem*>();
        QList<QUser>::iterator i;
        for(i=onlineUsers->begin(); i != onlineUsers->end(); ++i){
            usernameOnline->append(i->getUserName());
            colorList->append(colors[i->getUserId()]);
        }

        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Online")));
        usersTree->insertTopLevelItem(0, item);
        QList<QString>::iterator j;
        QList<QColor>::iterator z;
        z=colorList->begin();
        for(j=usernameOnline->begin(); j!=usernameOnline->end(); ++j){
            QTreeWidgetItem *utenteOnline = new QTreeWidgetItem((QTreeWidget*)0, QStringList(*j));
            QPixmap pix(16, 16);
            pix.fill(*z);
            utenteOnline->setIcon(0,QIcon(pix));
            itemUsernameOnline->append(utenteOnline);
            z++;
        }

        item->insertChildren(0,*itemUsernameOnline);

        if(!offlineUsers->isEmpty()){
            QList<QColor> *offlineColorList = new QList<QColor>();
            QTreeWidgetItem *item2 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Offline")));
            usersTree->insertTopLevelItem(0, item2);
            QList<QString> *usernameOffline = new QList<QString>();
            QList<QTreeWidgetItem*> *itemUsernameOffline = new QList<QTreeWidgetItem*>();
            QList<QUser>::iterator i;

            for(i=offlineUsers->begin(); i != offlineUsers->end(); ++i){
                usernameOffline->append(i->getUserName());
                offlineColorList->append(colors[i->getUserId()]);
            }

            QList<QString>::iterator j;
            z = offlineColorList->begin();
            for(j=usernameOffline->begin(); j!=usernameOffline->end(); ++j){
                QTreeWidgetItem *utenteOffline = new QTreeWidgetItem((QTreeWidget*)0, QStringList(*j));
                QPixmap pix(16, 16);
                pix.fill(*z);
                utenteOffline->setIcon(0,QIcon(pix));
                itemUsernameOffline->append(utenteOffline);
                z++;
            }
          item2->insertChildren(0,*itemUsernameOffline);
        }
    }else{
        auto colors = QColor::colorNames();
        auto *noWhiteyColors = new QStringList();
        auto colorIterator = colors.begin();
        while(colorIterator != colors.end()){
            if(QString("#%1").arg(QColor(*colorIterator).rgba(), 8, 16) < "#ff555555" && *colorIterator != "black"){
                noWhiteyColors->append(*colorIterator);
            }
            colorIterator++;
        }
        colors = *noWhiteyColors;
        delete noWhiteyColors;

        usersTree->setColumnCount(1);

        QList<QString> *usernameOnline = new QList<QString>();
        QList<QColor> *colorList = new QList<QColor>();
        QList<QTreeWidgetItem*> *itemUsernameOnline = new QList<QTreeWidgetItem*>();
        QList<QUser>::iterator i;
        for(i=onlineUsers->begin(); i != onlineUsers->end(); ++i){
            usernameOnline->append(i->getUserName());
            colorList->append(colors[i->getUserId()]);
        }

        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Online")));
        usersTree->insertTopLevelItem(0, item);
        QList<QString>::iterator j;
        QList<QColor>::iterator z;
        z=colorList->begin();
        for(j=usernameOnline->begin(); j!=usernameOnline->end(); ++j){
            QTreeWidgetItem *utenteOnline = new QTreeWidgetItem((QTreeWidget*)0, QStringList(*j));
            QPixmap pix(16, 16);
            pix.fill(*z);
            utenteOnline->setIcon(0,QIcon(pix));
            itemUsernameOnline->append(utenteOnline);
            z++;
        }
        item->insertChildren(0,*itemUsernameOnline);
    }
    dockUsersTree->setWidget(usersTree);
}

//********************************************
//
//Updates the userInfo toolbar content
//
//********************************************

void TextEdit::updateUserInfo(QUtente utente){

    if(!utente.getNomeImg().isEmpty() || !utente.getNomeImg().isNull())
        profileImage = QImage(utente.getNomeImg());
    else
        profileImage = QImage(rsrc+"/colored-edit-profile.png");

    profileImageLabel->setPixmap(QPixmap::fromImage(profileImage).scaled(50,50, Qt::AspectRatioMode::KeepAspectRatio));
    profileImageLabel->setMargin(5);
    usernameLabel->setText(utente.getUsername());
    usernameLabel->setMargin(5);
}

//************************************
//
//Restores the TextEdit to a clean state
//
//************************************

void TextEdit::restoreQTextEdit(){
    //Inserisco inizializzazione del CRDT
    algoritmoCRDT = new CRDT(siteId);
    //inizialmente scrittura normale
    colorWriting = false;

    //Creazione dockWidget utenti online/offline

    usersTree = new QTreeWidget();
    dockUsersTree = new QDockWidget("Users");
    onlineUsers = new QList<QUser>();
    offlineUsers = new QList<QUser>();
    updateTreeWidget(false);

    cursorMap = new QMap<quint16, QTextCursor*>();
    labelMap->clear();

    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);
    setCentralWidget(textEdit);
    addDockWidget(Qt::RightDockWidgetArea, dockUsersTree);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);

    setupFileActions();
    setupEditActions();
    setupTextActions();

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    textEdit->setFontPointSize(12);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());
    defaultFmt.setFontPointSize(12);
    defaultFmt.setFont(textEdit->font());
    defaultFmt.setForeground(textEdit->textColor());

    connect(textEdit->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);

    //connetto signal e slot che servono
    connect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove);

    connect(textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &TextEdit::updateRemoteCursor);


    setWindowModified(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

#ifndef QT_NO_CLIPBOARD
    actionCut->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);
#endif

#ifdef Q_OS_MACOS
    // Use dark text on light background on macOS, also in dark mode.
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    textEdit->setPalette(pal);
#endif
    textEdit->setWordWrapMode(QTextOption::WrapAnywhere);
}

//******************************
//
//Removes actions and clears menuBar
//in order to properly clean
//the TextEdit
//
//******************************

void TextEdit::removeActions(){

    removeToolBar(tbEdit);
    removeToolBar(tbFile);
    removeToolBar(tbText);
    removeToolBar(tbColor);
    removeToolBar(tbFormat);
    removeToolBar(userInfoTb);
    menuBar()->clear();
}

//*********************************
//
//Cleans the TextEdit by disconnetting the necessary signals
//and by calling the necessary methods
//
//*********************************

void TextEdit::cleanTextEdit(){

    disconnect(textEdit, &QTextEdit::currentCharFormatChanged,
                this, &TextEdit::currentCharFormatChanged);
    disconnect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);
    disconnect(textEdit->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    disconnect(textEdit->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);
    disconnect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove);
    disconnect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    disconnect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);
    disconnect(textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &TextEdit::updateRemoteCursor);

    removeActions();
    delete algoritmoCRDT;
    delete usersTree;
    delete dockUsersTree;
    delete onlineUsers;
    delete offlineUsers;
    delete cursorMap;
    delete this->textEdit;
    textEdit = new QTextEdit(this);
    restoreQTextEdit();
}

//**************************************
//
//This slot makes the remote cursors to be
//fixed with the text and not the viewport
//
//**************************************

void TextEdit::updateRemoteCursor(){
    auto userIterator = onlineUsers->begin();
    while(userIterator!=onlineUsers->end()){
        QTextCursor *cursor = cursorMap->find(userIterator->getUserId()).value();
        QRect rect = textEdit->cursorRect(*cursor);
        labelMap->find(userIterator->getUserId()).value()->move(rect.left(),rect.top());
        labelMap->find(userIterator->getUserId()).value()->setFixedSize(2,rect.bottom()-rect.top());
        userIterator++;
    }
}
