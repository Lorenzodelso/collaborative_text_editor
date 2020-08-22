#include "WorkerSocket.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include<string.h>
#include <QDir>

WorkerSocket::WorkerSocket(){

    qRegisterMetaType<CRDT>();
    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QUser>();
    qRegisterMetaType<QList<QString>>();
    qRegisterMetaType<DocOperation>();

    socketConnessoP = new QTcpSocket( this );
    user = *new QUtente();
}

void WorkerSocket::WorkerSocketAttivati(quintptr socketDescriptor){
    socketConnessoP->setSocketDescriptor(socketDescriptor);
    connect(socketConnessoP, &QTcpSocket::readyRead, this, &WorkerSocket::leggiMsgApp);
}

void WorkerSocket::leggiMsgApp() {
    while(socketConnessoP->bytesAvailable()){
        QDataStream in(this->socketConnessoP);
        char* msg;
        uint prova = 3;
        in.readBytes(msg, prova);
        if (strcmp(msg,"ope")==0)
        {
            QString msg1;
            BlockReader(socketConnessoP).stream() >> msg1;
            emit SigApriDoc(msg1, this , this->user);
        }
        if (strcmp(msg,"cre")==0)
        {
            QString msg1;
            BlockReader(socketConnessoP).stream() >> msg1;
            emit SigCreaDoc(msg1, this, this->user);
        }
        if (strcmp(msg,"log")==0)
        {
            QUtente user;
            BlockReader(socketConnessoP).stream() >> user;
             if (user.getUsername() != NULL && user.getPassword() != NULL /*&& user.getNomeImg() == NULL*/)
             {
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
            QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");

               if (user.getUsername() != NULL && user.getPassword() != NULL /*&& e.indexIn(user.getNomeImg()*/)
                   emit  SigRegistrazione(this, user);
        }
        if(strcmp(msg,"opd")==0)
        {
            DocOperation operazione;
            BlockReader(socketConnessoP).stream() >> operazione;
            emit SigOpDoc(operazione);
        }
        if (strcmp(msg,"c_i")==0)
        {
            emit SigOpChiHaInseritoCosa(this,this->user);
        }
        if(strcmp(msg,"mod")==0)
        {
            QUtente userNEW;
            QUtente userOLD=this->user;
            BlockReader(socketConnessoP).stream() >> userNEW;

           /* if(userOLD.getUsername() ==  userNEW.getUsername() &&
               userOLD.getPassword() ==  userNEW.getPassword() &&
               userNEW.getNomeImg()  !=  userOLD.getNomeImg())               // Gestione immagine

                this->image=

            */
            if ((userOLD.getUsername() !=  userNEW.getUsername() &&
                 userOLD.getPassword() ==  userNEW.getPassword() &&
                 userNEW.getNomeImg()  ==  userOLD.getNomeImg()) ||

                (userOLD.getUsername() ==  userNEW.getUsername() &&
                 userOLD.getPassword() !=  userNEW.getPassword() &&
                 userNEW.getNomeImg()  ==  userOLD.getNomeImg()) ||

                (userOLD.getUsername() ==  userNEW.getUsername() &&
                 userOLD.getPassword() ==  userNEW.getPassword() &&
                 userNEW.getNomeImg()  !=  userOLD.getNomeImg()))

                    emit SigModificaProfiloUtente(this, userOLD, userNEW);
        }
        if(strcmp(msg,"c_d")==0)
        {
           emit SigChiusuraDocumentoDaParteDelClient(this, this->user);
        }
        if (strcmp(msg,"c_c")==0)
        {
           emit SigChiusuraConnessioneDaParteDelClient(this, this->user);
        }
    }
   //emit  SigChiusuraConnessioneDaParteDelClient(this, this->user);
}


void WorkerSocket::rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati){

    QDataStream in(socketConnessoP);
    uint len=3;
    in.writeBytes("e_l",len);
    if(nomiFilesEditati.contains("Failed"))
    {
        uint len=3;
        in.writeBytes("fld",len);
        socketConnessoP->disconnectFromHost();
    }
    else{
        uint len=3;
        in.writeBytes("suc",len);
        this->user=user;
        BlockWriter(socketConnessoP).stream() << user;
        BlockWriter(socketConnessoP).stream() << nomiFilesEditati;
    }
}


void WorkerSocket::rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/){
    QDataStream in(socketConnessoP);
    uint len=3;
    in.writeBytes("ead",len);
    if (esito.compare("Success")==0)
    {
        in.writeBytes("ope",len);
        BlockWriter(socketConnessoP).stream() << doc;
    }
    else
    {
        in.writeBytes("fld",len);
    }
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
    else in.writeBytes("fld",len);
}


void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
{
    QDataStream in(socketConnessoP);
    uint len=3;
    in.writeBytes("e_r",len);
    if (esito.compare("OK")==0)
    {
        in.writeBytes("r_a",len);
        //this->image.save(QDir::currentPath+"/"+nomeImg);
        socketConnessoP->disconnectFromHost();
    }
    else
    {
        BlockWriter(socketConnessoP).stream() << esito;
        socketConnessoP->disconnectFromHost();
    }
}


void WorkerSocket::rispondiEsitoOpDoc(QString esito, DocOperation operazione)
{
    QDataStream in(socketConnessoP);
    uint len=3;
    in.writeBytes("e_o",len);
    if (esito.compare("Success")==0)  //operazione esito positivo
    {
        in.writeBytes("suc",len);
        BlockWriter(socketConnessoP).stream() << operazione;
    }
    else // operazione fallita
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
        in.writeBytes("fld",len);
        BlockWriter(socketConnessoP).stream() << userNew;

    }//esito negativo
    else if(userOLD.getUsername() == userNew.getUsername() &&
            userOLD.getPassword() == userNew.getPassword() &&
            userNew.getNomeImg()  != userOLD.getNomeImg()){
         //rename(userOLD.getNomeImg(),  userNew.getNomeImg() )
         //TO DO: Stesso riname di prima
    }
    this->user=userNew;
}


void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito)
{
    QDataStream in(socketConnessoP);
    uint len=3;
    in.writeBytes("e_c",len);
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
