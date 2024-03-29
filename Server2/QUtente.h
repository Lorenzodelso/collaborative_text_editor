#ifndef QUTENTE_H
#define QUTENTE_H


#include <QString>
#include <QObject>
#include <QDataStream>

class QUtente: public QObject {
    Q_OBJECT

private:
    quint32 userId;
    QString userName;
    QString password;
    QString nomeImg;



public:
    QUtente();
    QUtente(quint32 userId, QString userName, QString password, QString nomeImg);
    QUtente(QUtente const &utente);
    QUtente operator=(QUtente const &utente);
    friend QDataStream& operator<<(QDataStream& out,QUtente& user);
    friend QDataStream& operator>>(QDataStream& in, QUtente& user);

    quint32 getUserId();
    QString getUsername();
    QString getPassword();
    QString getNomeImg();




    void setUserId(quint32 userId);

    void setNomeImg(QString nomeImg);

    void setPassword(QString pass);

    void setUsername(QString username);

};
Q_DECLARE_METATYPE(QUtente)

#endif // QUTENTE_H
