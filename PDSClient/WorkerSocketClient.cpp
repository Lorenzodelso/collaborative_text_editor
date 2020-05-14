

#include "WorkerSocketClient.h"
#include  <QTcpSocket>
#include <QHostAddress>
#include<QThread>
#include <QImage>
#include <QDebug>




void WorkerSocketClient::connessioneAlServer() {

    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QList<QString>>();
    qRegisterMetaType<CRDT>();
    qRegisterMetaType<DocOperation>();

    this->socketConnesso = new QTcpSocket( this );

    this->socketConnesso->connectToHost(QHostAddress::LocalHost, 3030 );

    bool connected = (socketConnesso->state() == QTcpSocket::ConnectedState);

    if(connected==true)

        SigEsitoConnessioneAlServer("Success");

    else

        SigEsitoConnessioneAlServer("Failed");


    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp,Qt::DirectConnection);


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

    char* msg;

    char* opt;

    QDataStream in(this->socketConnesso);

    in.startTransaction();

    uint prova = 3;
    in.readBytes(msg, prova);

    if (strcmp(msg,"ead")==0)
    {
        in.readBytes(opt, prova);

        if (strcmp(opt,"ope")==0)
        {
            CRDT doc;

            in >> doc;

            if(in.commitTransaction()==true)

                SigEsitoApriDoc("Success"/*esito*/, doc/*rappresentazione del file*/);

             else return;
         }

        else
        {
            CRDT doc=*new CRDT();

            SigEsitoApriDoc(opt, doc);
        }
    }

    if (strcmp(msg,"ecd")==0){

        in.readBytes(opt,prova);

        if (strcmp(opt,"crt")==0)
        {
            CRDT doc;

            in >> doc;

            if(in.commitTransaction()==true)

                SigEsitoCreaDoc( "Success"/*esito*/,  doc/*rappresentazione del file*/);

            else return;

       }
       else
       {
            CRDT doc =*new CRDT(0);

            SigEsitoCreaDoc(opt, doc);

    }}

    if(strcmp(msg,"e_l")==0) {

        in.readBytes(opt, prova);

        if(strcmp(opt,"suc")==0)
        {

            QList<QString> nomiFilesEditati;

            QUtente user = *new QUtente();

            in >> user;

            in >> nomiFilesEditati;

            if(in.commitTransaction()==true)

                SigEsitoLogin( opt/*esito*/, user, nomiFilesEditati);

            else return;

        }

        else
        {
            QList <QString> nomiFilesEditati;

            QUtente user = *new QUtente();

            SigEsitoLogin(opt, user, nomiFilesEditati);
        }
    }

    if (strcmp(msg,"e_o")==0)
    {
        DocOperation* docOp = new DocOperation();

        SigOpDocRemota(/*esito*/*docOp);
    }

    if (strcmp(msg,"e_c")==0)
    {
        in.readBytes(opt,prova);

        SigEsitoChiudiDoc( opt/*esito*/);
    }


    if (strcmp(msg,"mop")==0)  {

        in.readBytes(opt, prova);

        if (strcmp(opt,"suc")==0)
        {
            QUtente userNew;

            in >> userNew;

            if(in.commitTransaction()==true)

                SigEsitoModificaProfiloUtente("Success"/*esito*/,userNew);

            else return;

        }

        else{

            QUtente user=*new QUtente();

            SigEsitoModificaProfiloUtente("Failed"/*esito*/, user);
        }
    }


    if (strcmp(msg,"e_o")==0)  { //ok

        in.readBytes(opt, prova);

        if (strcmp(opt,"suc")==0)
        {

            DocOperation operazione;

            in >> operazione;
            std::cout<<"Esito operazione dal server: "<<operazione.character.getValue().toLatin1()<<std::flush;

            if(in.commitTransaction()==true)

                SigEsitoOpDocLocale("Success",operazione);

            else return;
        }

        else
        {

            DocOperation operazione= *new DocOperation();

            SigEsitoOpDocLocale("Failed",operazione);

        }
    }


    if(strcmp(msg,"e_r")==0)
    {
        in.readBytes(opt, prova);
        if (strcmp(opt,"r_a")==0)
        {
            SigEsitoRegistrazione("Success"/*esito*/);
        }

        else

         SigEsitoRegistrazione("Failed"/*esito*/);


    }

    if (strcmp(msg,"c_i")==0)
    {

        QList <QUser> utenti;

        in >> utenti;

        if(in.commitTransaction()==true)

            SigEsitoOpChiHaInseritoCosa(utenti);   

        else return;

    }

    if (strcmp(msg,"ucd")==0)  { //ok

        QUser utente;

        in >> utente;

        if(in.commitTransaction()==true)

            SigQuestoUserHaChiusoIlDoc(utente);

        else return;
    }

    if (strcmp(msg,"uad")==0)  { //ok

        QUser utente;

        in >> utente;

        if(in.commitTransaction()==true)

            SigQuestoUserHaApertoIlDoc(utente);

        else return;
    }
}

void WorkerSocketClient::opDocLocale(DocOperation operazione){

    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("opd",len);

    in <<operazione;

}

void WorkerSocketClient::apriDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("ope",len);

    in <<nomeFile;

}

void WorkerSocketClient::creaDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("cre",len);

    in << nomeFile;

}

void WorkerSocketClient::login(QUtente user){


    QDataStream in(this->socketConnesso);


    uint len=3;

    in.writeBytes("log",len);

    in <<user;

}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1) {


    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("mod",len);

    in << user1;

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

    in << user;

}

void WorkerSocketClient::chiudiDoc(QString nomeFile){

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("c_d",len);

  //  in << nomeFile;


}

void WorkerSocketClient::opChiHaInseritoCosa(){

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("c_i",len);

}
