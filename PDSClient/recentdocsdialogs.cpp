#include "recentdocsdialogs.h"


recentDocsDialogs::recentDocsDialogs(QWidget *parent, WorkerSocketClient* wscP):QWidget(parent)
{

//*********************************************************************
//
//Creazione del dialog dei documenti recenti. C'è anche la possibilità
//di aprire un documento nuovo, inserendo il nome del documento, o
//tramite un URL, aprire un documento tramite invito. Si eseguono
//le connessioni tra slot e segnali.
//Si accede al file "docs.recent", se già esistente (creato in fase di
//login usando le informazioni ricevute dal server, per inizializzare
//la lista dei documenti editati dall'utente che ha effettuato il login.
//Nel caso suddetto file non fosse presente, la lista è vuota.
//
//*********************************************************************

    this-> setWindowTitle("Recent Docs");
    setParent(parent);
    setAttribute(Qt::WA_DeleteOnClose);
    this->wscP = wscP;
    mw = new TextEdit(this, wscP);
    recentDocs = new QListWidget();

    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/").exists()){
            QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/");
        }

    if(QFileInfo::exists(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent")){
         QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
         recent.open(QIODevice::ReadOnly);
         QString fileName = recent.readLine();
         while(!fileName.isNull() || !fileName.isEmpty()){
             new QListWidgetItem(fileName, recentDocs);
             fileName = recent.readLine();
         }
         recent.close();
    }else{
        recentDocs->clear();
    }

    recentDocs->setSelectionMode(QAbstractItemView::SingleSelection);

    create = new QPushButton(tr("&New File"));
    create->setDisabled(true);
    editProfile = new QPushButton(tr("&Edit User Profile"));
    open = new QPushButton(tr("&Open"));
    open->setDisabled(true);
    abort = new QPushButton(tr("&Close"));
    openUrl = new QPushButton(tr("&Open URL"));
    openUrl->setDisabled(true);
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

    connect(URL, SIGNAL(textEdited(const QString &)), this, SLOT(urlChanged(const QString &)));
    connect(newFileName, SIGNAL(textEdited(const QString &)), this, SLOT(newFileChanged(const QString &)));
    connect(create, SIGNAL(clicked()), this, SLOT(newFilePressed()));
    connect(open, SIGNAL(clicked()), this, SLOT(openPressed()));
    connect(openUrl, SIGNAL(clicked()), this, SLOT(openUrlPressed()));
    connect(abort, SIGNAL(clicked()), this, SLOT(abortPressed()));
    connect(recentDocs, SIGNAL(itemSelectionChanged()), this, SLOT(listItemSelected()));
    connect(editProfile, SIGNAL(clicked()), this, SLOT(launchEditProfile()));

    /*creazione documento*/
    QObject::connect(this, &recentDocsDialogs::SigCreaDoc, wscP, &WorkerSocketClient::creaDoc);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoCreaDoc, this,  &recentDocsDialogs::esitoCreaDoc);

    /*apertura documento*/
    QObject::connect(this, &recentDocsDialogs::SigApriDoc, wscP, &WorkerSocketClient::apriDoc);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoApriDoc, this,  &recentDocsDialogs::esitoApriDoc);


}

//*********************************************************************
//
//Emissione del segnale SigApriDoc per iniziare l'apertura del doc
//selezionato
//
//*********************************************************************

void recentDocsDialogs::openPressed(){
    QString selectedDoc = recentDocs->selectedItems().first()->text();
    emit(SigApriDoc(selectedDoc));
}

//*********************************************************************
//
//Chiusura dell'applicazione
//
//*********************************************************************

void recentDocsDialogs::abortPressed(){
    this->close();
}


//*********************************************************************
//
//Emissione del segnale SigApriDoc per avviare l'apertura del file
//da URL
//
//*********************************************************************
void recentDocsDialogs::openUrlPressed(){
    QString urlDOC = URL->text();
    emit(SigApriDoc(urlDOC));
}

//*********************************************************************
//
//Emissione del segnale SigCreaDoc per avviare la creazione e l'apertura
//di un nuovo file
//
//*********************************************************************
void recentDocsDialogs::newFilePressed(){
    QString docName = newFileName->text();
    emit(SigCreaDoc(docName));
}

