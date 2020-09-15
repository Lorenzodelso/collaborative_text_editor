#include "QUtente.h"

QUtente::QUtente(quint32 userId, QString username,QString password, QString imgName){
    this->userId = userId;
    this->userName = username;

    this->password = password;
    this->nomeImg = imgName;
}

QUtente::QUtente(){

}

QUtente::QUtente(QUtente const &utente){
    this->userId = utente.userId;
    this->userName = utente.userName;
    this->password = utente.password;
    this->nomeImg = utente.nomeImg;
}

QUtente QUtente::operator=(const QUtente &utente){

    this->userId = utente.userId;
    this->userName = utente.userName;
    this->password = utente.password;
    this->nomeImg = utente.nomeImg;

    return *this;
}

QDataStream& operator<<(QDataStream& out,QUtente& user){
    out <<user.getUserId()<< user.getUsername()<< user.getPassword()<<user.getNomeImg();
    return out;
}

QDataStream& operator>>(QDataStream& in, QUtente& user){

    quint32 Id;

    QString Username;
    QString Password;

    QString Nomeimg;

    in >> Id >> Username >>Password>>Nomeimg;
    user =  *new QUtente(Id,Username,Password,Nomeimg);

    return in;

}

quint32 QUtente::getUserId(){
    return this->userId;
}

QString QUtente::getUsername(){
    return this->userName;
}

QString QUtente::getPassword(){
    return this->password;
}

QString QUtente::getNomeImg(){
    return this->nomeImg;
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
