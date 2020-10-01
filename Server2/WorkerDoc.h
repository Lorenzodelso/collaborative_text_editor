//
// Created by Edoardo Dazzara on 26/02/2020.
//

#ifndef PDS1_WORKERDOC_H
#define PDS1_WORKERDOC_H


#include <QObject>
#include "QUtente.h"
#include "CRDT.h"
#include <QTextCursor>
#include "WorkerSocket.h"

class WorkerDoc : public QObject{

    Q_OBJECT

private:
    QString nomeFile; //mantengo anche il nome per comodità
    CRDT* crdt; //oggetto che mantiene la logica di ordinamento
    quint16 numClients=0; //contatore dei clients connessi al documento

signals:
    void SigEsitoOpDoc(QString esito, DocOperation docOp);
    void SigEsitoCreaDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);
    void SigEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/);
    void SigNessunClientStaEditando(QString nomeFile);
    void SigQuestoUserHaApertoIlDoc(QUser user);

public:
    WorkerDoc();
    ~WorkerDoc();
    QMap<quint16,QTextCursor>* cursorMap = new QMap<quint16, QTextCursor>();

public slots:




    /*
     * crea il file
     * crea l'oggetto CRDT(il cui costruttore inizializza la lista di Char sulla base del contenuto del file(ora è vuoto))
     * emette il segnale SigEsitoCreaDoc (esito, rappresentazione del file) dove:
     *  se esito negativo allora esito="Failed" e doc= CRDT()
     *  se esito positivo allora esito="Success" e doc= COPIA dell'oggetto CRDT interno
     * fa QObject::disconnect(this, &WorkerDoc::SigEsitoCreaDoc, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
     * */
    void workerDocCreaDoc(QString nomeFile, WorkerSocket* wsP);

    /*
     * crea l'oggetto CRDT(il cui costruttore inizializza la lista di Char sulla base del contenuto del file)
     * emette il segnale SigEsitoApriDoc (esito, rappresentazione del file) dove:
     *  se esito negativo allora esito="Failed" e doc= CRDT()
     *  se esito positivo allora esito="Success" e doc= COPIA dell'oggetto CRDT interno
     * fa QObject::disconnect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
     * */
    void workerDocPrimaAperturaDoc(QString nomeFile, WorkerSocket* wsP);

    /*
     * emette il segnale SigEsitoApriDoc (esito, rappresentazione del file) dove:
     *  se esito negativo allora esito="Failed" e doc= CRDT()
     *  se esito positivo allora esito="Success" e doc= COPIA dell'oggetto CRDT interno
     * fa QObject::disconnect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
     * */
    void workerDocNsimaAperturaDoc(QUtente user, WorkerSocket* wsP, QMap<QUser, WorkerSocket*> utentiGiaOnline, QMap<QUser, WorkerSocket*> nuovoUtenteOnline);



    /*
     * fa l'operazione richiesta sull'oggetto CRDT che ha all'interno
     * (l'oggetto CRDT la fa sia sulla lista interna sia sul file su disco)
     *
     * emette il segnale SigEsitoOpDoc(esito, rappresentazione richiesta operazione sul documento)
     * */
    void opDoc(DocOperation docOp);


    /*
     * nota:
     * internamente questa classe deve mantenere, tramite gestione opportuna negli slot creaDoc, primaAperturaDoc e nsimaAperturaDoc,
     * un contatore di quanti client ora stanno editando questo documento
     *
     * decrementa contatore e se diventa =0 emette segnale SigNessunClientStaEditando(Qstring NomeFile)
     *
     * */
    void unClientHaChiusoIlDoc();


    /*
     * note:
     *
     * nella classe CRDT a questo punto capita che
     * -lato server viene chiamato un suo metodo (es inserisci) e oltre a farlo su this deve farlo anche su disco
     * -lato client viene chiamato un suo metodo (es inserisci) e deve farlo solo su this
     *
     * la funzionalità "Il client può evidenziare il testo introdotto dai diversi utenti utilizzando colori differenti."
     * è da realizzare in questo modo:
     * la lista interna ad un oggetto CRDT è una lista di oggetti Pippo (non scrivo Char per non confondere perche in realtà sono
     * caratteri speciali)
     * ogni oggetto Pippo deve mantenere al suo interno l'id del client che lo ha inserito
     *
     * localmente sul client quando l'utente pigia qualcosa che attiva tale funzionalità capita che si dà colore ad un carattere
     * sulla base di quale client lo ha inserito grazie a tale informaizone, in questo modo il server non viene interpellato
     * e tale funzionalità è svolta in autonomia localmente dal client che la attiva
     *
     *
     *
     * la classe QUtente l'ho inventata per mettere le informazioni di un utente tutte assieme
     *
     * la classe CRDT deve avere il costruttore CRDT() che al suo interno non fa niente
     * (lo chiamo solo per passare comunque un oggetto CRDT allo slot del worker socket anche in caso di fallimento creazione/apertura doc)
     *
     * */


};


#endif //PDS1_WORKERDOC_H
