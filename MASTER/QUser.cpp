#include "QUser.h"

QUser::QUser(quint16 userId, QString userName): userId(userId), userName(userName){}

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
