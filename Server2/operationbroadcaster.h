#ifndef OPERATIONBROADCASTER_H
#define OPERATIONBROADCASTER_H

#include <QObject>
#include "WorkerSocket.h"

class OperationBroadcaster: public QObject{
    Q_OBJECT
public:
    OperationBroadcaster();
    void insertSocket(quint16 siteId, WorkerSocket* socket);
public slots:
     void broadcastOperation(DocOperation docOp);

signals:
     void sendOperationToSocket(QString esito, DocOperation docOp);

private:
    QMap<quint16,WorkerSocket*> socketList;
};

#endif // OPERATIONBROADCASTER_H
