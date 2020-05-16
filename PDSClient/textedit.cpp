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
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

#include "TextEdit.h"


#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

TextEdit::TextEdit(QWidget *parent, WorkerSocketClient* wscP,quint16 siteId)
    : QMainWindow(parent)
{
	//Inserisco inizializzazione del CRDT
    algoritmoCRDT = new CRDT(siteId);
    //inizialmente scrittura normale
    colorWriting = false;

    this->wscP = wscP;

    //Creazione dockWidget utenti online/offline

    usersTree = new QTreeWidget();
    dockUsersTree = new QDockWidget("Users");
    onlineUsers.append(*new QUser(15, "piero"));
    onlineUsers.append(*new QUser(12, "gianni"));
    offlineUsers.append(*new QUser(3, "laura"));

    updateTreeWidget(false);

    //connetto signal e slot che mi servono
    connect(this,&TextEdit::formatChanged,this, &TextEdit::format);




    setParent(parent);
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    cursorMap = new QMap<quint16, QTextCursor>();
    textEdit = new QTextEdit(this);

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
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    connect(textEdit->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);

    //connetto signal e slot che servono
    connect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove );

    /*chiusura documento*/
    QObject::connect(this, &TextEdit::SigChiudiDoc, wscP, &WorkerSocketClient::chiudiDoc);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoChiudiDoc, this,  &TextEdit::esitoChiudiDoc);

    /*operazione locale sul documento*/
    QObject::connect(this, &TextEdit::SigOpDocLocale, wscP, &WorkerSocketClient::opDocLocale);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoOpDocLocale, this,  &TextEdit::esitoOpDocLocale);

    /*operazione remota sul documento*/
    QObject::connect(wscP, &WorkerSocketClient::SigOpDocRemota, this,  &TextEdit::opDocRemota);

    /*un altro user ha aperto il doc*/
    QObject::connect(wscP, &WorkerSocketClient::SigQuestoUserHaApertoIlDoc, this,  &TextEdit::questoUserHaApertoIlDoc);

    /*un altro user ha chiuso il doc*/
    QObject::connect(wscP, &WorkerSocketClient::SigQuestoUserHaChiusoIlDoc, this,  &TextEdit::questoUserHaChiusoIlDoc);

    /*op chi ha inserito cosa*/
    QObject::connect(this, &TextEdit::SigOpChiHaInseritoCosa, wscP, &WorkerSocketClient::opChiHaInseritoCosa);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoOpChiHaInseritoCosa, this,  &TextEdit::esitoOpChiHaInseritoCosa);


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
}

//Le prossime due funzioni vengono usate nel RecentDocDialog per caricare il file
// (cioè la struttura CRDT) nell'editor. Viene fatto dentro il textEditor per non far uscire il CRDT e i cursori

CRDT* TextEdit::getStrutturaCRDT(){
    return algoritmoCRDT;
}

void TextEdit::loadCRDTIntoEditor(CRDT crdt){
  //this->algoritmoCRDT->setSiteID(crdt.getSiteID());
  //algoritmoCRDT = new CRDT(doc.getSiteID(),doc.getListChar()); //salvo nel CRDT la rappresentazione del file
  // devo andare ad aggiornare il contenuto del QTextEdit tramite l'uso di cursori sulla base di quello che c'� scritto nel CRDT
  int currentIndex = 0;
  *this->cursor = textEdit->textCursor();
  this->cursor->setPosition(currentIndex);
  auto lista = crdt.getListChar();
  for (auto richChar = lista.cbegin(); richChar!=lista.cend(); richChar++ ){
    QString str = "";
    Char ch = *richChar;
    str.append(ch.getValue());
    this->cursor->insertText(str,ch.getFormat());
    //Da controllare se il cursore si muove da solo dopo l'inserimento
    //currentIndex++
    //this->cursor->setPosition(currentIndex);
  }
}



void TextEdit::closeEvent(QCloseEvent *e)
{
   if(parentWidget() != NULL){
        emit(SigChiudiDoc(this->fileName));
        emit(updateRecDocs());
        parentWidget()->show();
        e->accept();
   }
   else
       e->ignore();
}
void TextEdit::setupFileActions()
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));
    QAction *a;

