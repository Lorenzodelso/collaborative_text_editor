#include "NewProfileDialog.h"
#include "LoginDialog.h"

//*********************************************************************
//
//Creazione del dialog per registrare un nuovo profilo utente.
//
//*********************************************************************

NewProfileDialog::NewProfileDialog(QWidget *parent, WorkerSocketClient* wscP)
{
    qRegisterMetaType<QUtente>();
    setParent(parent);
    setObjectName("newProfile");
    setWindowFlag(Qt::Window);
    setModal(true);
    this->wscP = wscP;
    username = new QLabel(tr("Username: "));
    userEdit = new QLineEdit();
    username->setBuddy(userEdit);

    password = new QLabel(tr("Password: "));
    passEdit = new QLineEdit();
    passEdit->setEchoMode(QLineEdit::Password);
    password->setBuddy(passEdit);

    repeatPassword = new QLabel(tr("Repeat Password: "));
    repPassEdit = new QLineEdit();
    repPassEdit->setEchoMode(QLineEdit::Password);
    password->setBuddy(repPassEdit);

    registerUser = new QPushButton(tr("Register"));
    registerUser->setFixedSize(80,25);
    registerUser->setEnabled(false);
    abort = new QPushButton(tr("Cancel"));
    abort->setFixedSize(70,25);

    userPic = new ClickableLabel();
    userPic->setBackgroundRole(QPalette::Dark);
    userPic->setScaledContents(false);
    profilePic = new QPixmap();
    profilePic->load(rsrc+"/colored-edit-profile.png");
    userPic->setPixmap(*profilePic);

    userErr = new QLabel("");
    passErr = new QLabel("");

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(registerUser, QDialogButtonBox::ActionRole);
    buttonBox->addButton(abort, QDialogButtonBox::ActionRole);

    QVBoxLayout *layout = new QVBoxLayout();
    QFormLayout *formLayout = new QFormLayout();

    connect(registerUser, &QPushButton::clicked, this, &NewProfileDialog::registerPressed);
    connect(abort, &QPushButton::clicked, this, &NewProfileDialog::abortPressed);
    connect(passEdit, &QLineEdit::textEdited, this, &NewProfileDialog::comparePasswords);
    connect(userEdit, &QLineEdit::textEdited, this, &NewProfileDialog::userWhitespaces);
    connect(repPassEdit, &QLineEdit::textEdited, this, &NewProfileDialog::comparePasswords);
    connect(userPic, &ClickableLabel::hovered, this, &NewProfileDialog::imageHovered);
    connect(userPic, &ClickableLabel::unHovered, this, &NewProfileDialog::imageUnhovered);
    connect(userPic, &ClickableLabel::clicked, this, &NewProfileDialog::selectImagePressed);

    /*registrazione*/
    QObject::connect(this, &NewProfileDialog::SigRegistrazione, wscP, &WorkerSocketClient::registrazione);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoRegistrazione,this,  &NewProfileDialog::esitoRegistrazione);

    layout->addWidget(userPic);
    layout->setAlignment(userPic, Qt::AlignCenter);
    layout->addSpacing(15);
    formLayout->addRow(username, userEdit);
    formLayout->addRow(userErr);
    formLayout->setSpacing(5);
    formLayout->addRow(password, passEdit);
    formLayout->setSpacing(5);
    formLayout->addRow(repeatPassword, repPassEdit);
    formLayout->addRow(passErr);
    layout->addItem(formLayout);
    layout->addSpacing(5);
    layout->addWidget(buttonBox);
    setLayout(layout);
    setFixedSize(300,450);
    utente = new QUtente(0,"","","");
}

//*********************************************************************
//
//Slot che chiude il dialog corrente quando viene premuto il tasto
//chiudi o "cancel"
//
//*********************************************************************

void NewProfileDialog::abortPressed(){
    parentWidget()->show();
    close();
}

void NewProfileDialog::userWhitespaces(){
    QString tempUsername = userEdit->text();
    if(checkString(tempUsername)){
        userErr->setText("");
        this->repaint();
        userFlag = 1;
    }else{
        userErr->setText("Username must contain no whitespaces");
        userErr->setStyleSheet("QLabel {color: #FF0000}");
        this->repaint();
        userFlag = 0;
        registerUser->setEnabled(false);
    }

    if(userFlag == 1 && passFlag == 1)
        registerUser->setEnabled(true);
}

