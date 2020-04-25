//
// Created by Edoardo Dazzara on 26/02/2020.
//

#ifndef PDS1_QTCPSERVERMIO_H
#define PDS1_QTCPSERVERMIO_H


#include <QTcpServer>
#include <QMessageBox>
#include <stdlib.h>

class QTcpServerMio : public QTcpServer  {
    Q_OBJECT
signals:
    void SigNuovaConnessione(qint64 socketDescriptor);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};


#endif //PDS1_QTCPSERVERMIO_H
