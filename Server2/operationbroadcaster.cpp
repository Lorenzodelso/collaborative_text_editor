#include "operationbroadcaster.h"

OperationBroadcaster::OperationBroadcaster()
{

}

void OperationBroadcaster::broadcastOperation(DocOperation docOp){
    WorkerSocket* socketToExile = socketList.value(docOp.getSiteId());
    disconnect(this,&OperationBroadcaster::sendOperationToSocket,socketToExile,&WorkerSocket::rispondiEsitoOpDoc);
    emit sendOperationToSocket("Success",docOp);
    connect(this,&OperationBroadcaster::sendOperationToSocket,socketToExile,&WorkerSocket::rispondiEsitoOpDoc);
}

void OperationBroadcaster::insertSocket(quint16 siteId, WorkerSocket *socket){
    socketList.insert(siteId,socket);
}
