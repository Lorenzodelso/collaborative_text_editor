//
// Created by Edoardo Dazzara on 26/02/2020.
//

#include "QTcpServerMio.h"

void QTcpServerMio::incomingConnection(qintptr socketDescriptor) {
    quint64 sockDescr=socketDescriptor;
    emit SigNuovaConnessione(sockDescr);
}
