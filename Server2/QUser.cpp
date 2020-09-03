#include "QUser.h"

QUser::QUser(quint16 userId, QString userName): userId(userId), userName(userName){
}

QUser::QUser(){}


QDataStream& operator<<(QDataStream& out,QUser& user){
    out <<user.getUserId()<< user.getUserName();
    return out;
}

QDataStream& operator>>(QDataStream& in, QUser& user){

    quint16 Id;
    QString Username;

    in >> Id >> Username;
    user =  *new QUser(Id,Username);

    return in;

}


quint16 QUser::getUserId(){return this->userId;}
void QUser::setUserId(quint16 userId){this->userId = userId;}
QString QUser::getUserName(){return this->userName;}
void QUser::setUserName(QString userName){this->userName = userName;}
bool QUser::operator==(QUser user){
    if((this->getUserId() == user.getUserId()) && (this->getUserName() == user.getUserName()))
        return true;
    else
        return false;
}
