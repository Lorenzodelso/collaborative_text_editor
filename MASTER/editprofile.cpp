#include "editprofile.h"

//*********************************************************************
//
//Creazione del dialog di modifica del profilo utente.
//Il dialog viene popolato con le informazioni ricevute dal file
//"user.settings", creato precedentemente durante il login con i
//dati ricevuti dal server. Con gli stessi dati, viene creato un
//oggetto "QUser" che verrà successivamente utilizzato durante
//l'atto di modifica del profilo
//
//*********************************************************************
editProfile::editProfile(QWidget *parent)
{
    setParent(parent);
    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/").exists()){
        QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/");
    }
    setWindowTitle("User Profile editor");
    username = new QLabel(tr("&Username: "));
    usernameEdit = new QLineEdit();
    usernameEdit->setEnabled(false);
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

    if(QFileInfo::exists(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/user.settings")){
        QFile settings(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/user.settings");
        settings.open(QIODevice::ReadOnly);
        quint32 userId = settings.readLine().toUInt();
        QString usernameData = settings.readLine();
        QString nicknameData = settings.readLine();
        QString proPicName = settings.readLine();
        proPicName.resize(proPicName.size() - 1);
        usernameData.resize(usernameData.size() - 1);
        nicknameData.resize(nicknameData.size() - 1);
        settings.close();
        user = new QUtente(userId, usernameData, nicknameData, "", proPicName);
        usernameEdit->setText(usernameData);
        usernameEdit->setEnabled(false);

        if(!nicknameData.isEmpty()){
            nickEdit->setText(nicknameData);
        }else{
            nickEdit->setPlaceholderText(tr("(Optional)"));
        }
        if(proPicName.isEmpty() || proPicName.isNull()){
                profilePic->load(rsrc+"/colored-edit-profile.png");
                userPic->setPixmap(*profilePic);
        }else{
            profilePic->load(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+proPicName);
            QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
            userPic->setPixmap(scaled);
        }
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

    connect(discard, SIGNAL(clicked()), this, SLOT(discardPressed()));
    connect(save, SIGNAL(clicked()), this, SLOT(savePressed()));
    connect(userPic, SIGNAL(clicked()), this, SLOT(selectImagePressed()));
    connect(nickEdit, SIGNAL(textEdited(const QString &)), this, SLOT(changedNick(const QString &)));
    connect(userPic, SIGNAL(hovered()), this, SLOT(imageHovered()));
    connect(userPic, SIGNAL(unHovered()), this, SLOT(imageUnhovered()));

//    //**********************************************************
//    QUtente *utenteTest = new QUtente(1234, "mario", "grdnmario", "password", "");
//    esitoModificaProfiloUtente("Success", *utenteTest);
//    //**********************************************************


}


//*********************************************************************
//
//Questo slot popola il campo "nickEdit" con il nickname ricevuto
//dal file "user.settings". Nel caso il campo "nick" sia vuoto, il
//campo corrispondente di questo dialog sarà popolato con un placeholder
//
//*********************************************************************
void editProfile::changedNick(const QString &){

   QString nick = nickEdit ->text();
   nickEdit->setStyleSheet("QLineEdit {color: #000000;}");
   if(!nick.isEmpty()){
       user->setNickName(nick);
    }
}

//*********************************************************************
//
//L'oggetto "QUtente" popolato con le eventuali modifiche, viene inviato
//
//*********************************************************************
void editProfile::savePressed(){

    emit(SigModificaProfiloUtente(*user));
}

//*********************************************************************
//
//In caso di cancellazione delle modifiche, l'immagine utente
//precedentemente copiata nella cartella contenente i file utente
//verrà cancellata. Le modifiche non verranno inviate al server
//ed il dialog verrà chiuso.
//N.B. La copia dell'immagine in locale è un refuso della vecchia
//implementazione in locale. Verrà cancellato e/o modificato non appena
//sarà chiaro come l'immagine utente verra inviata al client dal server
//
//*********************************************************************
void editProfile::discardPressed(){
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+user->getNomeImg());
    close();
}


//*********************************************************************
//
//Selezione dell'immagine del profilo utente. L'immagine selezionata
//verrà copiata nella cartella contenente i file utente. Il corrispondente
//oggetto QUtente verrà aggiornato con il nuovo "nomeImg"
//
//*********************************************************************
void editProfile::selectImagePressed(){
    QUrl imageUrl = QFileDialog::getOpenFileUrl(this, tr("Open Image"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), tr("Image Files (*.png *.jpg *.bmp)"));
    QString imagePath = imageUrl.path();
    imagePath.remove(0,1);
    QString imageName = imageUrl.fileName();
    if(!imagePath.isEmpty() || !imagePath.isNull()){
        if(QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/").exists()){
            QFile::copy(imagePath, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+imageName);
        }else{
            QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/");
            QFile::copy(imagePath, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+imageName);
        }
        user->setNomeImg(imageName);

        profilePic->load(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+imageName);
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

void editProfile::imageHovered(){
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
void editProfile::imageUnhovered(){

    if(user->getNomeImg().isEmpty() || user->getNomeImg().isNull() || user->getNomeImg() == ""){
            profilePic->load(rsrc+"/colored-edit-profile.png");
            userPic->setPixmap(*profilePic);
    }else{
            profilePic->load(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+user->getNomeImg());
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
void editProfile::esitoModificaProfiloUtente(QString esito/*esito*/, QUtente userNew){
    if(esito == "Failed"){
        QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+user->getNomeImg());
        user = new QUtente(0,"","","","");
        QMessageBox msgBox;
        msgBox.setText("Something went wrong. Please, try again");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }else{
        user = new QUtente(userNew.getUserId(), userNew.getUsername(), userNew.getNickName(), userNew.getPassword(), userNew.getNomeImg());
        QFile userSettings (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/user.settings");
        userSettings.open(QIODevice::WriteOnly);
        QTextStream out(&userSettings);
        out << user->getUserId() << endl
            << user->getUsername() << endl
            << user->getNickName() << endl
            << user->getNomeImg() << endl;
        userSettings.close();
        close();
    }


}
