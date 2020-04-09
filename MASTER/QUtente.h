#ifndef QUTENTE_H
#define QUTENTE_H


#include <QString>
#include <QObject>

class QUtente: public QObject {
    Q_OBJECT

private:
    quint32 userId;
    QString userName;
    QString nickName;
    QString password;
    QString nomeImg;


public:
    QUtente(quint32 userId, QString userName, QString nickName, QString password, QString nomeImg);
    QUtente(QUtente const &utente);
    QUtente operator=(QUtente const &utente);
    quint32 getUserId();
    QString getUsername();
    QString getPassword();
    QString getNomeImg();
    QString getNickName();

    void setNickName(QString nickName);

    void setUserId(quint32 userId);

    void setNomeImg(QString nomeImg);

    void setPassword(QString pass);

    void setUsername(QString username);
};

#endif // QUTENTE_H
