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
    setWindowFlag(Qt::Window);
    setObjectName("editProfile");
    recDocsUtente = utente;
    username = new QLabel(tr("&Username: "));
    utenteLocale = new QUtente(*utente);
    this->wscP = wscP;
    usernameEdit = new QLineEdit();
    newPass = new QLabel(tr("New Password"));
    newPassEdit = new QLineEdit();
    newPassEdit->setEchoMode(QLineEdit::Password);
    newRepPass = new QLabel(tr("Repeat New Password"));
    newRepPassEdit = new QLineEdit();
    newRepPassEdit->setEchoMode(QLineEdit::Password);
    userPic = new ClickableLabel();
    userPic->setBackgroundRole(QPalette::Dark);
    userPic->setScaledContents(false);
    profilePic = new QPixmap();
    userErr = new QLabel("");
    passErr = new QLabel("");
    userPic->setPixmap(*profilePic);
    username->setBuddy(usernameEdit);
    newPass->setBuddy(newPassEdit);
    newRepPass->setBuddy(newRepPassEdit);
    usernameEdit->setText(utente->getUsername());
    usernameEdit->setEnabled(true);
    if(!QFileInfo::exists(QDir::currentPath()+"/"+utente->getNomeImg()) || !QFileInfo(QDir::currentPath()+"/"+utente->getNomeImg()).isFile()){
       profilePic->load(rsrc+"/colored-edit-profile.png");
       userPic->setPixmap(*profilePic);
    }else{
       profilePic->load(QDir::currentPath()+"/"+utente->getNomeImg());
       QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
       userPic->setPixmap(scaled);
    }

    save = new QPushButton(tr("&Save"));
    save->setFixedSize(60,25);
    discard= new QPushButton(tr("&Discard"));
    discard->setFixedSize(70,25);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(save, QDialogButtonBox::ApplyRole);
    buttonBox->addButton(discard, QDialogButtonBox::RejectRole);
    QVBoxLayout *layout = new QVBoxLayout();
    userPic->setFixedHeight(200);
    layout->addWidget(userPic);
    layout->setAlignment(userPic, Qt::AlignCenter);
    layout->addSpacing(15);
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(username, usernameEdit);
    formLayout->addRow(userErr);
    formLayout->setSpacing(5);
    formLayout->setSpacing(5);
    formLayout->addRow(newPass, newPassEdit);
    formLayout->setSpacing(5);
    formLayout->addRow(newRepPass, newRepPassEdit);
    formLayout->addRow(passErr);
    layout->addItem(formLayout);
    layout->addSpacing(5);
    layout->addWidget(buttonBox);
    layout->setAlignment(buttonBox, Qt::AlignCenter);
    setLayout(layout);
    setFixedSize(300,450);


    connect(discard, &QPushButton::clicked, this, &EditProfile::discardPressed);
    connect(save, &QPushButton::clicked, this, &EditProfile::savePressed);
    connect(userPic, &ClickableLabel::clicked, this, &EditProfile::selectImagePressed);
    connect(userPic, &ClickableLabel::hovered, this, &EditProfile::imageHovered);
    connect(userPic, &ClickableLabel::unHovered, this, &EditProfile::imageUnhovered);
    connect(newPassEdit, &QLineEdit::textEdited, this, &EditProfile::comparePasswords);
    connect(usernameEdit, &QLineEdit::textEdited, this, &EditProfile::userWhitespaces);
    connect(newRepPassEdit, &QLineEdit::textEdited, this, &EditProfile::comparePasswords);

    /*modifica profilo utente*/
    QObject::connect(this, &EditProfile::SigModificaProfiloUtente, wscP, &WorkerSocketClient::modificaProfiloUtente);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoModificaProfiloUtente,this,  &EditProfile::esitoModificaProfiloUtente);

}

//**************************************************
//
//Checks whether the username has spaces in it.
//If it doesn't, the flag is set to 1 and saves
//the new value in a local QUtente. If all
//the other flags are set to 1, it enables the
//save button
//
//**************************************************

void EditProfile::userWhitespaces(){
    QString tempUsername = usernameEdit->text();
    if(tempUsername.isEmpty() || tempUsername.isNull()){
        userErr->setText("Username cannot be empty");
        userErr->setStyleSheet("QLabel {color: #FF0000}");
        save->setEnabled(false);
        userFlag = 0;
        return;
    }
    if(checkString(tempUsername)){
        userErr->setText("");
        utenteLocale->setUsername(tempUsername);
        this->repaint();
        userFlag = 1;
    }else{
        userErr->setText("Username must contain no whitespaces");
        userErr->setStyleSheet("QLabel {color: #FF0000}");
        this->repaint();
        save->setEnabled(false);
        userFlag = 0;
    }

    if(userFlag == 1 && passFlag == 1)
        save->setEnabled(true);
}


//*********************************************************************
//
//L'oggetto "QUtente" popolato con le eventuali modifiche, viene inviato
//
//*********************************************************************
void EditProfile::savePressed(){

    if(newPassEdit->text().isEmpty() || newPassEdit->text().isNull())
        utenteLocale->setPassword(recDocsUtente->getPassword());

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

//**********************************************************************
// fa la cosa opportuna sulla base dell'esito ricevuto
//
// nota:
// se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
// il costruttore senza paramteri
// se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
//
//**********************************************************************
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
        recDocsUtente->setNomeImg(userNew.getNomeImg());
        recDocsUtente->setPassword(userNew.getPassword());
        close();
    }
}

//**********************************************************
//Compares the password and password repetition.
//If they're equal and without whitespaces, it sets
//the flag at 1 and it saves the new value in a local QUtente.
//If all the flags of other fields are set to 1,
//"save" button is enabled
//*************************************************************

void EditProfile::comparePasswords(){
    QString pass = newPassEdit->text();
    QString repPass = newRepPassEdit->text();

    if(repPass.isNull() || repPass.isEmpty())
        return;
    else{
        if(pass.compare(repPass)!= 0){
            passErr->setText("Passwords must match");
            passErr->setStyleSheet("QLabel {color: #FF0000}");
            save->setEnabled(false);
            passFlag = 0;
            this->repaint();
        }
        else{
            passErr->setText("");
            this->repaint();
            passFlag = 1;
        }

        if(!checkString(pass) || !checkString(repPass)){
            passErr->setText("Password cannot contain whitespaces");
            passErr->setStyleSheet("QLabel {color: #FF0000}");
          }else if(passFlag == 1)
            utenteLocale->setPassword(pass);

        if(passFlag == 1 && userFlag == 1 && checkString(pass) && checkString(repPass))
            save->setEnabled(true);
    }

}

//***************************************************
//Checks whether the argument QString contains spaces.
//If it doesn't, return true.
//False, otherwise
//***************************************************
bool EditProfile::checkString(QString arg){
    auto it = arg.begin();
    while(it != arg.end()){
        if(it->isSpace())
            return false;
        it++;
    }
    return true;
}
