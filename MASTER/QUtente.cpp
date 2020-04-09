#include "QUtente.h"

QUtente::QUtente(quint32 userId, QString username, QString nickname, QString password, QString imgName){
    this->userId = userId;
    this->userName = username;
    this->nickName = nickname;
    this->password = password;
    this->nomeImg = imgName;
}

QUtente::QUtente(QUtente const &utente){
    this->userId = utente.userId;
    this->userName = utente.userName;
    this->nickName = utente.nickName;
    this->password = utente.password;
    this->nomeImg = utente.nomeImg;
}

QUtente QUtente::operator=(const QUtente &utente){

    this->userId = utente.userId;
    this->userName = utente.userName;
    this->nickName = utente.nickName;
    this->password = utente.password;
    this->nomeImg = utente.nomeImg;

    return *this;
}

quint32 QUtente::getUserId(){
    return this->userId;
}

QString QUtente::getUsername(){
    return this->userName;
}

QString QUtente::getNickName(){
    return this->nickName;
}

QString QUtente::getPassword(){
    return this->password;
}

QString QUtente::getNomeImg(){
    return this->nomeImg;
}

void QUtente::setNickName(QString nickName){
    this->nickName = nickName;
}

void QUtente::setUserId(quint32 userId){
    this->userId = userId;
}

void QUtente::setNomeImg(QString nomeImg){
    this->nomeImg = nomeImg;
}

void QUtente::setPassword(QString pass){
    this->password = pass;
}

void QUtente::setUsername(QString username){
    this->userName = username;
}
