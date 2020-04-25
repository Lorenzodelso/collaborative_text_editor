//
// Created by Edoardo Dazzara on 22/03/2020.
//

#ifndef PDS1_QUSER_H
#define PDS1_QUSER_H


#include <QObject>
#include <QDataStream>

class QUser {

private:
    quint16 userId;
    QString userName;

public:
    QUser(quint16 userId, QString userName);
    quint16 getUserId();
    void setUserId(quint16 userId);
    QString getUserName();
    void setUserName(QString userName);
    bool operator==(QUser user);

    friend QDataStream& operator<<(QDataStream& out,const QUser& user);
    friend QDataStream& operator>>(QDataStream& in, QUser& user);


};


#endif //PDS1_QUSER_H

