#include "QUser.h"

QUser::QUser(quint32 userId, QString userName): userId(userId), userName(userName){
}

QUser::QUser(){}


QDataStream& operator<<(QDataStream& out,const QUser& user){
    QUser utente = user;
    out <<utente.getUserId()<< utente.getUserName();
    return out;
}

QDataStream& operator>>(QDataStream& in, QUser& user){

    quint32 Id;
    QString Username;

    in >> Id >> Username;
    user = QUser(Id,Username);

    return in;

}


quint32 QUser::getUserId(){return this->userId;}
void QUser::setUserId(quint32 userId){this->userId = userId;}
QString QUser::getUserName(){return this->userName;}
void QUser::setUserName(QString userName){this->userName = userName;}
bool QUser::operator==(QUser user){
    if((this->getUserId() == user.getUserId()) && (this->getUserName() == user.getUserName()))
        return true;
    else
        return false;
}