#ifndef QT_NO_PRINTER
    const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png"));
    a = menu->addAction(printIcon, tr("&Print..."), this, &TextEdit::filePrint);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Print);
    tb->addAction(a);

    const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png"));
    menu->addAction(filePrintIcon, tr("Print Preview..."), this, &TextEdit::filePrintPreview);

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    tb->addAction(a);

    menu->addSeparator();
#endif

    a = menu->addAction(tr("&Quit"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
}

void TextEdit::setupEditActions()
{
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menu->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menu->addAction(cutIcon, tr("Cu&t"), textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

void TextEdit::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Format"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
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

    tb->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    tb->addSeparator();

    QToolBar *tb2 = addToolBar(tr("Color Mode"));
    QIcon colorModeIcon = QIcon(rsrcPath + "/colormode.png");
    colorMode = new QAction(colorModeIcon, tr("&ColorMode"), this);
    colorMode->setCheckable(true);
    colorMode->setShortcut(Qt::CTRL + Qt::Key_F);
    colorMode->setPriority(QAction::LowPriority);
    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);
    tb2->addWidget(spacerWidget);
    tb2->addAction(colorMode);
    tb2->setMovable(false);

    //connetto il segnale allo slot che fa update del valore booleano
    connect(colorMode, &QAction::triggered, this, &TextEdit::pressedButtonTrigger);

    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);
    addToolBar(tb2);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &TextEdit::textStyle);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
}

//void TextEdit::load(const QString &f)
//{
//    emit SigApriDoc(f);
//}

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

//void TextEdit::fileNew@(QString fileName)
//{
//        disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
//        textEdit->clear();
//        setCurrentFileName(fileName);
//        emit SigCreaDoc(fileName);
//        connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
//}

void TextEdit::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
#endif
}

void TextEdit::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &TextEdit::printPreview);
    preview.exec();
#endif
}

void TextEdit::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif
}


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

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

void TextEdit::textFamily(const QString &f)
{QTextCharFormat fmt;
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    disconnect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove);
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

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
    } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

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
    connect(textEdit->document(),&QTextDocument::contentsChange,this, &TextEdit::CRDTInsertRemove );
}

void TextEdit::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

/*
Funzione per creare una DocOperation che segnala il movimento del cursore
Riceve come parametro il cursore ed emette un segnale con la DocOperation
Nella DocOperation i parametri che non vengono utilizzati vengono settati a null
*/

void TextEdit::segnalaMovimentoCursore(QTextCursor cursor){
    DocOperation* docOpCursore = new DocOperation(
                cursorMoved,Char(),QTextCharFormat(),this->algoritmoCRDT->getSiteID(),cursor.position(),cursor.anchor() );
  //  emit SigOpDocLocale(*docOpCursore);
}

void TextEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
    QTextList *list = textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
        case QTextListFormat::ListDisc:
            comboStyle->setCurrentIndex(1);
            break;
        case QTextListFormat::ListCircle:
            comboStyle->setCurrentIndex(2);
            break;
        case QTextListFormat::ListSquare:
            comboStyle->setCurrentIndex(3);
            break;
        case QTextListFormat::ListDecimal:
            comboStyle->setCurrentIndex(4);
            break;
        case QTextListFormat::ListLowerAlpha:
            comboStyle->setCurrentIndex(5);
            break;
        case QTextListFormat::ListUpperAlpha:
            comboStyle->setCurrentIndex(6);
            break;
        case QTextListFormat::ListLowerRoman:
            comboStyle->setCurrentIndex(7);
            break;
        case QTextListFormat::ListUpperRoman:
            comboStyle->setCurrentIndex(8);
            break;
        default:
            comboStyle->setCurrentIndex(-1);
            break;
        }
    } else {
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 8 : 0);

        //segnalo al WorkerSocketClient che c'è stato il movimento del cursore
        segnalaMovimentoCursore(textEdit->textCursor());
    }
}

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
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

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



