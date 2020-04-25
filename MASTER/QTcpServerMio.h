//
// Created by Edoardo Dazzara on 26/02/2020.
//

#ifndef PDS1_QTCPSERVERMIO_H
#define PDS1_QTCPSERVERMIO_H


#include <QTcpServer>

class QTcpServerMio : public QTcpServer  {
    Q_OBJECT

signals:
    void SigNuovaConnessione(qintptr socketDescriptor);

public:
    QTcpServerMio(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};


#endif //PDS1_QTCPSERVERMIO_H
