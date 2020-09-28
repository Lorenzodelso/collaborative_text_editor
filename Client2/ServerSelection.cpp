#include "ServerSelection.h"

ServerSelection::ServerSelection(QWidget *parent, WorkerSocketClient *wscP):QWidget(parent), wscP(wscP)
{
    setWindowTitle("Connect");
    setAttribute(Qt::WA_DeleteOnClose);
    address = new QLineEdit();
    address->setPlaceholderText("Insert IP");
    address->setEnabled(true);
    addressLabel = new QLabel("Server IP Address: ");
    connectButton = new QPushButton("Connect");
    connectButton->setEnabled(false);
    connectButton->setFixedSize(80,25);
    closeButton = new QPushButton("Close");
    closeButton->setFixedSize(60,25);
    addressLabel->setBuddy(address);
    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(connectButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(addressLabel);
    hLayout->addWidget(address);
    hLayout->addSpacing(10);
    hLayout->setAlignment(addressLabel, Qt::AlignLeft);
    hLayout->setAlignment(address, Qt::AlignRight);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addSpacing(10);
    vLayout->addWidget(buttonBox);
    setLayout(vLayout);
    setFixedSize(300,100);

    QObject::connect(connectButton, &QPushButton::pressed, this, &ServerSelection::attivaSocket);
    QObject::connect(address, &QLineEdit::textEdited, this, &ServerSelection::activateConnect);
    QObject::connect(closeButton, &QPushButton::pressed, this, &ServerSelection::closePressed);

    /*settaggio connessione*/
    QObject::connect(this, &ServerSelection::SigConnessioneAlServer, wscP, &WorkerSocketClient::connessioneAlServer);
    QObject::connect(wscP, &WorkerSocketClient::SigEsitoConnessioneAlServer, this,  &ServerSelection::esitoConnessioneAlServer);

}

void ServerSelection::activateConnect(){
    if(std::regex_match(address->text().toStdString(), ipv4))
        connectButton->setEnabled(true);
    else
        connectButton->setEnabled(false);
}

void ServerSelection::closePressed(){
    QCoreApplication::quit();
}

void ServerSelection::attivaSocket(){
    emit SigConnessioneAlServer(address->text());
}

void ServerSelection::esitoConnessioneAlServer(QString esito){
    if(esito.compare("Success") == 0){
        ld = new LoginDialog(0, wscP);
        ld->show();
        close();
    }else{
        QMessageBox msgBox;
        msgBox.setText(tr("A problem occured while connecting to the server. Please, try again"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}