//*********************************************************************
//
//Abilitazione o disabilitazione del pulsante "openUrl" nel caso
//il campo "url" sia pieno o vuoto
//
//*********************************************************************
void recentDocsDialogs::urlChanged(const QString &url){
    if(url.isEmpty() || url.isNull()){
        openUrl->setDisabled(true);
    }
    else{
        openUrl->setDisabled(false);
    }
}


//*********************************************************************
//
//Abilitazione o disabilitazione del pulsante "create" nel caso
//il campo "newFileName" sia pieno o vuoto
//
//*********************************************************************
void recentDocsDialogs::newFileChanged(const QString &newFileName){
    if(newFileName.isEmpty() || newFileName.isNull()){
        create->setDisabled(true);
    }else{
        create->setDisabled(false);
    }
}

//*********************************************************************
//
//Abilitazione del tasto "open" non appena un documento viene
//selezionato dalla lista
//
//*********************************************************************
void recentDocsDialogs::listItemSelected(){
    open->setDisabled(false);

}
//*********************************************************************
//
//Lancia l'editor del profilo
//
//*********************************************************************

void recentDocsDialogs::launchEditProfile(){
    edit = new class editProfile(this, this->wscP);
    edit->setWindowFlag(Qt::Window);
    edit->show();
}
//*********************************************************************
//
//Aggiorna il contenuto del widget
//
//*********************************************************************
void recentDocsDialogs::updateRecDocs(){

    recentDocs->clear();
    URL->clear();
    newFileName->clear();
    create->setDisabled(true);
    openUrl->setDisabled(true);

    if(QFileInfo::exists(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent")){
         QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
         recent.open(QIODevice::ReadOnly);
         QString fileName = recent.readLine();
         while(!fileName.isNull() || !fileName.isEmpty()){
             new QListWidgetItem(fileName, recentDocs);
             fileName = recent.readLine();
         }
         recent.close();
    }else{
        recentDocs->clear();
    }


}

int isSuccess(QString esito){
  if (esito.compare("Success") == 0)
    return 1;
  else
    return 0;
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

void recentDocsDialogs::esitoCreaDoc(QString esito, CRDT doc){
  if (isSuccess(esito)){ //se esito positivo creo un CRDT vuoto perché il documento é stato appena creato
    mw->getStrutturaCRDT()->setSiteID(doc.getSiteID()); //Prendo solamente il siteId corretto da mettere nel CRDT
    QString docName = newFileName->text();
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
    mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    mw->move((availableGeometry.width() - mw->width()) / 2,
            (availableGeometry.height() - mw->height()) / 2);
    mw->setCurrentFileName(docName);
    if(QFileInfo::exists(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent")){
         QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
         recent.open(QIODevice::WriteOnly | QIODevice::Append);
         QTextStream out (&recent);
         out << docName << endl;
         recent.close();
     }else{
        QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
        recent.open(QIODevice::WriteOnly);
        QTextStream out (&recent);
        out << docName << endl;
        recent.close();
    }
    mw->show();
    this->hide();
  }
  else
  {
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
void recentDocsDialogs::esitoApriDoc(QString esito, CRDT doc){
  if (isSuccess(esito)){
      mw->loadCRDTIntoEditor(doc);
      QString docName = recentDocs->selectedItems().first()->text();
      const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
      mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
      mw->move((availableGeometry.width() - mw->width()) / 2,
              (availableGeometry.height() - mw->height()) / 2);
      mw->setCurrentFileName(docName);
      if(QFileInfo::exists(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent")){
           QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
           recent.open(QIODevice::WriteOnly | QIODevice::Append);
           QTextStream out (&recent);
           out << docName << endl;
           recent.close();
       }else{
          QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
          recent.open(QIODevice::WriteOnly);
          QTextStream out (&recent);
          out << docName << endl;
          recent.close();
      }
      mw->show();
      this->hide();

    }
  else
  {
      QMessageBox msgBox;
      msgBox.setText(tr("A problem occured while opening the selected doc. Please, try again"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();

  }
}

