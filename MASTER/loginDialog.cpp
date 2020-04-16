#include <QtWidgets>
#include "loginDialog.h"
#include "textedit.h"
#include "recentdocsdialogs.h"


loginDialog::loginDialog(QWidget *parent, WorkerSocketClient* wscP)
    :QWidget(parent){
    this->setWindowTitle("Login");
//********************************************************************************************
//
//Creazione del dialog di login, aggiungendo buttons e lineEdit.
//Connessione di slot e segnali necessari per il funzionamento.
//
//********************************************************************************************
    setParent(parent);
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

    connect(usernameEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableLoginButton()));
    connect(passwordEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableLoginButton()));
    connect(loginButton, SIGNAL(clicked()),
            this, SLOT(loginClicked()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(cancelClicked()));
    connect(newProfileButton, SIGNAL(clicked()), this, SLOT(registerClicked()));

    /*settaggio connessione*/
    QObject::connect(this, &loginDialog::SigConnessioneAlServer, wscP, &WorkerSocketClient::connessioneAlServer);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoConnessioneAlServer, this,  &loginDialog::esitoConnessioneAlServer);

    /*login*/
    QObject::connect(this, &loginDialog::SigLogin, wscP, &WorkerSocketClient::login);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoLogin, this,  &loginDialog::esitoLogin);


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

    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData").exists()){
            QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData");
    }

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
void loginDialog::loginClicked(){
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QUtente *user = new QUtente(0, username, "", password, "");
    emit(SigLogin(*user));
}


//*********************************************************************
//
//Chiusura dell'applicazione
//
//*********************************************************************
void loginDialog::cancelClicked(){

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
void loginDialog::enableLoginButton(){
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
void loginDialog::registerClicked(){
    newProfile = new newProfileDialog(this);
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

void loginDialog::esitoLogin(QString esito/*esito*/, QUtente user, QList<QString> nomiFilesEditati){
    QUtente *utente;
    if(esito == "Failed"){
        QMessageBox msgBox;
        msgBox.setText(tr("Username and/or password are wrong. Please, try again"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }else{
        utente = new QUtente(user.getUserId(), user.getUsername(), user.getNickName(), user.getPassword(), user.getNomeImg());
        QList<QString> editedFiles = nomiFilesEditati;
        QFile userSettings (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/user.settings");
        userSettings.open(QIODevice::WriteOnly);
        QTextStream out(&userSettings);
        out << utente->getUserId() << endl
            << utente->getUsername() << endl
            << utente->getNickName() << endl
            << utente->getNomeImg() << endl;
        userSettings.close();
        QFile recentDocs(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/CollaborativeEditor/usrData/docs.recent");
        recentDocs.open(QIODevice::WriteOnly);
        QTextStream out2(&recentDocs);
        QList<QString>::iterator iterator = editedFiles.begin();
        while(iterator != editedFiles.end()){
            out2 << *iterator << endl;
            iterator++;
        }
        recentDocs.close();
        this->setAttribute(Qt::WA_DeleteOnClose);
        recDoc = new recentDocsDialogs(0);
        recDoc->show();
        this->close();
    }

}

void loginDialog::attivaSocket(){
    emit(SigConnessioneAlServer());
}

void loginDialog::disattivaSocket(){
    emit(SigDisconnessioneDalServer());
}

QString loginDialog::esitoConnessioneAlServer(QString esito){
    return esito;
}
