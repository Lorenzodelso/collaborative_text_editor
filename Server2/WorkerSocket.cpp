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
    socketConnessoP->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(socketConnessoP, &QTcpSocket::readyRead, this, &WorkerSocket::leggiMsgApp);
    QObject::connect(socketConnessoP, SIGNAL(error(QAbstractSocket::SocketError)),
    this, SLOT(errorOccurred(QAbstractSocket::SocketError)));
}

void WorkerSocket::errorOccurred(QAbstractSocket::SocketError error){
    qDebug()<<socketConnessoP->error();
}



/*---------------------------------INPUT OPERATION------------------------------*/

void WorkerSocket::EmitSigCreaDoc(QDataStream& in){
    QString msg1;
    in >> msg1;
    if (!in.commitTransaction())
           return;
    emit SigCreaDoc(msg1, this, this->user);
}

void WorkerSocket::EmitSigApriDoc(QDataStream& in){
    QString msg1;
    in >> msg1;
    if (!in.commitTransaction())
           return;
    emit SigApriDoc(msg1, this , this->user);
}

void WorkerSocket::EmitSigLogin(QDataStream& in){
    QUtente user;
    in >> user;
    if (!in.commitTransaction())
           return;
    if (user.getUsername() != NULL && user.getPassword() != NULL /*&& user.getNomeImg() == NULL*/){
        emit SigLogin(this, user);}

}

void WorkerSocket::EmitSigRegistrazione(QDataStream& in){
    QUtente user;
    qint64 dimension;
    QByteArray data;
    in >> user;
    in >> dimension;
    in >> data;

    if(user.getNomeImg() != NULL)
        this->image.loadFromData(data,user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
    QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");
    if(user.getUsername() != NULL && user.getPassword() != NULL && e.indexIn(user.getNomeImg())){
        if (!in.commitTransaction())
               return;
        emit  SigRegistrazione(this, user);}
}

void WorkerSocket::EmitSigColorMode(QDataStream& in){
    if (!in.commitTransaction())
           return;
    emit SigOpChiHaInseritoCosa(this,this->user);
}

void WorkerSocket::EmitSigOperazioneDoc(QDataStream& in){
    DocOperation operazione;
    in >> operazione;
    if (!in.commitTransaction())
           return;
    emit SigOpDoc(operazione);
}

void WorkerSocket::EmitSigModificaProfiloUtente(QDataStream& in){
    QUtente userNEW;
    qint64 dimension;
    QByteArray data;
    in >> userNEW;
    in >> dimension;
    in >> data;
    QUtente userOLD=this->user;
    QString var=QString("NULL");

    if(dimension>0){
         this->temporaryImage.loadFromData(data,userNEW.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
         userNEW.setNomeImg(var+"."+userNEW.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
    }
    if (userOLD.getUsername() != userNEW.getUsername() ||
        userOLD.getPassword() != userNEW.getPassword()||
        userNEW.getNomeImg() != userOLD.getNomeImg()){
        if (!in.commitTransaction())
               return;
            emit SigModificaProfiloUtente(this, userOLD, userNEW);
    }
}

void WorkerSocket::EmitSigChiusuraDocClient(QDataStream& in){
    if (!in.commitTransaction())
           return;
    emit SigChiusuraDocumentoDaParteDelClient(this, this->user);
}

void WorkerSocket::EmitSigChiusuraConnClient(QDataStream& in){
    if (!in.commitTransaction())
           return;
    emit SigChiusuraConnessioneDaParteDelClient(this, this->user);
}

void WorkerSocket::InformaAltriClientLetturaBuffered(QDataStream& in){
    int bufferDim;
    in >> bufferDim;
    if (!in.commitTransaction())
           return;
    emit SigInformaAltriClientLetturaBuffered(bufferDim,user.getUserId());
}

void WorkerSocket::leggiMsgApp() {
        QDataStream in(this->socketConnessoP);
        in.setByteOrder(QDataStream::BigEndian);
        quint16 operationType;
        while(socketConnessoP->bytesAvailable()){
         qDebug()<<socketConnessoP->bytesAvailable();
         in.startTransaction();

        in >> operationType;
        switch (operationType){
        case Apri_doc:{EmitSigApriDoc(in); break;}
        case Crea_doc:{EmitSigCreaDoc(in); break;}
        case Login:{EmitSigLogin(in); break;}
        case Registrazione:{EmitSigRegistrazione(in); break;}
        case Operazione_doc:{EmitSigOperazioneDoc(in); break;}
        case Color_mode: {EmitSigColorMode(in); break;}
        case Modifica_profilo_utente:{EmitSigModificaProfiloUtente(in); break;}
        case Chiusura_doc_client:{EmitSigChiusuraDocClient(in); break;}
        case Chiusura_conn_client:{EmitSigChiusuraConnClient(in); break;}
        case Leggi_buffered:{InformaAltriClientLetturaBuffered(in); break;}
        default:in.commitTransaction();
        }
    }
}


/*-------------------------------------OUTPUT OPERATION---------------------------------*/

void WorkerSocket::AltroClientScriveBuffered(int bufferDim){
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out<<Lettura_buffered;
    out<<bufferDim;
    this->socketConnessoP->write(block);

}


void WorkerSocket::rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati){
    QByteArray block;
    QDataStream out(socketConnessoP);
    out.setByteOrder(QDataStream::BigEndian);

    out<<Esito_login;
    if(nomiFilesEditati.contains("Failed"))
    {
        uint len=3;
        out.writeBytes("fld",len);
        //socketConnessoP->disconnectFromHost();
    }
    else{
        uint len=3;
        out.writeBytes("suc",len);
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

    out<<user;
    out<<static_cast<qint64>(arr.size());
    out<< arr;
    out<< nomiFilesEditati;
    this->socketConnessoP->write(block);

    }
}


void WorkerSocket::rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/){
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    uint len=3;
    out<<Esito_apri_doc;
    if (esito.compare("Success")==0){
        out.writeBytes("ope",len);
    out<<doc;
    }
    else{
        out.writeBytes("fld",len);
    }
    this->socketConnessoP->write(block);

}


void WorkerSocket::rispondiEsitoCreaDoc(QString esito, CRDT doc){
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    uint len=3;
    out<<Esito_crea_doc;
    if (esito.compare("Success")==0){
        out.writeBytes("crt",len);
    out<<doc;
    }
    else out.writeBytes("fld",len);
    this->socketConnessoP->write(block);

}


void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    uint len=3;
    out<<Esito_registrazione;
    QString val= QString("/");
    if (esito.compare("OK")==0)
    {
        out.writeBytes("r_a",len);
        if(nomeImg!=NULL){
            this->image.save(QDir::currentPath()+val+nomeImg, nomeImg.split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        }
    }
    else
    {
    out<<esito;
    this->socketConnessoP->write(block);
    socketConnessoP->disconnectFromHost();
    }
}


void WorkerSocket::rispondiEsitoOpDoc(QString esito, DocOperation operazione)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    uint len=3;
    out<<Esito_operazione_doc;
    if (esito.compare("Success")==0) {
        out.writeBytes("suc",len);
    out<<operazione;
    this->socketConnessoP->write(block);
    }
    else{
        out.writeBytes("fld",len);
    out<<operazione;
    this->socketConnessoP->write(block);
    }
}


