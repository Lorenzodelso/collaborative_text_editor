//
// Created by Edoardo Dazzara on 16/04/2020.
//

#include "QUtenteServer.h"

#include "QUtente.h"
QUtenteServer::QUtenteServer(quint32 userId, QString username, QString password, QString salt, QString imgName){
    this->userId = userId;
    this->userName = username;
    this->salt = salt;
    this->password = password;
    this->nomeImg = imgName;
}

QUtenteServer::QUtenteServer(){

}

QUtenteServer::QUtenteServer(QUtenteServer const &utente){
    this->userId = utente.userId;
    this->userName = utente.userName;
    this->salt = utente.salt;
    this->password = utente.password;
    this->nomeImg = utente.nomeImg;
}

QUtenteServer QUtenteServer::operator=(const QUtenteServer &utente){

    this->userId = utente.userId;
    this->userName = utente.userName;
    this->password = utente.password;
    this->nomeImg = utente.nomeImg;

    return *this;
}

QDataStream& operator<<(QDataStream& out,QUtenteServer& user){
    out <<user.getUserId()<< user.getUsername()<< user.getPassword()<<user.getNomeImg();
    return out;
}

QDataStream& operator>>(QDataStream& in, QUtenteServer& user){

    quint32 Id;

    QString Username;
    QString salt;
    QString Password;

    QString Nomeimg;

    in >> Id >> Username>>Password>>Nomeimg;
    user = QUtenteServer(Id,Username,salt,Password,Nomeimg);

    return in;

}

quint32 QUtenteServer::getUserId(){
    return this->userId;
}

QString QUtenteServer::getUsername(){
    return this->userName;
}

QString QUtenteServer::getSalt(){
    return this->salt;
}

QString QUtenteServer::getPassword(){
    return this->password;
}

QString QUtenteServer::getNomeImg(){
    return this->nomeImg;
}

void QUtenteServer::setSalt(QString salt){
    this->salt = salt;
}

void QUtenteServer::setUserId(quint32 userId){
    this->userId = userId;
}

void QUtenteServer::setNomeImg(QString nomeImg){
    this->nomeImg = nomeImg;
}

void QUtenteServer::setPassword(QString pass){
    this->password = pass;
}

void QUtenteServer::setUsername(QString username){
    this->userName = username;
}
