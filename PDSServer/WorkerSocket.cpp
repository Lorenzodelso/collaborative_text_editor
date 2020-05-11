#include "WorkerSocket.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include<string.h>
#include <QDir>

WorkerSocket::WorkerSocket(){

    qRegisterMetaType<CRDT>();
    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QList<QString>>();
    qRegisterMetaType<DocOperation>();



    socketConnessoP = new QTcpSocket( this );
    user = *new QUtente();
}

void WorkerSocket::WorkerSocketAttivati(quintptr socketDescriptor){

    //DEBUG
    std::cout << "Ricevuto segnale workerSocketAttivati"<<std::flush;
    //DEBUG

    socketConnessoP->setSocketDescriptor(socketDescriptor);

    connect(socketConnessoP, &QTcpSocket::readyRead, this, &WorkerSocket::leggiMsgApp);
}

void WorkerSocket::leggiMsgApp() {

    qDebug()<<"inizio lettura";

    QDataStream in(this->socketConnessoP);

    in.startTransaction();

    uint prova = 3;
    in.readBytes(this->msg, prova);

    qDebug()<<this->msg;


    //DEBUG
    std::cout<<"\nRicevuto segnale readyRead - Slot leggiMsgApp"<<std::flush;
   // std::cout<<"\n"+msg<<std::flush;
    //DEBUG

    if (strcmp(this->msg,"ope")==0)
    {

        QString msg1;
        in>>msg1;

        qDebug()<<msg1;

        emit SigApriDoc(msg1, this , this->user);
    }

    if (strcmp(this->msg,"cre")==0)
    {

        std::cout<<"Entro nel caso crea nuovo documento\n"<<std::flush;

        QString msg1;
        in >> msg1;

       std::cout<<"Nome del file ricevuto: "<<msg1.toStdString()<<"\n"<<std::flush;

        emit SigCreaDoc(msg1, this, this->user);

    }

    if (strcmp(this->msg,"log")==0)
    {
        QUtente user;

        in >> user ;

        if(in.commitTransaction()==true)
        {
            if (user.getUsername() != NULL && user.getPassword() != NULL && user.getNomeImg() == NULL)
            {
                qDebug()<<"Segnale Log emesso\n";
                emit SigLogin(this, user);
            }

        }

        else return;

    }

    if (strcmp(msg,"reg")==0)
    {

        QUtente user;
        qint64 dimension;
        QByteArray data;

       // in >> dimension;

        //in.readBytes(data,dimension);
        // img = QImage::fromData(data,"PNG");

       // this->image=img;

        in >> user;

        //DEBUG
        std::cout<<"\nUser ricevuto: "+user.getUsername().toStdString()<<std::flush;
        //DEBUG
        QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");

        if(in.commitTransaction()==true)
        {

           if (user.getUsername() != NULL && user.getPassword() != NULL /*&& e.indexIn(user.getNomeImg()*/)

                emit SigRegistrazione(this, user);
        }

        else return;
    }


    if(strcmp(msg,"opd")==0)
    {
        DocOperation operazione;

        in >> operazione;

        if(in.commitTransaction()==true)

                emit SigOpDoc(operazione);

        else return;
    }

    if (strcmp(msg,"c_i")==0)
    {
        emit SigOpChiHaInseritoCosa(this,this->user);
    }

    if(strcmp(msg,"mod")==0) //ok
    {

        QUtente userOLD,userNEW;

        in >> userOLD;
        in >> userNEW;

        if(in.commitTransaction()==true)
        {

            if ( userOLD.getUsername() !=  userNEW.getUsername() &&
                 userOLD.getPassword() !=  userNEW.getPassword() &&
                 userNEW.getNomeImg()  ==  userOLD.getNomeImg())

                   emit SigModificaProfiloUtente(this, userOLD, userNEW);
        }

        else return;
    }

    if(strcmp(this->msg,"c_d")==0)
    {
        SigChiusuraDocumentoDaParteDelClient(this, this->user);
    }

    if (strcmp(this->msg,"c_c")==0)
    {
        SigChiusuraConnessioneDaParteDelClient(this, this->user);
    }

   // SigChiusuraConnessioneDaParteDelClient(this, this->user);

}


void WorkerSocket::rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati){

    QDataStream in(socketConnessoP);

    qDebug()<<"esitologin:\n";

    uint len=3;

    in.writeBytes("e_l",len);

    if(nomiFilesEditati.contains("Failed"))
    {
        uint len=3;

        this->user=user;

        qDebug()<<"fallito\n";

        in.writeBytes("fld",len);

        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }

    else{

        uint len=3;

        in.writeBytes("suc",len);

        this->user = user;

        in << user << nomiFilesEditati;

    }
}


void WorkerSocket::rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/){

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("ead",len);

    qDebug()<<esito;

    if (esito.compare("Success")==0){

        in.writeBytes("ope",len);

        in << doc;
    }

    else{

        in.writeBytes("fld",len);
    }
}


void WorkerSocket::rispondiEsitoCreaDoc(QString esito, CRDT doc/*rappresentazione del file*/){


    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("ecd",len);

    if (esito.compare("Success")==0)
    {

        in.writeBytes("crt",len);

        in << doc;
    }
    else

        in.writeBytes("fld",len);

}


void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
{
    QDataStream in(socketConnessoP);

    qDebug()<<"esito registrazione\n";

    uint len=3;

    in.writeBytes("e_r",len);

    if (esito.compare("OK")==0)
    {

        uint len=3;

        in.writeBytes("r_a",len);

        //this->image.save(QDir::currentPath+"/"+nomeImg);
        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }

    else
    {

        in << esito;

        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }
}


void WorkerSocket::rispondiEsitoOpDoc(QString esito, DocOperation operazione)
{

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("e_o",len);

    if (esito.compare("Success")==0)
    {

        uint len=3;

        in.writeBytes("suc",len);

        if(operazione.getSiteId() == this->user.getUserId()) {

            in << esito;
        }

        else{

            in << esito << operazione;
        }
    }

    else if(user.getUserId() == this->user.getUserId())
    {

        uint len=3;

        in.writeBytes("fld",len);
    }
}


void WorkerSocket::rispondiEsitoModificaProfiloUtente(QUtente userNew)
{

    QUtente userOLD = this->user;

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("mop",len);

    if((userOLD.getUsername() == userNew.getUsername() &&

        userOLD.getPassword() == userNew.getPassword() &&

        userNew.getNomeImg()  == userOLD.getNomeImg())){


        uint len=3;

        in.writeBytes("suc",len);
        in<< userNew;

    }//esito negativo

    else if( userNew.getNomeImg() != userOLD.getNomeImg()){

        //rename(userOLD.getNomeImg(),  userNew.getNomeImg() )
        //TO DO: Stesso riname di prima
    }

    this->user=userNew;
}


void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito/*esito*/){

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("c_d",len);
    in<<esito;
}


void WorkerSocket::questoUserHaApertoIlDoc(QUser user) {

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("uad",len);
    in << user;
}


void WorkerSocket::questoUserHaChiusoIlDoc(QUser user) {

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("ucd",len);
    in << user;
}


void WorkerSocket::rispondiChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/){

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("c_i",len);
    in << users;
}
