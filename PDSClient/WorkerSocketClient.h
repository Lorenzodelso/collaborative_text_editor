//
// Created by Edoardo Dazzara on 09/03/2020.
//

#ifndef PDS1_WORKERSOCKETCLIENT_H
#define PDS1_WORKERSOCKETCLIENT_H


#include <QObject>
#include "QUtente.h"
#include <QTcpSocket>
#include "CRDT.h"
#include "QUser.h"

class WorkerSocketClient : public QObject{
    Q_OBJECT

    QTcpSocket* socketConnesso;


    signals:


    void SigEsitoConnessioneAlServer(QString esito/*esito*/);

    void SigEsitoRegistrazione(QString esito/*esito*/);

    void SigEsitoLogin(QString esito/*esito*/, QUtente user, QList<QString> nomiFilesEditati);

    void SigEsitoCreaDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

    void SigEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

    void SigEsitoChiudiDoc(QString esito/*esito*/);

    void SigEsitoModificaProfiloUtente(QString esito/*esito*/, QUtente userNew);

    void SigOpDocRemota(/*rappresentazione operazione sul documento*/DocOperation docOp);

    void SigEsitoOpDocLocale(QString esito, DocOperation operazione/*esito*//*rappresentazione operazione sul documento*/);

    void SigQuestoUserHaApertoIlDoc(QUser usr);

    void SigQuestoUserHaChiusoIlDoc(QUser usr);

    void SigEsitoOpChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/);

public slots:

    /*
     * crea connessione verso server, in particolare;
     * questo oggetto ha all'interno un oggetto QTcpSocket e in questo metodo lo attacca al server
     * fa QObject::connect(ogg SocketConnesso, ready read, this,  leggiMsgApp)
     * emette SigEsitoConnessioneAlServer("Success"/"Failed")
     * */
    void connessioneAlServer();

    /*
     * chiude la connessione verso il server NB NON manda nessun messaggio applicativo
     * */
    void disconnessioneDalServer();

    /*
     * sulla base del msg di livello applicazione ricevuto emette
     *
     * SigEsitoRegistrazione
     *     se esito positivo
     *        usa come parametri di tale segnale:
     *        -per l'esito "Success"
     *
     *
     *     se esito negativo
     *        usa come parametridi tale segnale:
     *        -per l'esito "Failed"
     *
     * SigEsitoLogin
     *     se esito positivo
     *        si salva internamente l'oggetto QUtente
     *        usa come parametri di tale segnale:
     *        -per l'esito "Success"
     *        -per gli altri la deserializzazione degli oggetti ricevuti dal server
     *
     *     se esito negativo
     *        usa come parametridi tale segnale:
     *        -per l'esito "Failed"
     *        -per gli altri oggetti che costruisce lui chiamando il costruttore senza parametri
     *
     * SigEsitoCreaDoc
     *     se esito positivo
     *        usa come parametri di tale segnale:
     *        -per l'esito "Success"
     *       - per gli altri la deserializzazione degli oggetti ricevuti dal server
     *
     *     se esito negativo
     *        usa come parametridi tale segnale:
     *        -per l'esito "Failed"
     *        -per gli altri oggetti che costruisce lui chiamando il costruttore senza parametri
     *
     * SigEsitoApriDoc
     *     se esito positivo
     *        usa come parametri di tale segnale:
     *        -per l'esito "Success"
     *        -per gli altri la deserializzazione degli oggetti ricevuti dal server
     *
     *     se esito negativo
     *        usa come parametridi tale segnale:
     *        -per l'esito "Failed"
     *        -per gli altri oggetti che costruisce lui chiamando il costruttore senza parametri
     *
     * SigEsitoModificaProfiloUtente
     *     se esito positivo
     *        usa come parametri di tale segnale:
     *        -per l'esito "Success"
     *        -per gli altri la deserializzazione degli oggetti ricevuti dal server
     *
     *     se esito negativo
     *        usa come parametridi tale segnale:
     *        -per l'esito "Failed"
     *        -per gli altri oggetti che costruisce lui chiamando il costruttore senza parametri
     *
     * SigOpDocRemota
     *        usa come parametri di tale segnale :
     *        la deserializzazione degli oggetti ricevuti dal server
     *
     * SigEsitoOpDocLocale
     *     se esito positivo
     *        usa come parametri di tale segnale:
     *        -per l'esito "Success"
     *        -per gli altri la deserializzazione degli oggetti ricevuti dal server
     *
     *     se esito negativo
     *       usa come parametridi tale segnale:
     *       -per l'esito "Failed"
     *       -per gli altri la deserializzazione degli oggetti ricevuti dal server
     *
     * SigEsitoChiudiDoc
     *       usa come parametri di tale segnale:
     *       ("Success/Failed")
     *
     * SigQuestoUserHaApertoIlDoc
     *        usa come parametri di tale segnale :
     *        la deserializzazione degli oggetti ricevuti dal server
     *
     * SigQuestoUserHaChiusoIlDoc
     *        usa come parametri di tale segnale :
     *        la deserializzazione degli oggetti ricevuti dal server
     *
     * SigEsitoOpChiHaInseritoCosa
     *        usa come parametri di tale segnale :
     *        la deserializzazione degli oggetti ricevuti dal server
     *
     * */
    void leggiMsgApp();


    /*
     * manda la richiesta di registrazione al server
     * */
    void registrazione(QUtente user);

    /*
     * manda la richiesta di login al server
     * */
    void login(QUtente user);

    /*
     * manda la richiesta di creazione documento al server
     * */
    void creaDoc(QString nomeFile);

    /*
     * manda la richiesta di apertura documento al server
     * */
    void apriDoc(QString nomeFile);

    /*
     * manda la richiesta di chiusura documento al server
     * */
    void chiudiDoc(QString nomeFile);

    /*
     * manda la richiesta di modifica profilo utente
     * */
    void modificaProfiloUtente(QUtente userNew);

    /*
     * manda la richiesta dell'operazione sul documento che ho già fatto localmente
     * */
    void opDocLocale(DocOperation operazione/*rappresentazione richiesta operazione sul documento*/);

    /*
     * manda al server la richiesta di ricevere la lista di oggetti QUser dello storico del documento corrente
     * */
    void opChiHaInseritoCosa();


};


#endif //PDS1_WORKERSOCKETCLIENT_H