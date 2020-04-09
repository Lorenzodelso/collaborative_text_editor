//
// Created by Edoardo Dazzara on 09/03/2020.
//


#ifndef PDS1_CLIENT_H
#define PDS1_CLIENT_H


#include <QObject>
#include "QUtente.h"
#include "CRDT.h"
#include "QUser.h"

class Client : public QObject{
    Q_OBJECT

public:
    /*emette il segnale SigConnessioneAlServer*/
    void attivaSocket();

    /*emette il segnale SigDisconnessione al server*/
    void disattivaSocket();

signals:

    void SigConnessioneAlServer();

    void SigDisconnessioneDalServer();

    void SigRegistrazione(QUtente user);

    void SigLogin(QUtente user);

    void SigCreaDoc(QString nomeFile);

    void SigApriDoc(QString nomeFile);

    void SigChiudiDoc(QString nomeFile);

    void SigModificaProfiloUtente(QUtente userNew);

    void SigOpDocLocale(/*rappresentazione richiesta operazione sul documento*/);

    void SigOpChiHaInseritoCosa();

public slots:


    /*
     * ritorna il paramtero ricevuto
     * */
    QString esitoConnessioneAlServer(QString esito/*esito*/);



    /*
     * fa la cosa opportuna sulla base dell'esito ricevuto
     *
     * nota:
     * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
     * il costruttore senza paramteri
     * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
     *
     * */
    void esitoRegistrazione(QString esito/*esito*/, QUtente user, QList<QString> nomiFilesEditati);

    /*
     * fa la cosa opportuna sulla base dell'esito ricevuto
     *
     * nota:
     * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
     * il costruttore senza paramteri
     * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
     *
     * */
    void esitoLogin(QString esito/*esito*/, QUtente user, QList<QString> nomiFilesEditati);

   /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoCreaDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

   /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

    /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    * */
    void esitoChiudiDoc(QString esito/*esito*/);

    /*
    * fa la cosa opportuna sulla base dell'esito ricevuto
    *
    * nota:
    * se esito = "Failed" allora gli altri parametri sono oggetti vuoti costruiti localmente sul client chiamando
    * il costruttore senza paramteri
    * se esito = "Success" allora gli altri paramteri sono la deserializzazione di oggetti mandati dal server
    *
    * */
    void esitoModificaProfiloUtente(QString esito/*esito*/, QUtente userNew);

    /*
     * manipolazione struttura CRDT conseguente all'esito ricevuto dal server e conseguenti ripercussioni sull'editor
     * NB se esito negativo devo fare UNDO della operazione locale fatta (qui la ricevo nuovamente, così so di quale si tratta)
     * */
    void esitoOpDocLocale(QString esito/*esito*//*rappresentazione operazione sul documento*/);

    /*
     * manipolazione struttura CRDT conseguente all'operazione remota ricevuta dal server e conseguenti ripercussioni sull'editor
     * */
    void opDocRemota(/*rappresentazione operazione sul documento*/);


    /*
     * mostra tale user come online, ovvero che sta editando ora il documento corrente
     * note:
     * va fatto altro, sulla base di come implementiamo le funzionalità di "mostrami gli username di chi ha inserito cosa" e
     * "mostrami gli username degli utenti ora online"
     * */
    void questoUserHaApertoIlDoc(QUser usr);

    /*
     * smette di mostrare tale user come online
     * note:
     * va fatto altro, sulla base di come implementiamo le funzionalità di "mostrami gli username di chi ha inserito cosa" e
     * "mostrami gli username degli utenti ora online"
     * */
    void questoUserHaChiusoIlDoc(QUser usr);

    /*
     * usa l'informazione ricevuta per implementare la funzione corrispondente
     * */
    void esitoOpChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/);



};


#endif //PDS1_CLIENT_H
