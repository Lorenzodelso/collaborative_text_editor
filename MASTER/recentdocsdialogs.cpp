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
    mw = new TextEdit(this,wscP);
    wscPointer = wscP;
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
//Creazione dell'oggetto "textedit", che caricherà il documento selezionato
//precedentemente il cui nome è salvato in "selectedDoc"
//
//*********************************************************************

void recentDocsDialogs::openPressed(){
    QString selectedDoc = recentDocs->selectedItems().first()->text();
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
    mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    mw->move((availableGeometry.width() - mw->width()) / 2,
            (availableGeometry.height() - mw->height()) / 2);
    mw->load(selectedDoc);
    mw->setEnabled(true);
    mw->show();
    this->hide();
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
//Creazione dell'oggetto "textedit", che caricherà il file tramite
//l'uso di un URL. Il file aperto verrà aggiunto alla lista dei
//documenti recenti dell'utente. Nel caso il file contenente la lista
//dei documenti recenti non fosse già presente, verrà creato.
//
//*********************************************************************
void recentDocsDialogs::openUrlPressed(){
    QString urlDOC = URL->text();
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
    mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    mw->move((availableGeometry.width() - mw->width()) / 2,
            (availableGeometry.height() - mw->height()) / 2);
    mw->load(urlDOC);
    mw->setEnabled(true);
    mw->show();
    if(QFileInfo::exists(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent")){
         QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
         recent.open(QIODevice::WriteOnly | QIODevice::Append);
         QTextStream out (&recent);
         out << URL;
         recent.close();
    }else{
         QFile recent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
         recent.open(QIODevice::WriteOnly);
         QTextStream out (&recent);
         out << URL;
         recent.close();
    }
    this->hide();
}

//*********************************************************************
//
//Viene creato l'oggetto "textedit", che caricherà un documento nuovo
//a cui verrà assegnato il nome contenuto nella variabile "docName".
//Il documento verrà inserito nel file contenente la lista dei
//documenti recenti. Nel caso questo file non fosse già presente,
//verrà creato
//
//*********************************************************************
void recentDocsDialogs::newFilePressed(){
    QString docName = newFileName->text();
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(mw);
    mw->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    mw->move((availableGeometry.width() - mw->width()) / 2,
            (availableGeometry.height() - mw->height()) / 2);
    mw->fileNew(docName);

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
    edit = new class editProfile(this,wscPointer);
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


void recentDocsDialogs::esitoCreaDoc(QString esito, CRDT doc){
  if (isSuccess(esito)){ //se esito positivo creo un CRDT vuoto perch� il documento � stato appena creato
    mw->getStrutturaCRDT()->setSiteID(doc.getSiteID()); //Prendo solamente il siteId corretto da mettere nel CRDT
  }
  else
    //per ora solo messaggio di errore sull'output
    std::cout << "Errore nella crazione di un nuovo documento\n" << std::flush;
}

void recentDocsDialogs::esitoApriDoc(QString esito, CRDT doc){
  if (isSuccess(esito)){
      mw->loadCRDTIntoEditor(doc);
    }
  else
    std::cout << "Errore nell'apertura di un file esistente\n" <<std::flush;
}

