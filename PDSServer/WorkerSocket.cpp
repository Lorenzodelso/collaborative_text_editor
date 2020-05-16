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
        BlockReader(socketConnessoP).stream() >> msg1;

        qDebug()<<msg1;

        emit SigApriDoc(msg1, this , this->user);
    }

    if (strcmp(this->msg,"cre")==0)
    {
        std::cout<<"Entro nel caso crea nuovo documento\n"<<std::flush;

        QString msg1;
        BlockReader(socketConnessoP).stream() >> msg1;

        std::cout<<"Nome del file ricevuto: "<<msg1.toStdString()<<"\n"<<std::flush;

        emit SigCreaDoc(msg1, this, this->user);
    }

    if (strcmp(this->msg,"log")==0)
    {
        QUtente user;

        BlockReader(socketConnessoP).stream() >> user;

         if (user.getUsername() != NULL && user.getPassword() != NULL && user.getNomeImg() == NULL)
         {
                qDebug()<<"Segnale Log emesso\n";
                emit SigLogin(this, user);
         }
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

        BlockReader(socketConnessoP).stream() >> user;

        //DEBUG
        std::cout<<"\nUser ricevuto: "+user.getUsername().toStdString()<<std::flush;
        //DEBUG
        QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");

           if (user.getUsername() != NULL && user.getPassword() != NULL /*&& e.indexIn(user.getNomeImg()*/)

               emit  SigRegistrazione(this, user);
    }


    if(strcmp(msg,"opd")==0)
    {
        DocOperation operazione;
        BlockReader(socketConnessoP).stream() >> operazione;

        qDebug()<<"carattere ricevuto lato server: "<<operazione.character.getValue()<<"Con site id :"<<operazione.getSiteId()<<"\n";

        emit  SigOpDoc(operazione);
    }

    if (strcmp(msg,"c_i")==0)
    {
        emit SigOpChiHaInseritoCosa(this,this->user);
    }

    if(strcmp(msg,"mod")==0)
    {
        QUtente userOLD,userNEW;

        BlockReader(socketConnessoP).stream() >> userOLD;
        BlockReader(socketConnessoP).stream() >> userNEW;

        if ( userOLD.getUsername() !=  userNEW.getUsername() &&
             userOLD.getPassword() !=  userNEW.getPassword() &&
             userNEW.getNomeImg()  ==  userOLD.getNomeImg())

                emit  SigModificaProfiloUtente(this, userOLD, userNEW);
    }

    if(strcmp(this->msg,"c_d")==0)
    {
       emit SigChiusuraDocumentoDaParteDelClient(this, this->user);
    }

    if (strcmp(this->msg,"c_c")==0)
    {
       emit SigChiusuraConnessioneDaParteDelClient(this, this->user);
    }

   //emit  SigChiusuraConnessioneDaParteDelClient(this, this->user);

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
        
        BlockWriter(socketConnessoP).stream() << user;

        BlockWriter(socketConnessoP).stream() << nomiFilesEditati;
    }
}


void WorkerSocket::rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/){

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("ead",len);

    qDebug()<<esito;

    if (esito.compare("Success")==0)
    {
        in.writeBytes("ope",len);

        BlockWriter(socketConnessoP).stream() << doc;
    }

    else in.writeBytes("fld",len);
}


void WorkerSocket::rispondiEsitoCreaDoc(QString esito, CRDT doc){

    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("ecd",len);

    if (esito.compare("Success")==0)
    {
        in.writeBytes("crt",len);

        BlockWriter(socketConnessoP).stream() << doc;
    }

    else  in.writeBytes("fld",len);
}


void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
{
    QDataStream in(socketConnessoP);

    qDebug()<<"esito registrazione\n";

    uint len=3;

    in.writeBytes("e_r",len);

    if (esito.compare("OK")==0)
    {
        in.writeBytes("r_a",len);

        //this->image.save(QDir::currentPath+"/"+nomeImg);
        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }

    else
    {
        BlockWriter(socketConnessoP).stream() << esito;

        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }
}


void WorkerSocket::rispondiEsitoOpDoc(QString esito, DocOperation operazione)
{
    QDataStream in(socketConnessoP);

    qDebug()<<"rispondiesitoopdoc";

    uint len=3;

    in.writeBytes("e_o",len);

    if (esito.compare("Success")==0)
    {
        in.writeBytes("suc",len);

       // if(operazione.getSiteId() == this->user.getUserId()) {

       // BlockWriter(socketConnessoP).stream() << esito;

        BlockWriter(socketConnessoP).stream() << operazione;

      //  }

      //  else{

         //   in << esito << operazione;
       // }
    }
    else //if(user.getUserId() == this->user.getUserId())
    {
        in.writeBytes("fld",len);

        BlockWriter(socketConnessoP).stream() << operazione;
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

        in.writeBytes("suc",len);

        BlockWriter(socketConnessoP).stream() << userNew;

    }//esito negativo

    else if( userNew.getNomeImg() != userOLD.getNomeImg()){

        //rename(userOLD.getNomeImg(),  userNew.getNomeImg() )
        //TO DO: Stesso riname di prima
    }

    this->user=userNew;
}


void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito)
{
    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("c_d",len);

    BlockWriter(socketConnessoP).stream() << esito;
}


void WorkerSocket::questoUserHaApertoIlDoc(QUser user)
{
    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("uad",len);

    BlockWriter(socketConnessoP).stream() << user;
}


void WorkerSocket::questoUserHaChiusoIlDoc(QUser user)
{
    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("ucd",len);

    BlockWriter(socketConnessoP).stream() << user;
}


void WorkerSocket::rispondiChiHaInseritoCosa(QList<QUser> users)
{
    QDataStream in(socketConnessoP);

    uint len=3;

    in.writeBytes("c_i",len);

    BlockWriter(socketConnessoP).stream() << users;
}
