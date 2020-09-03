//
// Created by Edoardo Dazzara on 22/03/2020.
//

#ifndef PDS1_QUSER_H
#define PDS1_QUSER_H


#include <QObject>
#include <QDataStream>

class QUser {

private:
    quint32 userId;
    QString userName;

public:
    QUser();
    QUser(quint32 userId, QString userName);
    quint32 getUserId();
    void setUserId(quint32 userId);
    QString getUserName();
    void setUserName(QString userName);
    bool operator==(QUser user);

    friend QDataStream& operator<<(QDataStream& out,const QUser& user);
    friend QDataStream& operator>>(QDataStream& in, QUser& user);


};

Q_DECLARE_METATYPE(QUser);
#endif //PDS1_QUSER_H
