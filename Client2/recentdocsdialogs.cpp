#include "RecentDocsDialogs.h"


//*********************************************************************
//
//Creazione del dialog dei documenti recenti. C'è anche la possibilità
//di aprire un documento nuovo, inserendo il nome del documento, o
//tramite un URL, aprire un documento tramite invito. Si eseguono
//le connessioni tra slot e segnali.
//Si inizializza la lista dei documenti recenti coi dati ricevuti dal
//server in fase di login
//
//*********************************************************************

RecentDocsDialogs::RecentDocsDialogs(QWidget *parent, WorkerSocketClient* wscP,quint32 siteId, QUtente utente, QList<QString> docList):QWidget(parent){

    this-> setWindowTitle("Recent Docs");
    setParent(parent);
    setObjectName("recDocs");
    setAttribute(Qt::WA_DeleteOnClose);
    this->wscP = wscP;
    this->siteId = siteId;
    recentDocs = new QListWidget();
    this->utente = utente;
    this->docList = docList;
    QList<QString>::iterator i;
    mw = new TextEdit(this,this->wscP,this->siteId, this->utente);

    /*un altro user ha aperto il doc*/
    QObject::connect(wscP, &WorkerSocketClient::SigQuestoUserHaApertoIlDoc, mw,  &TextEdit::questoUserHaApertoIlDoc);

    /*un altro user ha chiuso il doc*/
    QObject::connect(wscP, &WorkerSocketClient::SigQuestoUserHaChiusoIlDoc, mw,  &TextEdit::questoUserHaChiusoIlDoc);

    if(!this->docList.isEmpty())
         for(i=this->docList.begin(); i!=this->docList.end(); i++)
             new QListWidgetItem(*i, recentDocs);
    else
        recentDocs->clear();

    recentDocs->setSelectionMode(QAbstractItemView::SingleSelection);
    create = new QPushButton(tr("&New File"));
    create->setDisabled(true);
    create->setFixedSize(90,25);
    editProfile = new QPushButton(tr("&Edit User Profile"));
    editProfile->setFixedSize(120, 25);
    open = new QPushButton(tr("&Open"));
    open->setFixedSize(65,25);
    open->setDisabled(true);
    abort = new QPushButton(tr("&Close"));
    abort->setFixedSize(70,25);
    openUrl = new QPushButton(tr("&Open URL"));
    openUrl->setDisabled(true);
    openUrl->setFixedSize(90,25);
    newFileName = new QLineEdit();
    newFileName->setEnabled(true);
    newFileName->setPlaceholderText("New File Name");
    newFileName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    newFileName->setMinimumWidth(200);

    URL = new QLineEdit();
    URL->setEnabled(true);
    URL->setPlaceholderText("Shared URL");
    URL->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    URL->setMinimumWidth(200);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(open, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(abort, QDialogButtonBox::RejectRole);
    buttonBox->addButton(editProfile, QDialogButtonBox::ActionRole);

    QHBoxLayout *URLLayout = new QHBoxLayout();
    URLLayout->addWidget(URL);
    URLLayout->addWidget(openUrl);
    URLLayout->setSpacing(10);
    URLLayout->setAlignment(URL, Qt::AlignLeft);
    URLLayout->setAlignment(openUrl, Qt::AlignRight);
    URLLayout->setStretchFactor(URL,1);


    QHBoxLayout *newLayout = new QHBoxLayout();
    newLayout->addWidget(newFileName);
    newLayout->addWidget(create);
    newLayout->setSpacing(10);
    newLayout->setAlignment(newFileName, Qt::AlignLeft);
    newLayout->setAlignment(create, Qt::AlignRight);
    newLayout->setStretchFactor(newFileName,1);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(recentDocs);
    dialogLayout->setAlignment(recentDocs, Qt::AlignLeft);
    dialogLayout->addSpacing(5);
    dialogLayout->addItem(URLLayout);
    dialogLayout->setAlignment(URLLayout, Qt::AlignCenter);
    dialogLayout->addSpacing(5);
    dialogLayout->addItem(newLayout);
    dialogLayout->setAlignment(newLayout, Qt::AlignCenter);
    dialogLayout->addSpacing(5);
    dialogLayout->addWidget(buttonBox);
    dialogLayout->setAlignment(buttonBox, Qt::AlignCenter);

    setLayout(dialogLayout);
    setMinimumSize(250,500);

    connect(URL, &QLineEdit::textEdited, this, &RecentDocsDialogs::urlChanged);
    connect(newFileName, &QLineEdit::textEdited, this, &RecentDocsDialogs::newFileChanged);
    connect(create, &QPushButton::clicked, this, &RecentDocsDialogs::newFilePressed);
    connect(open, &QPushButton::clicked, this, &RecentDocsDialogs::openPressed);
    connect(openUrl, &QPushButton::clicked, this, &RecentDocsDialogs::openUrlPressed);
    connect(abort, &QPushButton::clicked, this, &RecentDocsDialogs::abortPressed);
    connect(recentDocs, &QListWidget::itemSelectionChanged, this, &RecentDocsDialogs::listItemSelected);
    connect(editProfile, &QPushButton::clicked, this, &RecentDocsDialogs::launchEditProfile);

    /*creazione documento*/
    QObject::connect(this, &RecentDocsDialogs::SigCreaDoc, wscP, &WorkerSocketClient::creaDoc);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoCreaDoc, this,  &RecentDocsDialogs::esitoCreaDoc);

    /*apertura documento*/
    QObject::connect(this, &RecentDocsDialogs::SigApriDoc, wscP, &WorkerSocketClient::apriDoc);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoApriDoc, this,  &RecentDocsDialogs::esitoApriDoc);

    /*Chiusura documento*/
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoChiudiDoc, this,  &RecentDocsDialogs::esitoChiudiDoc);


}