void WorkerSocket::rispondiEsitoModificaProfiloUtente(QUtente userNew,bool immagineModificata)
{
    QUtente userOLD = this->user;
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    uint len=3;
    QByteArray arr;
    QBuffer buffer(&arr);
    QString val= QString("/");
    out<<Esito_modifica_profilo_utente;
    this->user=userNew;

    if(userOLD.getUsername() == userNew.getUsername() &&
        userOLD.getPassword() == userNew.getPassword() &&
        immagineModificata==false){
            out.writeBytes("fld",len);
    }
    else{
        out.writeBytes("suc",len);
        if(immagineModificata){
            this->temporaryImage.save(QDir::currentPath()+val+userNew.getNomeImg(), userNew.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
            this->image=this->temporaryImage;
            buffer.open(QIODevice::WriteOnly);
            this->image.save(&buffer, userNew.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        }


    out<<this->user;
    out<<static_cast<qint64>(arr.size());
    out<<arr;
    this->socketConnessoP->write(block);
    }
}

void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out<<Esito_chiusura_doc_client;
    out<<esito;
    this->socketConnessoP->write(block);
}


void WorkerSocket::questoUserHaApertoIlDoc(QUser user)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out<<User_apri_doc;
    out<<user;
    this->socketConnessoP->write(block);
}


void WorkerSocket::questoUserHaChiusoIlDoc(QUser user)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out<<User_chiudi_doc;
    out<<user;
    this->socketConnessoP->write(block);
}


void WorkerSocket::rispondiChiHaInseritoCosa(QList<QUser> users)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out<<Esito_operazione_colorMode;
    out<<users;
    this->socketConnessoP->write(block);
}
