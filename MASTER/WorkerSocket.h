//
// Created by Edoardo Dazzara on 26/02/2020.
//

#ifndef PDS1_WORKERSOCKET_H
#define PDS1_WORKERSOCKET_H


#include <QTcpSocket>
#include "QUtente.h"
#include "CRDT.h"
#include "QUser.h"

class WorkerSocket: public QObject {
    Q_OBJECT
    QTcpSocket* socketConnessoP;



signals:

void SigApriDoc(QString nomeFile, WorkerSocket* wsP, QUtente user);
void SigCreaDoc(QString nomeFile , WorkerSocket* wsP, QUtente user);
void SigLogin(WorkerSocket* wsP, QUtente user);
void SigRegistrazione(WorkerSocket* wsP, QUtente user);
void SigModificaProfiloUtente(WorkerSocket* wsP, QUtente userOld, QUtente userNew);
void SigChiusuraConnessioneDaParteDelClient(WorkerSocket* wsP, QUtente user);
void SigChiusuraDocumentoDaParteDelClient(WorkerSocket* wsP, QUtente user);
void SigOpDoc(/*rapprensetazione richiesta operazione sul documento*/);
void SigOpChiHaInseritoCosa(WorkerSocket* wsP, QUtente user);


public slots:
    /*
     * creo ogg socketConnesso e gli setto il socket descriptor ricevuto come parametro
     * fa QObject::connect(ogg SocketConnesso, ready read, this,  leggiMsgApp)
     * fa la connect per rendersi conto se il client ha chiuso la connessione, ovvero QObject::connect(ogg SocketConnesso, QUALCOSA , this,  leggiMsgApp)
     * */

    void WorkerSocketAttivati(quintptr socketDescriptor);


    /*
     * sulla base del msg app emetto:
     * SigApriDoc
     *
     * SigCreaDoc
     *
     * SigLogin
     *   in questo caso dentro a user devono esserci settati sia username sia password (e basta)
     *
     * SigRegistrazione
     *   in questo caso dentro a user devono esserci settati sia username sia password sia nomeImg (e basta)
     *   NB nome img deve contenere l'estensione es: "pippo.png"
     *   in questo caso tengo salvata in un oggetto l'immagine dell'utente e NON la salvo su disco
     *
     * SigModificaProfiloUtente
     *   in questo caso userOld deve essere una copia dell'oggetto user che ti sei memorizzato qui internamente dopo login/registrazione con successo
     *   modifica profilo può essere:
     *   -modifica username: userNew deve avere tutto settato come userOld tranne username con il nuovo username
     *   -modifica password: userNew deve avere tutto settato come userOld tranne password con la nuova password
     *   -modifica immagine: userNew deve avere tutto settato come userOld tranne nomeImg con il nome dell'immagine nuova (con estensione)
     *      in questo caso tengo salvata in un oggetto l'immagine nuova dell'utente e NON la salvo su disco
     *
     * SigChiusuraDocumentoDaParteDelClient
     *
     * SigChiusuraConnessioneDaParteDelClient  (il client ha chiuso la app)
     *
     * SigOpDoc
     *
     * SigOpChiHaInseritoCosa
     * */
    void leggiMsgApp();

