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
          //qDebug() << "Debug Message";    // CAN BE REMOVED AT COMPILE TIME!
          //std::cout << "ok";
            BlockReader(socketConnessoP).stream() >> user;
            BlockReader(socketConnessoP).stream() >> dimension;
            BlockReader(socketConnessoP).stream() >> data;

            if(user.getNomeImg() != NULL)
                  this->image.loadFromData(data,user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());

            QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");
            if(user.getUsername() != NULL && user.getPassword() != NULL && e.indexIn(user.getNomeImg()))
            {
               emit  SigRegistrazione(this, user);
            }
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
            qint64 dimension;
            QByteArray data;

            BlockReader(socketConnessoP).stream() >> userNEW;
            BlockReader(socketConnessoP).stream() >> dimension;
            BlockReader(socketConnessoP).stream() >> data;

            QUtente userOLD=this->user;
            QString var=QString("NULL");

            if(dimension>0){

                     this->temporaryImage.loadFromData(data,userNEW.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
                     userNEW.setNomeImg(var+userNEW.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
            }
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

        QByteArray arr;
        QBuffer buffer(&arr);
        QImage* image=new QImage();
        QString val=QString("/");

        this->user=user;

        if(user.getNomeImg()!=NULL)
        {
            image->load(QDir::currentPath()+val+user.getNomeImg());
            qDebug()<<QDir::currentPath();
            buffer.open(QIODevice::WriteOnly);
            image->save(&buffer, user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
         }

        BlockWriter(socketConnessoP).stream() << user;
        BlockWriter(socketConnessoP).stream() << static_cast<qint64>(arr.size());
        BlockWriter(socketConnessoP).stream() << arr;

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
    QString val= QString("/");
    if (esito.compare("OK")==0)
    {
        in.writeBytes("r_a",len);
        if(nomeImg!=NULL){
            this->image.save(QDir::currentPath()+val+nomeImg);
        }
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


void WorkerSocket::rispondiEsitoModificaProfiloUtente(QUtente userNew,bool immagineModificata)
{
    QUtente userOLD = this->user;
    QDataStream in(socketConnessoP);
    uint len=3;
    QByteArray arr;
    QBuffer buffer(&arr);
    QString val= QString("/");
    in.writeBytes("mop",len);
    this->user=userNew;

    if(userOLD.getUsername() == userNew.getUsername() &&
        userOLD.getPassword() == userNew.getPassword() &&
        immagineModificata==false){

        in.writeBytes("fld",len);

    }

    else{

        in.writeBytes("suc",len);

        if(immagineModificata){

            this->temporaryImage.save(QDir::currentPath()+val+userNew.getNomeImg());
            this->image=this->temporaryImage;

        }


        buffer.open(QIODevice::WriteOnly);
        this->image.save(&buffer, userNew.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());


        BlockWriter(socketConnessoP).stream() << this->user;
        BlockWriter(socketConnessoP).stream() << static_cast<qint64>(arr.size());
        BlockWriter(socketConnessoP).stream() << arr;


    }
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