void TextEdit::comunicaCRDTInserimentoLocale(QTextEdit* txe,QTextCursor* cursor, int pos, int numInserted,CRDT* algCRDT){
    auto inserted = txe->document()->toPlainText().mid(pos,numInserted);
    cursor->setPosition(pos,QTextCursor::MoveAnchor);
    for (int i=0;i<numInserted;i++){
        QTextCharFormat format = cursor->charFormat();
        if (colorWriting){
            format.setForeground(QBrush(QColor("black")));
        }
        QChar ch = inserted[i];
        std::cout<<"Comunico al CRDT inserimento del carattere: "<<ch.toLatin1()<<" all'indice locale: "<<pos+i<<"\n"<<std::flush;
        DocOperation docOp = algCRDT->localInsert(ch,format,pos+i);
        std::cout<<algCRDT->text.toStdString()<<"\n"<<std::flush;
        emit SigOpDocLocale(docOp);
    }
}

void TextEdit::comunicaCRDTRimozioneLocale(int pos, int numRemoved,CRDT* algCRDT){
    for(int i=0;i<numRemoved;i++){
        std::cout<<"Comunico al CRDT rimozione all'indice: "<<pos<<"\n"<<std::flush;
        DocOperation docOp = algCRDT->localErase(pos);
        std::cout<<algCRDT->text.toStdString()<<"\n"<<std::flush;
        emit SigOpDocLocale(docOp);
    }
}

void TextEdit::comunicaCRDTCambioFormat(QTextCursor* cursor, int pos, int numCar,CRDT* algCRDT){
    auto format = cursor->charFormat();
    std::cout<<"Comunico al CRDT la modifica del format di "<<numCar<<" a partire dall'indice "<<pos<<"\n"<<std::flush;
    if (colorWriting){
        format.setForeground(QBrush(QColor("black")));
    }
    for(int i=0;i<numCar;i++){
        DocOperation docOp = algCRDT->localFormatChange(format,pos+i);
        emit SigOpDocLocale(docOp);
    }
}

void TextEdit::CRDTInsertRemove(int pos, int rem, int add){
    std::cerr << "removed: "<<rem <<" added: "<<add<<" pos: "<<pos<< "\n";
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

            auto selection = cursor.selectedText();
            //auto position = cursor.position();
            //std::cout<<"Posizione del cursore: "<<position<<"\n"<<std::flush;
            //std::cout<<"Testo selezionato: "<< selection.toStdString().c_str()<<"\n"<<std::flush;
            if(!(selection.isEmpty())){
                //Si tratta di un cambio di formato, quindi ottengo il formato e lo comunico al CRDT
                //comunicaCRDTCambioFormat(&cursor,pos,add);
            }
            else{
                //Caso in cui si incolla su una selezione una stringa della stessa lunghezza
                //Non gestisco il caso particolare in cui la stringa incollata sia la stessa di quella cancellata
                //Pros: rimuovo controllo           Cons: faccio cancellazione e inserimento anche in quel caso
                comunicaCRDTRimozioneLocale(pos,rem,algoritmoCRDT);
                comunicaCRDTInserimentoLocale(textEdit,&cursor,pos,add,algoritmoCRDT);
            }
        }
        else{
            //Caso in cui si incolla una stringa sopra una selezione
            comunicaCRDTRimozioneLocale(pos,rem,algoritmoCRDT);
            comunicaCRDTInserimentoLocale(textEdit,&cursor,pos,add,algoritmoCRDT);
        }
    }
}

void TextEdit::format(const QTextCharFormat &format){
    std::cerr<<"Arrivato segnale di modifica del formato!\n"<<std::flush;
    QTextCursor cursor = textEdit->textCursor();
    int posCursor = cursor.position();
    int posAnchor = cursor.anchor();
    int changed = abs(cursor.anchor() - cursor.position());
    comunicaCRDTCambioFormat(&cursor,posCursor<posAnchor ? posCursor:posAnchor,changed,algoritmoCRDT);
}

int TextEdit::isSuccess(QString esito){
  if (esito.compare("Success") == 0)
    return 1;
  else
    return 0;
}




