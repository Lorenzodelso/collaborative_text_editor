

#include "WorkerSocketClient.h"
#include  <QTcpSocket>
#include <QHostAddress>
#include<QThread>
#include <windows.h>
#include <QImage>




void WorkerSocketClient::connessioneAlServer() {


    this->socketConnesso = new QTcpSocket( this );

    this->socketConnesso->connectToHost(QHostAddress::LocalHost, 3030 );

    bool connected = (socketConnesso->state() == QTcpSocket::ConnectedState);

    if(connected==true)

        SigEsitoConnessioneAlServer("Success");

    else

        SigEsitoConnessioneAlServer("Failed");


    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp);


}


void WorkerSocketClient::disconnessioneDalServer(){

    this->socketConnesso->disconnect();
    this->socketConnesso->disconnectFromHost();

    bool connected = (this->socketConnesso->state() == QTcpSocket::ConnectedState);

    if(connected==false)

        disconnessioneDalServer();

    else

        SigEsitoConnessioneAlServer("Failed");

}

void  WorkerSocketClient::leggiMsgApp(){

    QDataStream in(this->socketConnesso);

    char* msg;
    uint prova = 3;
    in.readBytes(msg,prova);

    if (msg.compare("ead",Qt::CaseSensitivity::CaseSensitive)==0) { //ok


        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        if (msg.compare("opn",Qt::CaseSensitivity::CaseSensitive)==0) {

            CRDT doc;

            in >> doc;

            SigEsitoApriDoc(msg1/*esito*/, doc/*rappresentazione del file*/);
        }
        else{


            CRDT doc=*new CRDT();

            SigEsitoApriDoc(msg1, doc);
        }
    }


    if (msg.compare("ecd",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        if (msg1.compare("crt",Qt::CaseSensitivity::CaseSensitive)==0) {

            CRDT doc;

            in >> doc;

            SigEsitoCreaDoc( msg1/*esito*/,  doc/*rappresentazione del file*/);
        }

        else{

            CRDT doc =*new CRDT(0);

            SigEsitoCreaDoc(msg1, doc);
        }
    }


    if (msg.compare("e_l",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        if (msg.compare("suc",Qt::CaseSensitivity::CaseSensitive)==0) {

            QList<QString> nomiFilesEditati;

            QUtente user = *new QUtente();


            in >> user;

            in >> nomiFilesEditati;

            SigEsitoLogin( msg1/*esito*/, user, nomiFilesEditati);
        }

        else {

            QList <QString> nomiFilesEditati;

            QUtente user = *new QUtente();

            SigEsitoLogin(msg1, user, nomiFilesEditati);
        }
    }
    if (msg.compare("e_o",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        DocOperation* docOp = new DocOperation();

        SigOpDocRemota(/*esito*/*docOp);
    }

    if (msg.compare("e_c",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        SigEsitoChiudiDoc( msg1/*esito*/);
    }


    if (msg.compare("mop",Qt::CaseSensitivity::CaseSensitive)==0) { //mop

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        if (msg1.compare("suc",Qt::CaseSensitivity::CaseSensitive)==0) {

            QUtente userNew;

            in >> userNew;

            SigEsitoModificaProfiloUtente(msg1/*esito*/,  userNew);
        }

        else{

            QUtente user=*new QUtente();

            SigEsitoModificaProfiloUtente(msg1/*esito*/, user);
        }
    }


    if (msg.compare("e_o",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        if (msg1.compare("suc",Qt::CaseSensitivity::CaseSensitive)==0) {

            DocOperation operazione;

            in >> operazione;

            SigEsitoOpDocLocale(msg1,operazione);

        }

        else{

            DocOperation operazione= *new DocOperation();

            SigEsitoOpDocLocale(msg1,operazione);

        }
    }


    if (msg.compare("e_r",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        char* msg1;
        in.readBytes(msg1,prova);

        if (msg1.compare("suc",Qt::CaseSensitivity::CaseSensitive)==0) {

            QList <QString> nomiFilesEditati;


            SigEsitoRegistrazione(msg1/*esito*/);

        } else{


            SigEsitoRegistrazione( msg1/*esito*/);
        }
    }

    if (msg.compare("c_i",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        QList <QUser> utenti;

        in >> utenti;
    }

    if (msg.compare("ucd",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        Quser utente;

        in >> utente;

        SigQuestoUserHaChiusoIlDoc(QUser utente);
    }

    if (msg.compare("uad",Qt::CaseSensitivity::CaseSensitive)==0) { //ok

        Quser utente;

        in >> utente;

        SigQuestoUserHaApertoIlDoc(QUser utente);
    }


}

void WorkerSocketClient::opDocLocale(DocOperation operazione){

    QDataStream in(this->socketConnesso);

    socketConnesso->write("opd"); //ok

    in <<operazione<< "\n";

}

void WorkerSocketClient::apriDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    socketConnesso->write("ope"); //ok

    in <<nomeFile<< "\n";

}

void WorkerSocketClient::creaDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    socketConnesso->write("cre"); //ok

    in <<nomeFile<< "\n";

}

void WorkerSocketClient::login(QUtente user){


    QDataStream in(this->socketConnesso);

    socketConnesso->write("log"); //ok

    in <<user<< "\n";

}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1) {


    QDataStream in(this->socketConnesso);

    socketConnesso->write("mod"); //ok

    in << user1 << "\n";

}

void WorkerSocketClient::registrazione(QUtente user){

    char fullpath[100];
    char name[100]

    QDataStream in(this->socketConnesso);

    socketConnesso->write("reg"); //ok

    GetFullPathName(user.getNomeImg(), 100, fullpath, name);

    QImage image(fullpath);

    QBuffer buffer;

    image.save(&buffer, "PNG");

    in << buffer.size()

    in << buffer.data();

    in << user << "\n";

}

void WorkerSocketClient::chiudiDoc(QString nomeFile){


    QDataStream in(this->socketConnesso);

    socketConnesso->write("c_d"); //ok

    in << nomeFile << "\n";


}

void WorkerSocketClient::opChiHaInseritoCosa(){

    QDataStream in(this->socketConnesso);

    in << "c_i" << "\n"; //ok



}
