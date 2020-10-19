//
// Created by Edoardo Dazzara on 26/02/2020.
//

#ifndef PDS1_SERVER_H
#define PDS1_SERVER_H


#include <map>
#include <QObject>
#include "QTcpServerMio.h"
#include "WorkerSocket.h"
#include "QUtente.h"
#include "WorkerDoc.h"
#include <QFile>
#include "QTcpServerMio.h"
#include "WorkerDoc.h"
#include "QUtenteServer.h"
#include "operationbroadcaster.h"
#include <QThread>
#include <iostream>
/*
 * un thread che possiede un oggetto Server
 * per ogni client: un thread che possiede un oggetto WorkerSocket
 * per ogni documento in corso di modifica: un thread che possiede un oggetto WorkerDoc
 * comunicazioni tra threads mediante signal e slot:
 * un oggetto A posseduto da un thead ALFA emette un segnale connesso ad uno slot di un oggetto B posseduto da un thread BETA
 * risultato: lo slot di B viene eseguito dal thread BETA quando tale thread lo estrae dalla propria coda
 *
 * */

class Server : public QObject {
    Q_OBJECT
    /*
     * mantengo memorizzati dentro all'oggetto server i puntatori agli oggetti WorkerDoc e WorkerSocket che creo man mano
     * perchè devo connettere e disconnettere tra loro i vari signal e slot
     * nota bene: NON accedo mai ad un oggetto da un thread diverso da quello che lo possiede
     *
     * */

    /*
     * salvo qui l'informazione per ogni documento che sta venendo modificato da almeno un client di quale è
     * l'oggetto WorkerDoc corrispondente
     *
     * */
    QMap<QString/*nomeFile*/, WorkerDoc*> documents;

    /*
     * il processo server è un processo che nasce e non muore mai
     * qui tengo memorizzati tutti gli utenti della applicazione
     * */
    QMap<quint32/*clientId*/,QUtenteServer> users;

    quint32 currUserId;


    /*
     * se l'operazione di login del client ha successo salvo ANCHE qui il puntatore al WorkerSocket
     * corrispondente
     * non solo, ma lo salvo associandolo all'identificativo del client in questione
     * in questo modo mantengo l'informazione su quale oggetto WorkerSocket è associato a quale client (che ha fatto
     * accesso con successo)
     *
     * */
    QMap<quint32 /*clientId*/,WorkerSocket*> userConnections;

    /*
     * salvo qui l'informazione per ogni utente ATTIVO di quale WorkerDoc (=documento) sta modificando in questo momento
     * */
    QMap<quint32/*clientId*/, WorkerDoc*> userEdits;

    /*
     * salvo qui l'informazione per ogni utente di quali documenti ha editato fino ad ora
     * */
    QMap<quint32/*clientId*/, QList<QString>/*lista di nomi dei file editati fino ad ora dall'utente*/> userEdited;

    QMap<WorkerDoc*, QThread*> threadsDoc;
    QMap<WorkerSocket*, QThread*> threadsSocket;

    /*
     * socket principale su cui arrivano le richieste di connessione dei clients
     * */
    QTcpServerMio* mainSocketP;

    QMap<QString,OperationBroadcaster*> broadcasterMap;

public slots:


void nuovaConnessione(quint64 socketDescriptor);
void apriDoc(QString nomeFile, WorkerSocket* wsP, QUtente user);
void creaDoc(QString nomeFile , WorkerSocket* wsP, QUtente user);
void login(WorkerSocket* wsP, QUtente user);
void registrazione(WorkerSocket* wsP, QUtente user);
void chiusuraConnessioneDaParteDelClient(WorkerSocket* wsP, QUtente user);
void chiusuraConnessioneDaParteDelServer(WorkerSocket* wsP);
void chiusuraDocumentoDaParteDelClient(WorkerSocket* wsP, QUtente user);
void modificaProfiloUtente(WorkerSocket* wsP, QUtente userOld, QUtente userNew);
void nessunClientStaEditando(QString nomeFile);
void chiHaInseritoCosa(WorkerSocket* wsP, QUtente user);

signals:
void SigWorkerSocketAttivati(quint64 socketDescriptor);
void SigUnClientHaChiuso();
void SigWorkerDocCreaDoc(QString nomeFile, WorkerSocket* wsP);
void SigCreaORApriDocFallito(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);
void SigWorkerDocPrimaAperturaDoc(QString nomeFile, WorkerSocket* wsP);
void SigWorkerDocNsimaAperturaDoc(QUtente user, WorkerSocket* wsP, QMap<QUser, WorkerSocket*> utentiGiaOnline, QMap<QUser, WorkerSocket*> nuovoUtenteOnline);
void SigEsitoChiusuraDocumentoDaParteDelClient(QString esito/*esito*/);

void SigEsitoLogin(QUtente user, QList<QString> nomiFilesEditati);
void SigEsitoRegistrazione(QString esito, QString nomeImg);
void SigEsitoModificaProfiloUtente(QUtente userNew,bool immagineModificata);
void SigChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/);
void SigQuestoUserHaChiusoIlDoc(QUser user);


public:
    Server();


};




#endif //PDS1_SERVER_H


