#include "EditProfile.h"

//*********************************************************************
//
//Creazione del dialog di modifica del profilo utente.
//Il dialog viene popolato con le informazioni ricevute al momento
//del login con i dati ricevuti dal server.
//Con gli stessi dati, viene creato un
//oggetto "QUser" che verrà successivamente utilizzato durante
//l'atto di modifica del profilo
//
//*********************************************************************
EditProfile::EditProfile(QWidget *parent, WorkerSocketClient* wscP, QUtente* utente)
{
    setParent(parent);
    setWindowTitle("User Profile editor");
    recDocsUtente = utente;
    username = new QLabel(tr("&Username: "));
    utenteLocale = new QUtente(*utente);
    this->wscP = wscP;
    usernameEdit = new QLineEdit();
    //usernameEdit->setEnabled(false);
    nickname = new QLabel(tr("&Nickname: "));
    nickEdit = new QLineEdit();
    userPic = new ClickableLabel();
    userPic->setBackgroundRole(QPalette::Dark);
    userPic->setScaledContents(false);
    profilePic = new QPixmap();

    userPic->setPixmap(*profilePic);
    username->setBuddy(usernameEdit);
    nickname->setBuddy(nickEdit);
    setGeometry(575,300,400,200);


        usernameEdit->setText(utente->getUsername());
        usernameEdit->setEnabled(false);

        if(!utente->getNickName().isEmpty()){
            nickEdit->setText(utente->getNickName());
        }else{
            nickEdit->setPlaceholderText(tr("(Optional)"));
        }
        qDebug()<< QDir::currentPath()+"/"+utente->getNomeImg();
        if(!QFileInfo::exists(QDir::currentPath()+"/"+utente->getNomeImg()) || !QFileInfo(QDir::currentPath()+"/"+utente->getNomeImg()).isFile()){
                profilePic->load(rsrc+"/colored-edit-profile.png");
                userPic->setPixmap(*profilePic);
        }else{
            profilePic->load(QDir::currentPath()+"/"+utente->getNomeImg());
            QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
            userPic->setPixmap(scaled);
        }


    save = new QPushButton(tr("&Save"));
    discard= new QPushButton(tr("&Discard"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(save, QDialogButtonBox::ApplyRole);
    buttonBox->addButton(discard, QDialogButtonBox::RejectRole);
    //setModal(true);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(userPic);
    layout->setAlignment(userPic,Qt::AlignCenter);
    layout->setSpacing(15);
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(username, usernameEdit);
    formLayout->addRow(nickname, nickEdit);
    layout->addLayout(formLayout);
    layout->addWidget(buttonBox);
    layout->setAlignment(buttonBox, Qt::AlignCenter);
    setLayout(layout);
    setMinimumSize(200, 300);

    connect(discard, &QPushButton::clicked, this, &EditProfile::discardPressed);
    connect(save, &QPushButton::clicked, this, &EditProfile::savePressed);
    connect(userPic, &ClickableLabel::clicked, this, &EditProfile::selectImagePressed);
    connect(nickEdit, &QLineEdit::textEdited, this, &EditProfile::changedNick);
    connect(usernameEdit, &QLineEdit::textEdited, this, &EditProfile::changedUsername);
    connect(userPic, &ClickableLabel::hovered, this, &EditProfile::imageHovered);
    connect(userPic, &ClickableLabel::unHovered, this, &EditProfile::imageUnhovered);

    /*modifica profilo utente*/
    QObject::connect(this, &EditProfile::SigModificaProfiloUtente, wscP, &WorkerSocketClient::modificaProfiloUtente);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoModificaProfiloUtente,this,  &EditProfile::esitoModificaProfiloUtente);


//    //**********************************************************
//    QUtente *utenteTest = new QUtente(1234, "mario", "grdnmario", "password", "");
//    esitoModificaProfiloUtente("Success", *utenteTest);
//    //**********************************************************


}


//*********************************************************************
//
//Questo slot modifica il campo nickName dell'oggetto "QUtente"
//ogni qualvolta il lineEdit corrispondete viene modificato
//
//*********************************************************************
void EditProfile::changedNick(const QString &){

   QString nick = nickEdit ->text();
   nickEdit->setStyleSheet("QLineEdit {color: #000000;}");
   if(!nick.isEmpty()){
       utenteLocale->setNickName(nick);
    }
}

void EditProfile::changedUsername(const QString &){

   QString username = usernameEdit ->text();
   usernameEdit->setStyleSheet("QLineEdit {color: #000000;}");
   if(!username.isEmpty()){
       utenteLocale->setUsername(username);
    }
}

//*********************************************************************
//
//L'oggetto "QUtente" popolato con le eventuali modifiche, viene inviato
//
//*********************************************************************
void EditProfile::savePressed(){

    emit(SigModificaProfiloUtente(*utenteLocale));
}

//*********************************************************************
//
//In caso di cancellazione, nessuna modifica verrà inviata al server
//
//*********************************************************************
void EditProfile::discardPressed(){
    close();
}


//*********************************************************************
//
//Selezione dell'immagine del profilo utente. Il path verrà salvato
//nell'oggetto "QUtente" e l'immagine verrà utilizzata al posto di
//quella di default
//*********************************************************************
void EditProfile::selectImagePressed(){
    QUrl imageUrl = QFileDialog::getOpenFileUrl(this, tr("Open Image"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), tr("Image Files (*.png *.jpg *.bmp)"));
    QString imagePath = imageUrl.path();
    #ifndef Q_OS_MAC
    imagePath.remove(0,1);
    #endif
    QString imageName = imageUrl.fileName();
    if(!imagePath.isEmpty() || !imagePath.isNull()){
        utenteLocale->setNomeImg(imagePath);
        profilePic->load(imagePath);
        QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
        userPic->setPixmap(scaled);
    }
}

//*********************************************************************
//
//Slot che fa visualizzare l'immagine standard di "aggiunta immagine
//profilo" all'utente ogni volta che l'immagine corrente viene
//toccata dal puntatore del mouse
//
//*********************************************************************

void EditProfile::imageHovered(){
    profilePic->load(rsrc+"/add-profile-image.png");
    userPic->setPixmap(*profilePic);
}

//*********************************************************************
//
//Slot che fa visualizzare l'immagine utente selezionata dall'utente
//stesso, qualora ne abbia scelta una, o l'immagine stock dell'utente,
//qualora non sia stata fatta una scelta, non appena il puntatore del
//mouse non tocca più l'immagine
//
//*********************************************************************
void EditProfile::imageUnhovered(){

    if(!QFileInfo::exists(utenteLocale->getNomeImg())){
            profilePic->load(rsrc+"/colored-edit-profile.png");
            userPic->setPixmap(*profilePic);
    }else{
            profilePic->load(utenteLocale->getNomeImg());
            QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
            userPic->setPixmap(scaled);
    }
}

/*
 * fa la cosa opportuna sulla base dell'esito ricevuto
 *
 * nota:
 * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
 * il costruttore senza paramteri
 * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
 *
 * */
void EditProfile::esitoModificaProfiloUtente(QString esito/*esito*/, QUtente userNew){
    if(esito == "Failed"){
        QMessageBox msgBox;
        msgBox.setText("Something went wrong. Please, try again");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }else{
        recDocsUtente->setUserId(userNew.getUserId());
        recDocsUtente->setUsername(userNew.getUsername());
        recDocsUtente->setNickName(userNew.getNickName());
        recDocsUtente->setNomeImg(userNew.getNomeImg());
        close();
    }


}
