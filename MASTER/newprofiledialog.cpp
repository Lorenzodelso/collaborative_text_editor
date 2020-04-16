#include "newprofiledialog.h"
#include "loginDialog.h"

//*********************************************************************
//
//Creazione del dialog per registrare un nuovo profilo utente.
//
//*********************************************************************

newProfileDialog::newProfileDialog(QWidget *parent)
{
    setParent(parent);
    setWindowFlag(Qt::Window);
    setModal(true);
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
    registerUser->setEnabled(false);
    abort = new QPushButton(tr("Cancel"));

    nickname = new QLabel(tr("Nickname: "));
    nickEdit = new QLineEdit();
    userPic = new ClickableLabel();
    userPic->setBackgroundRole(QPalette::Dark);
    userPic->setScaledContents(false);
    profilePic = new QPixmap();
    profilePic->load(rsrc+"/colored-edit-profile.png");
    userPic->setPixmap(*profilePic);

    err = new QLabel("");

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(registerUser, QDialogButtonBox::ActionRole);
    buttonBox->addButton(abort, QDialogButtonBox::ActionRole);

    QVBoxLayout *layout = new QVBoxLayout();
    QFormLayout *formLayout = new QFormLayout();

    connect(registerUser, SIGNAL(clicked()), this, SLOT(registerPressed()));
    connect(abort, SIGNAL(clicked()), this, SLOT(abortPressed()));
    connect(passEdit, SIGNAL(textEdited(const QString &)), this, SLOT(comparePasswords()));
    connect(repPassEdit, SIGNAL(textEdited(const QString &)), this, SLOT(comparePasswords()));
    connect(userPic, SIGNAL(hovered()), this, SLOT(imageHovered()));
    connect(userPic, SIGNAL(unHovered()), this, SLOT(imageUnhovered()));
    connect(userPic, SIGNAL(clicked()), this, SLOT(selectImagePressed()));

    layout->addWidget(userPic);
    layout->setAlignment(userPic, Qt::AlignCenter);
    layout->addSpacing(15);
    formLayout->addRow(username, userEdit);
    formLayout->setSpacing(15);
    formLayout->addRow(nickname, nickEdit);
    formLayout->setSpacing(15);
    formLayout->addRow(password, passEdit);
    formLayout->setSpacing(15);
    formLayout->addRow(repeatPassword, repPassEdit);
    layout->addItem(formLayout);
    layout->addSpacing(5);
    layout->addWidget(err);
    layout->setSpacing(10);
    layout->addWidget(buttonBox);
    setLayout(layout);
    setFixedSize(sizeHint());
    utente = new QUtente(0,"","","","");

//    //**********************************************************
//    QUtente *utenteTest = new QUtente(1234, "mario", "mariogrdn", "password", "");
//    QList<QString> *listaTest = new QList<QString>();
//    listaTest->append("ciao");
//    listaTest->append("come");
//    listaTest->append("stai");
//    esitoRegistrazione("Success", *utenteTest, *listaTest);
//    //**********************************************************
}

//*********************************************************************
//
//Slot che chiude il dialog corrente quando viene premuto il tasto
//chiudi o "cancel"
//
//*********************************************************************

void newProfileDialog::abortPressed(){
    parentWidget()->show();
    close();
}


//*********************************************************************
//
//Lo slot si occupa di popolare un oggetto QUtente con i dati inseriti
//nel dialog di registrazione. L'oggetto QUtente verrà trasmesso al server
//solo nel caso in cui le password inserite siano uguali e non vuote
//e il campo username non sia vuoto.
//
//*********************************************************************
void newProfileDialog::registerPressed(){
    QString user = userEdit->text();
    QString pass = passEdit->text();
    QString nick = nickEdit->text();

    if(user.isNull() || user.isEmpty()){
        err->setText("Username cannot be empty");
        err->setStyleSheet("QLabel {color: #FF0000}");
        return;
    }else{
        utente->setUserId(0);
        utente->setPassword(pass);
        utente->setUsername(user);
        utente->setNickName(nick);

        emit(SigRegistrazione(*utente));
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
void newProfileDialog::comparePasswords(){
    QString pass = passEdit->text();
    QString repPass = repPassEdit->text();

    if(repPass.isNull() || repPass.isEmpty())
        return;
    else{
        if(pass.compare(repPass)!= 0){
            err->setText("Passwords must match");
            err->setStyleSheet("QLabel {color: #FF0000}");
            this->repaint();
        }
        else{
            err->setText("");
            this->repaint();
            registerUser->setEnabled(true);
        }
    }

}

//*********************************************************************
//
//Slot che fa visualizzare l'immagine standard di "aggiunta immagine
//profilo" all'utente ogni volta che l'immagine corrente viene
//toccata dal puntatore del mouse
//
//*********************************************************************
void newProfileDialog::imageHovered(){
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
void newProfileDialog::imageUnhovered(){

    if(utente->getNomeImg().isEmpty() || utente->getNomeImg().isNull()){
            profilePic->load(rsrc+"/colored-edit-profile.png");
            userPic->setPixmap(*profilePic);
    }else{
            profilePic->load(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/"+utente->getNomeImg());
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
void newProfileDialog::selectImagePressed(){
    QUrl imageUrl = QFileDialog::getOpenFileUrl(this, tr("Open Image"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), tr("Image Files (*.png *.jpg *.bmp)"));
    QString imagePath = imageUrl.path();
    imagePath.remove(0,1);
    QString imageName = imageUrl.fileName();
    utente->setNomeImg(imageName);
    profilePic->load(imagePath);
    QPixmap scaled = profilePic->scaled(147, 200, Qt::AspectRatioMode::KeepAspectRatio);
    userPic->setPixmap(scaled);

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
void newProfileDialog::esitoRegistrazione(QString esito/*esito*/){
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