void TextEdit::esitoOpDocLocale(QString esito, DocOperation operation){

  qDebug()<<"esito_textedit: "<<esito<<"\n";

  if(!isSuccess(esito)){
    //in caso di esito negativo devo fare UNDO dell'operazione
    std::cout << "Operazione non andata a buon fine\n" << std::flush;
    switch(operation.type){
    case remoteInsert: //caso in cui avessi inserito un carattere -> lo rimuovo per fare UNDO
      algoritmoCRDT->remoteDelete(operation.character);
      break;
    case remoteDelete: //caso in cui avessi rimosso un carattere -> lo inserisco di nuovo per fare UNDO
      algoritmoCRDT->remoteInsert(operation.character);
      break;
    case changedFormat:// mi salvo il vecchio formato dal CRDT prima di cambiarlo
      Char* character = new Char(operation.character);
      character->setFormat(operation.oldFormat);
      algoritmoCRDT->remoteFormatChange(*character);
      break;
    }
  }

}

void TextEdit::opDocRemota(DocOperation operation){
   switch(operation.type){
    case remoteInsert:
       //Se è attiva la modalità di scrittura a colori devo fare un merge sul formato che mi arriva
       //inserendo anche il colore corretto rispetto al siteId del client che l'ha inserito
       if (colorWriting == true){
           auto colors = QColor::colorNames();
           QTextCharFormat coloredFormat(operation.character.getFormat());
           coloredFormat.setForeground(QBrush(QColor(colors[operation.character.getSiteId()])));
           operation.character.setFormat(coloredFormat);
       }
      algoritmoCRDT->remoteInsert(operation.character);
      break;
    case remoteDelete:
      algoritmoCRDT->remoteDelete(operation.character);
      break;
    case changedFormat:
       if (colorWriting == true){
           auto colors = QColor::colorNames();
           QTextCharFormat coloredFormat(operation.character.getFormat());
           coloredFormat.setForeground(QBrush(QColor(colors[operation.character.getSiteId()])));
           operation.character.setFormat(coloredFormat);
       }
      algoritmoCRDT->remoteFormatChange(operation.character);
      break;
   case cursorMoved:
       //Il QTextCursor si posiziona sempre in mezzo a due caratteri
       //Supponiamo quindi che per gestire la viasualizzazione del cursore, il carattere '|' si posiziona sempre prima del cursore
       // Esempio: testo di pro|(cursore)va

       //Rimuovo carattere | dalla vecchia posizione
       QTextCursor cursor = cursorMap->find(operation.siteId).value();
       cursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,1);
       cursor.removeSelectedText();

       //Inserisco | nella nuova posizione col colore del client
       auto colors = QColor::colorNames();
       QTextCharFormat* formatoCursoreColore = new QTextCharFormat(cursor.charFormat());
       formatoCursoreColore->setForeground(QBrush(QColor(colors[operation.siteId])));
       cursor.setPosition(operation.cursorPos, QTextCursor::MoveAnchor);
       cursor.insertText("|",*formatoCursoreColore);

       //Aggiorno mappa siteId - cursore
       cursorMap->find(operation.siteId).value() = cursor;
    }
}


void TextEdit::questoUserHaApertoIlDoc(QUser usr){
    onlineUsers.append(usr);
    updateTreeWidget(colorWriting);
}

void TextEdit::questoUserHaChiusoIlDoc(QUser usr){
    onlineUsers.removeAll(usr);
    updateTreeWidget(colorWriting);
}

void TextEdit::enteringColorMode(){
    if (colorWriting==false){
        //non dovrebbe mai entrare qui perchè deve essere attivata solo quando la variabile è true
        std::cerr << "Expected variable value:true, while it's false"<<std::flush;
        return;
    }
    auto colors = QColor::colorNames();
    QTextCursor* colorCursor = new QTextCursor(textEdit->textCursor());
    colorCursor->setPosition(QTextCursor::End);

    //Devo disconnettere e riconnettere il segnale per non far arrivare un segnale allo slot
    //che sto modificando il formato del testo
    QObject::disconnect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove );

    updateTreeWidget(colorWriting); //deve essere true per forza
    for (auto character:algoritmoCRDT->getListChar()){
        QTextCharFormat* format = new QTextCharFormat();
        format->setForeground( QBrush( QColor(colors[character.getSiteId()]) ) );
        colorCursor->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,1);
        colorCursor->mergeCharFormat(*format);
        //colorCursor->movePosition(colorCursor->NextCharacter);
    }

    //setto il colore del cursore corretto in base all'id del mio client
    //in modo che da ora in avanti sul client si scriva con quel colore
    auto cursorFormat = textEdit->textCursor().charFormat();
    cursorFormat.setForeground(QBrush(QColor(colors[this->algoritmoCRDT->getSiteID()])));
    textEdit->setCurrentCharFormat(cursorFormat);

    QObject::connect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove );
}