//*********************************************************************
//
//Lo slot si occupa di popolare un oggetto QUtente con i dati inseriti
//nel dialog di registrazione. L'oggetto QUtente verrà trasmesso al server
//solo nel caso in cui le password inserite siano uguali e non vuote
//e il campo username non sia vuoto.
//
//*********************************************************************

void NewProfileDialog::registerPressed(){
    QString user = userEdit->text();
    QString pass = passEdit->text();

    if(utente->getNomeImg().isNull() || utente->getNomeImg().isEmpty())
        utente->setNomeImg(nullptr);

    if(user.isNull() || user.isEmpty()){
        userErr->setText("Username cannot be empty");
        userErr->setStyleSheet("QLabel {color: #FF0000}");
        return;
    }else{
        utente->setUserId(0);
        utente->setPassword(pass);
        utente->setUsername(user);
        emit SigRegistrazione(*utente);
    }
}


//*********************************************************************
//
//Questo slot si occupa, ad ogni modifica del campo "passEdit" e del
//campo "repPassEdit", di controllare che le password inserite siano
//uguali. Nel caso non lo fossero, il tasto "register" rimarrà disabilitato
//e comparirà un messaggio d'errore.
//
//*********************************************************************

void NewProfileDialog::comparePasswords(){
    QString pass = passEdit->text();
    QString repPass = repPassEdit->text();

    if(repPass.isNull() || repPass.isEmpty())
        return;
    else{
        if(pass.compare(repPass)!= 0){
            passErr->setText("Passwords must match");
            passErr->setStyleSheet("QLabel {color: #FF0000}");
            registerUser->setEnabled(false);
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
          }

        if(passFlag == 1 && userFlag == 1 && checkString(pass) && checkString(repPass))
            registerUser->setEnabled(true);
    }

}

//*********************************************************************
//
//Slot che fa visualizzare l'immagine standard di "aggiunta immagine
//profilo" all'utente ogni volta che l'immagine corrente viene
//toccata dal puntatore del mouse
//
//*********************************************************************

void NewProfileDialog::imageHovered(){
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

void NewProfileDialog::imageUnhovered(){

    if(utente->getNomeImg().isEmpty() || utente->getNomeImg().isNull()){
        profilePic->load(rsrc+"/colored-edit-profile.png");
        userPic->setPixmap(*profilePic);
    }else{
        profilePic->load(utente->getNomeImg());
        QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
        userPic->setPixmap(scaled);
    }
}

//*********************************************************************
//
//Selezione dell'immagine del profilo utente. L'immagine selezionata
//verrà copiata nella cartella contenente i file utente. Il corrispondente
//oggetto QUtente verrà aggiornato con il nuovo "nomeImg"
//
//*********************************************************************

void NewProfileDialog::selectImagePressed(){
    QUrl imageUrl = QFileDialog::getOpenFileUrl(this, tr("Open Image"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), tr("Image Files (*.png *.jpg *.bmp)"));
    if(!imageUrl.isEmpty()){
        QString imagePath = imageUrl.path();
        #ifndef Q_OS_MAC
        imagePath.remove(0,1);
        #endif
        utente->setNomeImg(imagePath);
        profilePic->load(imagePath);
        QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
        userPic->setPixmap(scaled);
    }
}


//*********************************************************************************
// fa la cosa opportuna sulla base dell'esito ricevuto
//
// nota:
// se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
// il costruttore senza paramteri
// se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
//
//*********************************************************************************
void NewProfileDialog::esitoRegistrazione(QString esito){
    if(esito == "Failed"){
        QMessageBox msgBox;
        msgBox.setText("An error occured while signing up. Please, try again.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }else{
        QMessageBox msgBox;
        msgBox.setText("Successful Registration");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        parentWidget()->show();
        this->close();
    }
}

//*********************************************************
//
//Checks whether arg contains whitespaces. If it does, it
//returns false. True otherwise
//
//*********************************************************

bool NewProfileDialog::checkString(QString arg){
    auto it = arg.begin();
    while(it != arg.end()){
        if(it->isSpace())
            return false;
        it++;
    }
    return true;
}