    /*
     * risponde al client dandogli l'esito della sua richiesta di login
     *
     * in particolare:
     *
     * se esito positivo(= se la lista NON ha una sola QString con scritto "failed"):
     *   memorizza internamente una copia dell'oggetto QUtente che riceve
     *   dà al client:
     *     i nomi dei file precedentemente da lui editati (= la lista vuota)
     *     l'oggetto QUtente che riceve, in modo che il client (socket) possa salvarselo e inserire quindi in ogni successiva richiesta il suo userId
     *
     *
     * se esito negativo(=se la lista ha una sola QString con scritto "Failed"):
     *   chiude la connessione
     * */
    void rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati);

    /*
   * risponde al client dandogli l'esito della sua richiesta di creazione doc
   * in particolare:
   * se esito="Success" allora
   *   -rispondo al client che la sua richiesta ha avuto successo
   *   -doc è la copia dell'oggetto CRDT da mandare al client
   * se esito ="Failed" allora
   *  -rispondo al client che la sua richeista è fallita
   *  -doc è un oggetto CRDT vuoto (quindi inutile) da NON mandare al client
   * */
    void rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

    /*
     * risponde al client dandogli l'esito della sua richiesta di creazione doc
     * in particolare:
     * se esito="Success" allora
     *   -rispondo al client che la sua richiesta ha avuto successo
     *   -doc è la copia dell'oggetto CRDT da mandare al client
     * se esito ="Failed" allora
     *  -rispondo al client che la sua richeista è fallita
     *  -doc è un oggetto CRDT vuoto (quindi inutile) da NON mandare al client
     * */
    void rispondiEsitoCreaDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);

    /*
     *
     * risponde al client dandogli l'esito della sua richiesta di registrazione
     *
     * in particolare:
     *
     * se esito positivo(= se la lista è vuota):
     *   memorizza internamente una copia dell'oggetto QUtente che riceve
     *   salva su disco l'immagine dell'utente precedentemente ricevuta chiamandola con il nome scritto dentro a user
     *   dà al client:
     *     i nomi dei file precedentemente da lui editati (= la lista vuota)
     *     l'oggetto QUtente che riceve, in modo che il client (socket) possa salvarselo e inserire quindi in ogni successiva richiesta il suo userId
     *
     * se esito negativo(=se la lista ha una sola QString con scritto "Failed"):
     *   chiude la connessione
     * */
    void rispondiEsitoRegistrazione(QUtente user, QList<QString> nomiFilesEditati);


    /*
     * se l'operazione ha esito positivo allora
     *
     *  se l'ha richiesta questo stesso client allora gli mando solo l'esito
     *  se l'ha richiesta un altro client allora la mando a questo client in modo che la faccia anche lui
     *
     * se l'operazione ha esito negativo allora
     *
     *  se l'ha richiesta questo stesso client allora gli mando solo l'esito
     *  se l'ha richiesta un altro client allora non faccio niente
     *
     * */
    void rispondiEsitoOpDoc(/*esito, rappresentazione richiesta operazione sul documento*/);





    /*
     * risponde al client dandogli l'esito della sua richiesta di modifica profilo
     *
     * innanzitutto deve capire se l'esito è positivo o negativo
     *
     * se esito negativo(= userNew e l'oggetto QUtente che hai salvato internamente hanno TUTTI i campi uguali)
     *   risponde al client che l'esito è stato negativo
     *
     * se esito positivo(= userNew e l'oggetto QUtente che hai salvato internamente hanno UN campo diverso):
     *   -se usernames diversi -> la modifica era una modifica dello username e ha avuto successo
     *   -se passwords diverse -> la modifica era una modifica della password e ha avuto successo
     *   -altrimenti -> la modifica era una modifica dell'immagine e ha successo
     *     in questo salva su disco l'immagine nuova dell'utente precedentemente ricevuta chiamandola con il nome scritto dentro a user
     *     (NB la vecchia immagine è già stata cancellata da disco, non devi preoccupartene)
     *
     * indipendentemente dalla operazine specifica che ha avuto successo sostituisce l'oggetto interno QUtente con userNew
     *
     * */
    void rispondiEsitoModificaProfiloUtente(QUtente userNew);

    /*
     * risponde al client dandogli l'esito della sua richiesta di chiusura documento
     * */
    void rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito/*esito*/);

    /*
     * manda al client l'informazione che lo user che ha ricevuto come parametro da ora è online
     * */
    void questoUserHaApertoIlDoc(QUser user);

    /*
     * manda al client l'informazione che lo user che ha ricevuto come parametro da ora ora non è più online
     * */
    void questoUserHaChiusoIlDoc(QUser user);

    /*
     * manda al client ciò che ha ricevuto come parametro
     * */
    void rispondiChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/);

    /*
     * note:
     *
     * lato client se ricevo che la mia operazione ha avuto esito negativo devo fare UNDO di tale operazione che
     * ho già fatto in locale
     *
     * */



};


#endif //PDS1_WORKERSOCKET_H