//*********************************************************************
//
//Emissione del segnale SigApriDoc per iniziare l'apertura del doc
//selezionato
//
//*********************************************************************

void RecentDocsDialogs::openPressed(){
    fileName = recentDocs->selectedItems().first()->text();
    if(!fileName.isNull() || !fileName.isEmpty())
        emit(SigApriDoc(fileName.remove('\n')));
}

//*********************************************************************
//
//Chiusura dell'applicazione
//
//*********************************************************************

void RecentDocsDialogs::abortPressed(){
    this->close();
}


//*********************************************************************
//
//Emissione del segnale SigApriDoc per avviare l'apertura del file
//da URL
//
//*********************************************************************

void RecentDocsDialogs::openUrlPressed(){
    fileName = URL->text();
    emit(SigApriDoc(fileName));
}

//*********************************************************************
//
//Emissione del segnale SigCreaDoc per avviare la creazione e l'apertura
//di un nuovo file
//
//*********************************************************************

void RecentDocsDialogs::newFilePressed(){
    fileName = newFileName->text();
    emit(SigCreaDoc(fileName));
}

//*********************************************************************
//
//Abilitazione o disabilitazione del pulsante "openUrl" nel caso
//il campo "url" sia pieno o vuoto
//
//*********************************************************************

void RecentDocsDialogs::urlChanged(const QString &url){
    if(url.isEmpty() || url.isNull())
        openUrl->setDisabled(true);
    else
        openUrl->setDisabled(false);
}


//*********************************************************************
//
//Abilitazione o disabilitazione del pulsante "create" nel caso
//il campo "newFileName" sia pieno o vuoto
//
//*********************************************************************

void RecentDocsDialogs::newFileChanged(const QString &newFileName){
    if(newFileName.isEmpty() || newFileName.isNull())
        create->setDisabled(true);
    else
        create->setDisabled(false);
}

//*********************************************************************
//
//Abilitazione del tasto "open" non appena un documento viene
//selezionato dalla lista
//
//*********************************************************************

void RecentDocsDialogs::listItemSelected(){
    open->setDisabled(false);

}
//*********************************************************************
//
//Lancia l'editor del profilo
//
//*********************************************************************

void RecentDocsDialogs::launchEditProfile(){
    edit = new class EditProfile(this, this->wscP, &this->utente);
    edit->setWindowFlag(Qt::Window);
    edit->show();
}
//*********************************************************************
//
//Aggiorna il contenuto del widget
//
//*********************************************************************

void RecentDocsDialogs::updateRecDocs(){

    recentDocs->clear();
    URL->clear();
    newFileName->clear();
    create->setDisabled(true);
    openUrl->setDisabled(true);
    QList<QString>::iterator i;
    if(!this->docList.isEmpty())
         for(i=this->docList.begin(); i!=this->docList.end(); i++)
             new QListWidgetItem(*i, recentDocs);
    else
        recentDocs->clear();


}

//*************************************************
//
//Nel caso l'esito della creazione del nuovo doc
//sia positivo, il textEditor viene popolato e mostrato.
//La lista dei file recenti viene aggiornata
//con il nuovo file. In caso di errore
//viene visualizzato un msgBox che avvisa l'utente
//del problema.
//
//*************************************************

void RecentDocsDialogs::esitoCreaDoc(QString esito, CRDT doc){

    if (esito.compare("Success") == 0){
        mw->updateUserInfo(this->utente);
        mw->loadCRDTIntoEditor(doc);
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
        mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
        mw->move((availableGeometry.width() - mw->width()) / 2, (availableGeometry.height() - mw->height()) / 2);
        mw->setCurrentFileName(fileName);
        this->docList.append(fileName);
        mw->show();
        this->hide();
  }
  else{
      QMessageBox msgBox;
      msgBox.setText(tr("A problem occured while creating a new doc. Please, try again"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
  }
}

//******************************************
//
//Nel caso l'esito dell'apertura di un doc
//esistente sia positivo, il textEditor viene
//popolato e mostrato. Viene aggiornata la lista
//dei documenti recenti. In caso di errore
//viene visualizzato un msgBox che avvisa l'utente
//del problema.
//
//******************************************

void RecentDocsDialogs::esitoApriDoc(QString esito, CRDT doc){
    if (esito.compare("Success") == 0){
      mw->updateUserInfo(this->utente);
      mw->loadCRDTIntoEditor(doc);
      const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
      mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
      mw->move((availableGeometry.width() - mw->width()) / 2,
              (availableGeometry.height() - mw->height()) / 2);
      mw->setCurrentFileName(fileName);
      if(!docList.contains(fileName))
          docList.append(fileName);
      mw->show();
      this->hide();
   }else{
      QMessageBox msgBox;
      msgBox.setText(tr("A problem occured while opening the selected doc. Please, try again"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
  }
}

//******************************************************************
//
//If esito is "Success", the text editor is closed and cleaned.
//If esito is "Failed", an error message box will show up.
//
//******************************************************************
void RecentDocsDialogs::esitoChiudiDoc(QString esito){
  if (esito.compare("Success") == 0){
      mw->hide();
      mw->cleanTextEdit();
      this->show();
      open->setEnabled(false);
  }
  else{
      QMessageBox msgBox;
      msgBox.setText(tr("A problem occured while closing the document. Please, try again"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
  }
}


