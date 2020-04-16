//
// Created by Edoardo Dazzara on 16/04/2020.
//

#ifndef PDS1_QUTENTESERVER_H
#define PDS1_QUTENTESERVER_H


#include <QObject>

class QUtenteServer : public QObject{
    Q_OBJECT

public:
    QUtenteServer();
    QUtenteServer(quint32 userId, QString userName, QString password,QString salt,  QString nomeImg);
    QUtenteServer(QUtenteServer const &utente);
    QUtenteServer operator=(QUtenteServer const &utente);
    quint32 getUserId();
    QString getUsername();
    QString getPassword();
    QString getSalt();
    QString getNomeImg();

    void setUserId(quint32 userId);

    void setUsername(QString username);
    void setSalt(QString salt);
    void setPassword(QString password);
    void setNomeImg(QString nomeImg);

};


#endif //PDS1_QUTENTESERVER_H
