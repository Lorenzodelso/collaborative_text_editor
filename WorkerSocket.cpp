#include "WorkerSocket.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include<string.h>
#include <QDir>

WorkerSocket::WorkerSocket(){

    qRegisterMetaType<CRDT>();
    qRegisterMetaType<QList<QUser>>();
    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QUser>();
    qRegisterMetaType<QList<QString>>();
    qRegisterMetaType<DocOperation>();
    qRegisterMetaType<QMap<QUser,WorkerSocket*>>();

    socketConnessoP = new QTcpSocket(this);
}

WorkerSocket::~WorkerSocket(){
    delete socketConnessoP;
}

void WorkerSocket::WorkerSocketAttivati(quintptr socketDescriptor){
    socketConnessoP->setSocketDescriptor(socketDescriptor);
    connect(socketConnessoP, &QTcpSocket::readyRead, this, &WorkerSocket::leggiMsgApp);
}


/*---------------------------------INPUT OPERATION------------------------------*/

void WorkerSocket::EmitSigCreaDoc(){
    QString msg1;
    BlockReader(socketConnessoP).stream() >> msg1;
    emit SigCreaDoc(msg1, this, this->user);
}

void WorkerSocket::EmitSigApriDoc(){
    QString msg1;
    BlockReader(socketConnessoP).stream() >> msg1;
    emit SigApriDoc(msg1, this , this->user);
}

void WorkerSocket::EmitSigLogin(){
    //TENTATIVO DI LOGIN
    //
    //
    //
    qDebug()<< "Tentativo di login";
    //
    //
    //
    //******************************
    QUtente user;
    BlockReader(socketConnessoP).stream() >> user;
    if (user.getUsername() != NULL && user.getPassword() != NULL /*&& user.getNomeImg() == NULL*/)
        emit SigLogin(this, user);

}

