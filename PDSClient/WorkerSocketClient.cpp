

#include "WorkerSocketClient.h"
#include  <QTcpSocket>
#include <QHostAddress>
#include<QThread>
#include <windows.h>
#include <QImage>
#include <QDebug>




void WorkerSocketClient::connessioneAlServer() {

    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QList<QString>>();


    this->socketConnesso = new QTcpSocket( this );

    this->socketConnesso->connectToHost(QHostAddress::LocalHost, 3030 );

    bool connected = (socketConnesso->state() == QTcpSocket::ConnectedState);

    if(connected==true)

        SigEsitoConnessioneAlServer("Success");

    else

        SigEsitoConnessioneAlServer("Failed");


    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp,Qt::QueuedConnection);


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

    qDebug()<<"inizio lettura";


    QDataStream in(this->socketConnesso);

    in.startTransaction();

    if(strcmp(this->msg,"second_read")!=0)  {

        uint prova = 3;
        in.readBytes(this->msg, prova);

    }



    if (strcmp(this->msg,"ead")==0)  { //ok



        if(strcmp(this->msg,"second_read")==0)  {

            uint prova = 3;
            in.readBytes(this->opt, prova);

        }

        if (strcmp(this->msg,"opn")==0)  {

            CRDT doc;

            in >> doc;

            char* msg1;
            uint prova = 3;
            in.readBytes(msg1,prova);

            if(in.commitTransaction()==true){

                SigEsitoApriDoc(msg1/*esito*/, doc/*rappresentazione del file*/);
                this->msg="";
            }

            else{
                this->msg="second_read";
                this->opt="opn";

                }

        }
        else{


            CRDT doc=*new CRDT();

            SigEsitoApriDoc(this->opt, doc);
        }
    }


    if (strcmp(this->msg,"ecd")==0)  { //ok


        if(strcmp(this->msg,"second_read"))  {

            uint prova = 3;
            in.readBytes(this->opt, prova);

        }

        if (strcmp(this->msg,"crt")==0)  {

            CRDT doc;

            in >> doc;

            char* msg1;
            uint prova = 3;
            in.readBytes(msg1,prova);


            if(in.commitTransaction()==true){

                SigEsitoCreaDoc( msg1/*esito*/,  doc/*rappresentazione del file*/);
                this->msg="";
            }

            else{
                this->msg="second_read";
                this->opt="opn";

            }

        }

        else{

            CRDT doc =*new CRDT(0);

            SigEsitoCreaDoc(this->opt, doc);
        }
    }


    if(strcmp(this->msg,"e_l")==0) { //ok



        if(strcmp(this->msg,"second_read")!=0)  {

            uint prova = 3;
            in.readBytes(this->opt, prova);

        }


        if(strcmp(this->msg,"suc")==0) {

            QList<QString> nomiFilesEditati;

            QUtente user = *new QUtente();


            in >> user;

            in >> nomiFilesEditati;


            if(in.commitTransaction()==true){

                SigEsitoLogin( this->opt/*esito*/, user, nomiFilesEditati);
                this->msg="";
            }

            else{
                this->msg="second_read";
                this->opt="opn";

            }

        }

        else {

            QList <QString> nomiFilesEditati;

            QUtente user = *new QUtente();

            SigEsitoLogin(this->opt, user, nomiFilesEditati);
        }
    }
    if (strcmp(this->msg,"e_o")==0)  { //ok

        DocOperation* docOp = new DocOperation();

        SigOpDocRemota(/*esito*/*docOp);
    }

    if (strcmp(this->msg,"e_c")==0)  { //ok

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        SigEsitoChiudiDoc( msg1/*esito*/);
    }


    if (strcmp(this->msg,"mop")==0)  { //mop


        if(strcmp(this->msg,"second_read")==0)  {

            uint prova = 3;

            in.readBytes(this->opt, prova);


        }


        if (strcmp(this->msg,"suc")==0)  {

            QUtente userNew;

            in >> userNew;


            if(in.commitTransaction()==true){

                SigEsitoModificaProfiloUtente(this->opt/*esito*/,  userNew);

                this->msg="";
            }

            else{
                this->msg="second_read";
                this->opt="opn";

            }

        }

        else{

            QUtente user=*new QUtente();

            SigEsitoModificaProfiloUtente(this->opt/*esito*/, user);
        }
    }


    if (strcmp(this->msg,"e_o")==0)  { //ok

        if(strcmp(this->msg,"second_read")==0)  {

            uint prova = 3;
            in.readBytes(this->opt, prova);

        }


        if (strcmp(this->msg,"suc")==0)  {

            DocOperation operazione;

            in >> operazione;


            if(in.commitTransaction()==true){

                SigEsitoOpDocLocale(this->opt,operazione);
                this->msg="";
            }

            else{
                this->msg="second_read";
                this->opt="opn";

            }


        }

        else{

            DocOperation operazione= *new DocOperation();

            SigEsitoOpDocLocale(this->opt,operazione);

        }
    }


    if(strcmp(this->msg,"e_r")==0)  { //ok

        if(strcmp(this->msg,"second_read")==0)  {

            uint prova = 3;
            in.readBytes(this->opt, prova);

        }


        if (strcmp(this->msg,"suc")==0)  {

            QList <QString> nomiFilesEditati;


            SigEsitoRegistrazione(this->opt/*esito*/);

        } else{


            SigEsitoRegistrazione( this->opt/*esito*/);
        }
    }

    if (strcmp(this->msg,"c_i")==0)  { //ok

        QList <QUser> utenti;

        in >> utenti;

        if(in.commitTransaction()==true){

            SigEsitoOpChiHaInseritoCosa(utenti);
            this->msg="";
        }

        else{
            this->msg="second_read";
            this->opt="opn";

        }



    }

    if (strcmp(this->msg,"ucd")==0)  { //ok

        QUser utente;

        in >> utente;


        if(in.commitTransaction()==true){

            SigQuestoUserHaChiusoIlDoc( utente);
            this->msg="";
        }

        else{
            this->msg="second_read";
            this->opt="opn";

        }

    }

    if (strcmp(this->msg,"uad")==0)  { //ok

        QUser utente;

        in >> utente;


        if(in.commitTransaction()==true){

            SigQuestoUserHaApertoIlDoc(utente);
            this->msg="";
        }

        else{
            this->msg="second_read";
            this->opt="opn";

        }
    }


}

void WorkerSocketClient::opDocLocale(DocOperation operazione){

    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("opd",len);

    in <<operazione<< "\n";

}

void WorkerSocketClient::apriDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("ope",len);

    in <<nomeFile<< "\n";

}

void WorkerSocketClient::creaDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("cre",len);

    in <<nomeFile<< "\n";

}

void WorkerSocketClient::login(QUtente user){


    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("log",len);

    in <<user<< "\n";

}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1) {


    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("mod",len);

    in << user1 << "\n";

}

void WorkerSocketClient::registrazione(QUtente user){

    char fullpath[100];
    char name[100];

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("reg",len);

/*
    GetFullPathName(user.getNomeImg(), 100, fullpath, nullptr);

    QImage image(fullpath);

    QBuffer buffer;

    image.save(&buffer, "PNG");

    in << buffer.size()

    in << buffer.data();*/

    qDebug()<< "inizio trasmissione";

    in << user << "\n";

}

void WorkerSocketClient::chiudiDoc(QString nomeFile){


    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("c_d",len);

    in << nomeFile << "\n";


}

void WorkerSocketClient::opChiHaInseritoCosa(){

    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("c_i",len);

}
