//
// Created by Edoardo Dazzara on 26/02/2020.
//

#include "QTcpServerMio.h"

QTcpServerMio::QTcpServerMio(QObject *parent): QTcpServer(parent){}

void QTcpServerMio::incomingConnection(qintptr socketDescriptor) {
    emit SigNuovaConnessione(socketDescriptor);
}