void WorkerSocket::EmitSigRegistrazione(){
    QUtente user;
    qint64 dimension;
    QByteArray data;
    BlockReader(socketConnessoP).stream() >> user;
    BlockReader(socketConnessoP).stream() >> dimension;
    BlockReader(socketConnessoP).stream() >> data;

    if(user.getNomeImg() != NULL)
        this->image.loadFromData(data,user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
    QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");
    if(user.getUsername() != NULL && user.getPassword() != NULL && e.indexIn(user.getNomeImg()))
        emit  SigRegistrazione(this, user);
}

void WorkerSocket::EmitSigColorMode(){
    emit SigOpChiHaInseritoCosa(this,this->user);
}

void WorkerSocket::EmitSigOperazioneDoc(){
    DocOperation operazione;
    BlockReader(socketConnessoP).stream() >> operazione;
    emit SigOpDoc(operazione);
}

void WorkerSocket::EmitSigModificaProfiloUtente(){
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
         userNEW.setNomeImg(var+"."+userNEW.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
    }
    if (userOLD.getUsername() != userNEW.getUsername() ||
        userOLD.getPassword() != userNEW.getPassword()||
        userNEW.getNomeImg() != userOLD.getNomeImg()){
            emit SigModificaProfiloUtente(this, userOLD, userNEW);
    }
}

void WorkerSocket::EmitSigChiusuraDocClient(){
    emit SigChiusuraDocumentoDaParteDelClient(this, this->user);
}

void WorkerSocket::EmitSigChiusuraConnClient(){
    emit SigChiusuraConnessioneDaParteDelClient(this, this->user);
}

void WorkerSocket::leggiMsgApp() {
    while(socketConnessoP->bytesAvailable()){
        QDataStream in(this->socketConnessoP);
        quint16 operationType;
        in >> operationType;
        switch (operationType){
        case Apri_doc:{EmitSigApriDoc(); break;}
        case Crea_doc:{EmitSigCreaDoc(); break;}
        case Login:{EmitSigLogin(); break;}
        case Registrazione:{EmitSigRegistrazione(); break;}
        case Operazione_doc:{EmitSigOperazioneDoc(); break;}
        case Color_mode: {EmitSigColorMode(); break;}
        case Modifica_profilo_utente:{EmitSigModificaProfiloUtente(); break;}
        case Chiusura_doc_client:{EmitSigChiusuraDocClient(); break;}
        case Chiusura_conn_client:{EmitSigChiusuraConnClient(); break;}
        }
    }
}


/*-------------------------------------OUTPUT OPERATION---------------------------------*/

void WorkerSocket::rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati){

    QDataStream in(socketConnessoP);
    in << Esito_login;
    if(nomiFilesEditati.contains("Failed"))
    {
        uint len=3;
        in.writeBytes("fld",len);
        //socketConnessoP->disconnectFromHost();
    }
    else{
        uint len=3;
        in.writeBytes("suc",len);
        QByteArray arr;
        QBuffer buffer(&arr);
        QImage image;
        QString val=QString("/");
        this->user=user;
        if(user.getNomeImg()!=NULL)
        {
            image.load(QDir::currentPath()+val+user.getNomeImg());
            qDebug()<<QDir::currentPath();
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
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
    in << Esito_apri_doc;
    if (esito.compare("Success")==0){
        in.writeBytes("ope",len);
        BlockWriter(socketConnessoP).stream() << doc;
    }
    else{
        in.writeBytes("fld",len);
    }
}


void WorkerSocket::rispondiEsitoCreaDoc(QString esito, CRDT doc){
    QDataStream in(socketConnessoP);
    uint len=3;
    in << Esito_crea_doc;
    if (esito.compare("Success")==0){
        in.writeBytes("crt",len);
        BlockWriter(socketConnessoP).stream() << doc;
    }
    else in.writeBytes("fld",len);
}


void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
{
    QDataStream in(socketConnessoP);
    uint len=3;
    in << Esito_registrazione;
    QString val= QString("/");
    if (esito.compare("OK")==0)
    {
        in.writeBytes("r_a",len);
        if(nomeImg!=NULL){
            this->image.save(QDir::currentPath()+val+nomeImg, nomeImg.split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        }
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
    in << Esito_operazione_doc;
    if (esito.compare("Success")==0) {
        in.writeBytes("suc",len);
        BlockWriter(socketConnessoP).stream() << operazione;
    }
    else{
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
    in << Esito_modifica_profilo_utente;
    this->user=userNew;

    if(userOLD.getUsername() == userNew.getUsername() &&
        userOLD.getPassword() == userNew.getPassword() &&
        immagineModificata==false){
            in.writeBytes("fld",len);
    }
    else{
        in.writeBytes("suc",len);
        if(immagineModificata){
            this->temporaryImage.save(QDir::currentPath()+val+userNew.getNomeImg(), userNew.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
            this->image=this->temporaryImage;
            buffer.open(QIODevice::WriteOnly);
            this->image.save(&buffer, userNew.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        }


        BlockWriter(socketConnessoP).stream() << this->user;
        BlockWriter(socketConnessoP).stream() << static_cast<qint64>(arr.size());
        BlockWriter(socketConnessoP).stream() << arr;
    }
}

void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito)
{
    QDataStream in(socketConnessoP);
    in << Esito_chiusura_doc_client;
    BlockWriter(socketConnessoP).stream() << esito;
}


void WorkerSocket::questoUserHaApertoIlDoc(QUser user)
{
    QDataStream in(socketConnessoP);
    in << User_apri_doc;
    BlockWriter(socketConnessoP).stream() << user;
}


void WorkerSocket::questoUserHaChiusoIlDoc(QUser user)
{
    QDataStream in(socketConnessoP);
    in << User_chiudi_doc;
    BlockWriter(socketConnessoP).stream() << user;
}


void WorkerSocket::rispondiChiHaInseritoCosa(QList<QUser> users)
{
    QDataStream in(socketConnessoP);
    in << Esito_operazione_colorMode;
    BlockWriter(socketConnessoP).stream() << users;
}