void TextEdit::quittingColorMode(){
    if (colorWriting==true){
        //non dovrebbe mai entrare qui perchè deve essere attivata solo quando la variabile è false
        std::cerr << "Expected variable value:false, while it's true"<<std::flush;
        return;
    }
    auto colors = QColor::colorNames();
    QTextCursor* colorCursor = new QTextCursor(textEdit->textCursor());
    QObject::disconnect(textEdit->document(),&QTextDocument::contentsChange,
            this, &TextEdit::CRDTInsertRemove );
    updateTreeWidget(colorWriting); //deve essere false per forza
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

    QObject::connect(textEdit->document(),&QTextDocument::contentsChange,
        this, &TextEdit::CRDTInsertRemove );
}


void TextEdit::esitoOpChiHaInseritoCosa(QList<QUser> users){
    for (auto user : users){
        if (!onlineUsers.contains(user) && user.getUserId()!=this->algoritmoCRDT->getSiteID())
            offlineUsers.append(user);
    }
    enteringColorMode();
    updateTreeWidget(colorWriting);
}



//slot per gestire segnale di click sul color Mode
void TextEdit::pressedButtonTrigger(bool checked){
    colorWriting = checked;
    if (checked==true){ // solamente se entriamo nella color Mode
    //emetto segnale per inviare la richiesta al server di ricevere la lista con storico utenti
        //emit SigOpChiHaInseritoCosa();
        enteringColorMode();
    }
    else{
        quittingColorMode();
    }
}


void TextEdit::esitoChiudiDoc(QString esito){
  //Per ora stampo solo l'esito ricevuto dal server
  //Per evitare la chiusura del file nel caso in cui si ricevesse un esito negativo devo mantenere l'informazione riguardante
  //il QCloseEvent scatenante il messaggio di chiusura
  std::cout << esito.toStdString()<< "\n" << std::flush;
  if (isSuccess(esito)){
     textEdit->close();
  }
  else
      //Qui dovrebbe apparire una finestra in cui si indica l'errore, per far sapere all'utente che qualcosa è andato storto
      std::cout << "Non ho chiuso il documento perchè il server ha risposto esito negativo\n"<<std::flush;
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
        usersTree->setColumnCount(1);

        QList<QString> *usernameOnline = new QList<QString>();
        QList<QColor> *colorList = new QList<QColor>();
        QList<QTreeWidgetItem*> *itemUsernameOnline = new QList<QTreeWidgetItem*>();
        QList<QUser>::iterator i;
        for(i=onlineUsers.begin(); i != onlineUsers.end(); ++i){
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

        if(!offlineUsers.isEmpty()){
            QList<QColor> *offlineColorList = new QList<QColor>();
            QTreeWidgetItem *item2 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Offline")));
            usersTree->insertTopLevelItem(0, item2);
            QList<QString> *usernameOffline = new QList<QString>();
            QList<QTreeWidgetItem*> *itemUsernameOffline = new QList<QTreeWidgetItem*>();
            QList<QUser>::iterator i;

            for(i=offlineUsers.begin(); i != offlineUsers.end(); ++i){
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


        usersTree->setColumnCount(1);

        QList<QString> *usernameOnline = new QList<QString>();
        QList<QTreeWidgetItem*> *itemUsernameOnline = new QList<QTreeWidgetItem*>();
        QList<QUser>::iterator i;
        for(i=onlineUsers.begin(); i != onlineUsers.end(); ++i){
            usernameOnline->append(i->getUserName());
        }

        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Online")));
        usersTree->insertTopLevelItem(0, item);
        QList<QString>::iterator j;
        for(j=usernameOnline->begin(); j!=usernameOnline->end(); ++j){
            QTreeWidgetItem *utenteOnline = new QTreeWidgetItem((QTreeWidget*)0, QStringList(*j));
            //utenteOnline->setIcon(0,QIcon(rsrcPath + "/online.png"));
            itemUsernameOnline->append(utenteOnline);
        }

        item->insertChildren(0,*itemUsernameOnline);
    }


    dockUsersTree->setWidget(usersTree);

}
