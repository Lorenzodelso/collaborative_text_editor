

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

      emit SigEsitoConnessioneAlServer("Success");

    else

      emit SigEsitoConnessioneAlServer("Failed");


    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp,Qt::QueuedConnection);


}


void WorkerSocketClient::disconnessioneDalServer(){

    this->socketConnesso->disconnect();
    this->socketConnesso->disconnectFromHost();

    bool connected = (this->socketConnesso->state() == QTcpSocket::ConnectedState);

    if(connected==false)

        disconnessioneDalServer();

    else

     emit  SigEsitoConnessioneAlServer("Failed");

}

void  WorkerSocketClient::leggiMsgApp(){

    qDebug()<<"inizio lettura";

    char* msg;

    char* opt;

    QDataStream in(this->socketConnesso);

    uint prova = 3;
    in.readBytes(msg, prova);

    if (strcmp(msg,"ead")==0)
    {
        in.readBytes(opt, prova);

        if (strcmp(opt,"ope")==0)
        {
            CRDT doc;

            in >> doc;

            emit SigEsitoApriDoc("Success"/*esito*/, doc/*rappresentazione del file*/);
         }

        else
        {
            CRDT doc=*new CRDT();

            emit  SigEsitoApriDoc(opt, doc);
        }
    }

    if (strcmp(msg,"ecd")==0){

        in.readBytes(opt,prova);

        if (strcmp(opt,"crt")==0)
        {
            CRDT doc;

            in >> doc;

            emit  SigEsitoCreaDoc( "Success"/*esito*/,  doc/*rappresentazione del file*/);
       }
       else
       {
            CRDT doc =*new CRDT();

          emit SigEsitoCreaDoc(opt, doc);

       }
    }

    if(strcmp(msg,"e_l")==0) {

        in.readBytes(opt, prova);

        if(strcmp(opt,"suc")==0)
        {

            QList<QString> nomiFilesEditati;

            QUtente user = *new QUtente();

            in >> user;

            in >> nomiFilesEditati;

            emit  SigEsitoLogin( opt/*esito*/, user, nomiFilesEditati);
        }

        else
        {
            QList <QString> nomiFilesEditati;

            QUtente user = *new QUtente();

            emit  SigEsitoLogin(opt, user, nomiFilesEditati);
        }
    }

    if (strcmp(msg,"eop")==0)
    {
        DocOperation* docOp = new DocOperation();

        emit  SigOpDocRemota(/*esito*/*docOp);
    }

    if (strcmp(msg,"e_c")==0)
    {
        in.readBytes(opt,prova);

        emit  SigEsitoChiudiDoc( opt/*esito*/);
    }


    if (strcmp(msg,"mop")==0)  {

        in.readBytes(opt, prova);

        if (strcmp(opt,"suc")==0)
        {
            QUtente userNew;

            in >> userNew;

            emit  SigEsitoModificaProfiloUtente("Success"/*esito*/,userNew);
        }

        else{

            QUtente user=*new QUtente();

            emit SigEsitoModificaProfiloUtente("Failed"/*esito*/, user);
        }
    }


    if (strcmp(msg,"e_o")==0)  { //ok

        in.readBytes(opt, prova);

        DocOperation operazione;

        qDebug()<<"esito_operazione: "<<opt<<"\n";
        if (strcmp(opt,"suc")==0)
        {

            BlockReader(socketConnesso).stream() >> operazione;
            qDebug()<<"carattere ricevuto lato client: "<<operazione.character.getValue()<<"Con site id :"<<operazione.getSiteId()<<"\n";

            std::cout<<"Esito operazione dal server: "<<operazione.character.getValue().toLatin1()<<std::flush;

            emit SigEsitoOpDocLocale("Success",operazione);
        }

        else
        {
            BlockReader(socketConnesso).stream() >> operazione;
            qDebug()<<"carattere ricevuto lato client: "<<operazione.character.getValue()<<"Con site id :"<<operazione.getSiteId()<<"\n";

            emit SigEsitoOpDocLocale("Failed",operazione);
        }
    }


    if(strcmp(msg,"e_r")==0)
    {
        in.readBytes(opt, prova);

        if (strcmp(opt,"r_a")==0)

          emit SigEsitoRegistrazione("Success"/*esito*/);

        else

         emit SigEsitoRegistrazione("Failed"/*esito*/);
    }

    if (strcmp(msg,"c_i")==0)
    {
        QList <QUser> utenti;

        in >> utenti;

        emit SigEsitoOpChiHaInseritoCosa(utenti);
    }

    if (strcmp(msg,"ucd")==0)  { //ok

        QUser utente;

        in >> utente;

        emit  SigQuestoUserHaChiusoIlDoc(utente);
    }

    if (strcmp(msg,"uad")==0)  { //ok

        QUser utente;

        in >> utente;

        emit  SigQuestoUserHaApertoIlDoc(utente);
    }
}

void WorkerSocketClient::opDocLocale(DocOperation operazione)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("opd",len);

    BlockWriter(socketConnesso).stream()<< operazione;
}

void WorkerSocketClient::apriDoc(QString nomeFile)
{

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("ope",len);

    in <<nomeFile;
}

void WorkerSocketClient::creaDoc(QString nomeFile)
{

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

void WorkerSocketClient::chiudiDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("c_d",len);
}

void WorkerSocketClient::opChiHaInseritoCosa(){

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("c_i",len);
}
