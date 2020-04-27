

#include "WorkerSocketClient.h"
#include  <QTcpSocket>
#include <QHostAddress>
#include<QThread>



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

    QString msg = socketConnesso->readLine();

        if (msg.compare("esito_Apri_doc",Qt::CaseSensitivity::CaseSensitive)==0) {


            QString msg1 = socketConnesso->readLine();

        if (msg.compare("open",Qt::CaseSensitivity::CaseSensitive)==0) {

            CRDT doc;

            in >> doc;

            SigEsitoCreaDoc(msg1/*esito*/, doc/*rappresentazione del file*/);
        }
        else{


            CRDT doc=*new CRDT();

            SigEsitoCreaDoc(msg1, doc);
        }
    }


        if (msg.compare("esito_Crea_doc",Qt::CaseSensitivity::CaseSensitive)==0) {

            QString msg1 = socketConnesso->readLine();

        if (msg.compare("create",Qt::CaseSensitivity::CaseSensitive)==0) {

            CRDT doc;

            in >> doc;

            SigEsitoCreaDoc( msg1/*esito*/,  doc/*rappresentazione del file*/);
        }

        else{

            CRDT doc =*new CRDT(0);

            SigEsitoCreaDoc(msg1, doc);
        }
    }


        if (msg.compare("esito_login",Qt::CaseSensitivity::CaseSensitive)==0) {

            QString msg1 = socketConnesso->readLine();

         if (msg.compare("success",Qt::CaseSensitivity::CaseSensitive)==0) {

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
        if (msg.compare("OpDocRemota",Qt::CaseSensitivity::CaseSensitive)==0) {

            DocOperation* docOp = new DocOperation();

            SigOpDocRemota(/*esito*/*docOp);
        }

        if (msg.compare("esitochiusura",Qt::CaseSensitivity::CaseSensitive)==0) {

            QString msg1 = socketConnesso->readLine();

            SigEsitoChiudiDoc( msg1/*esito*/);
        }


        if (msg.compare("modificaprofilo",Qt::CaseSensitivity::CaseSensitive)==0) {

            QString msg1 = socketConnesso->readLine();

            if (msg1.compare("success",Qt::CaseSensitivity::CaseSensitive)==0) {

            QUtente userNew;

            in >> userNew;

            SigEsitoModificaProfiloUtente(msg1/*esito*/,  userNew);
        }

        else{

            QUtente user=*new QUtente();

            SigEsitoModificaProfiloUtente(msg1/*esito*/, user);
        }
    }


        if (msg.compare("esito_operazione",Qt::CaseSensitivity::CaseSensitive)==0) {

        QString msg1 = socketConnesso->readLine();

        if (msg1.compare("Success",Qt::CaseSensitivity::CaseSensitive)==0) {

            DocOperation operazione;

            in >> operazione;

            SigEsitoOpDocLocale(msg1,operazione);

        }

        else{

            DocOperation operazione= *new DocOperation();

            SigEsitoOpDocLocale(msg1,operazione);

        }
    }


        if (msg.compare("esito_registrazione",Qt::CaseSensitivity::CaseSensitive)==0) {

        QString msg1 = socketConnesso->readLine();

        if (msg1.compare("Success",Qt::CaseSensitivity::CaseSensitive)==0) {

            QList <QString> nomiFilesEditati;


            SigEsitoRegistrazione(msg1/*esito*/);

        } else{


            SigEsitoRegistrazione( msg1/*esito*/);
          }
    }

        if (msg.compare("chi_ha_inserito_cosa",Qt::CaseSensitivity::CaseSensitive)==0) {

        QList <QUtente> utenti;

        in >> utenti;
    }


}

void WorkerSocketClient::opDocLocale(DocOperation operazione){

    QDataStream in(this->socketConnesso);

    in<<"opdoclocale"<<"\n";

    in <<operazione<< "\n";

}

void WorkerSocketClient::apriDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    in <<"open"<< "\n";

    in <<nomeFile<< "\n";

}

void WorkerSocketClient::creaDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    in <<"create"<< "\n";

    in <<nomeFile<< "\n";

}

void WorkerSocketClient::login(QUtente user){


    QDataStream in(this->socketConnesso);

    in <<"login"<< "\n";

    in <<user<< "\n";

}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1) {


    QDataStream in(this->socketConnesso);

    in << "modify" << "\n";

    in << user1 << "\n";

}

void WorkerSocketClient::registrazione(QUtente user){


    QDataStream in(this->socketConnesso);
    //in << "register\n";
    socketConnesso->write("register\n");

    in << user << "\n";

}

void WorkerSocketClient::chiudiDoc(QString nomeFile){


    QDataStream in(this->socketConnesso);

    in << "close" << "\n";

    in << nomeFile << "\n";


}

void WorkerSocketClient::opChiHaInseritoCosa(){

    QDataStream in(this->socketConnesso);

    in << "chi_inserito" << "\n";



}
