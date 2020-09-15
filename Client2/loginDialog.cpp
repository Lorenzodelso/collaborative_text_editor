#include <QtWidgets>
#include "LoginDialog.h"
#include "TextEdit.h"
#include "RecentDocsDialogs.h"


LoginDialog::LoginDialog(QWidget *parent, WorkerSocketClient* wscP)
    :QWidget(parent){
    this->setWindowTitle("Login");
//********************************************************************************************
//
//Creazione del dialog di login, aggiungendo buttons e lineEdit.
//Connessione di slot e segnali necessari per il funzionamento.
//
//********************************************************************************************
    setParent(parent);
    this->wscP = wscP;
    username = new QLabel(tr("Username: "));
    usernameEdit = new QLineEdit;
    password = new QLabel(tr("Password: "));
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    username->setBuddy(usernameEdit);
    password->setBuddy(passwordEdit);
    loginButton = new QPushButton(tr("&Login"));
    loginButton->setDefault(true);
    loginButton->setEnabled(false);
    cancelButton = new QPushButton(tr("&Cancel"));
    newProfileButton = new QPushButton(tr("&Create a new profile"));
    newProfileButton->setFlat(true);
    newProfileButton->setStyleSheet("QPushButton {color: #FFFFFF;}");
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(loginButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);
    //setModal(true);

    connect(usernameEdit, &QLineEdit::textChanged, this, &LoginDialog::enableLoginButton);
    connect(passwordEdit, &QLineEdit::textChanged, this, &LoginDialog::enableLoginButton);
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::loginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::cancelClicked);
    connect(newProfileButton, &QPushButton::clicked, this, &LoginDialog::registerClicked);

    /*settaggio connessione*/
    QObject::connect(this, &LoginDialog::SigConnessioneAlServer, wscP, &WorkerSocketClient::connessioneAlServer);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoConnessioneAlServer, this,  &LoginDialog::esitoConnessioneAlServer);

    /*login*/
    QObject::connect(this, &LoginDialog::SigLogin, wscP, &WorkerSocketClient::login);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoLogin, this,  &LoginDialog::esitoLogin);


    QVBoxLayout *layout = new QVBoxLayout();
    QFormLayout *loginForm = new QFormLayout;
    loginForm->addRow(username, usernameEdit);
    loginForm->addRow(password, passwordEdit);
    layout->addLayout(loginForm);
    layout->addSpacing(15);
    layout->addWidget(buttonBox);
    layout->setAlignment(buttonBox, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(newProfileButton);
    layout->setAlignment(newProfileButton, Qt::AlignCenter);
    setLayout(layout);
    setFixedSize(sizeHint());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

//    //**********************************************************
//    QUtente *utenteTest = new QUtente(1234, "mario", "mariogrdn", "password", "");
//    QList<QString> *listaTest = new QList<QString>();
//    listaTest->append("ciao");
//    listaTest->append("come");
//    listaTest->append("stai");
//    esitoLogin("Success", *utenteTest, *listaTest);
//    //**********************************************************
}


//*********************************************************************
//
//Creazione di un oggetto QUtente contentente username e password
//inseriti in fase di login.
//Il suddetto oggetto viene mandato allo slot corrispondente
//mediante il segnale SigLogin()
//
//*********************************************************************
void LoginDialog::loginClicked(){
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QUtente *user = new QUtente(0, username, password, "");
    emit(SigLogin(*user));
}


//*********************************************************************
//
//Chiusura dell'applicazione
//
//*********************************************************************
void LoginDialog::cancelClicked(){

    QCoreApplication::quit();

}


//*********************************************************************
//
//Abilitazione del pulsante di login, o eventuale disabilitazione,
//a seconda del fatto che i campi "Username" e "Password" siano
//pieni o no
//
//
//*********************************************************************
void LoginDialog::enableLoginButton(){
    if(!usernameEdit->text().isEmpty() && !passwordEdit->text().isEmpty())
        loginButton->setEnabled(true);
    else
        loginButton->setEnabled(false);
}

//*********************************************************************
//
//Creazione e avvio del dialog di registrazione di un nuovo profilo
//
//*********************************************************************
void LoginDialog::registerClicked(){
    newProfile = new NewProfileDialog(this, this->wscP);
    newProfile->show();
    this->hide();
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

void LoginDialog::esitoLogin(QString esito/*esito*/, QUtente user, QList<QString> nomiFilesEditati){
    if(esito == "Failed"){
        QMessageBox msgBox;
        msgBox.setText(tr("Username and/or password are wrong. Please, try again"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }else{
        this->setAttribute(Qt::WA_DeleteOnClose);
        recDoc = new RecentDocsDialogs(0, this->wscP,user.getUserId(), user, nomiFilesEditati);
        recDoc->show();
        this->close();
    }

}

void LoginDialog::attivaSocket(){
    emit SigConnessioneAlServer();
}

QString LoginDialog::esitoConnessioneAlServer(QString esito){
    return esito;
}
