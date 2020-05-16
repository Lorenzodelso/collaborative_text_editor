//
// Created by Lorenzo Del Sordo on 2020-03-08.
//

#include "WorkerDoc.h"

WorkerDoc::WorkerDoc(){

}
/*
    COMMENTI EDO:
     * crea il file
     * crea l'oggetto CRDT(il cui costruttore inizializza la lista di Char sulla base del contenuto del file(ora è vuoto))
     * emette il segnale SigEsitoCreaDoc (esito, rappresentazione del file) dove:
     *  se esito negativo allora esito="Failed" e doc= CRDT()
     *  se esito positivo allora esito="Success" e doc= COPIA dell'oggetto CRDT interno
     * fa QObject::disconnect(this, &WorkerDoc::SigEsitoCreaDoc, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
     * */

//DA MODIFICARE IL COSTRUTTORE DI COPIA DEL CRDT, SE RICEVO UN CRDT CON SITEID 0, VUOL DIRE CHE LO RICEVO DAL SERVER. DEVO COSTRUIRE NEL CLIENT 
//INVECE UN CRDT IDENTICO MA CON SITEID QUELLO DEL CLIENT, QUINDI LA GESTIONE VIENE DELEGATA AL COSTRUTTORE DI COPIA
void WorkerDoc::workerDocCreaDoc(QString nomeFile, WorkerSocket* wsP){
    crdt = new CRDT(0); //CRDT vuoto perchè nuovo file
    QFile newFile(nomeFile); //creo nuovo file
    QString esito("");
    if ( !newFile.open(QIODevice::ReadWrite) ){ //controllo esito dell'operazione di apertura
        esito = "Failed";
    }
    else{
        esito="Success";
        openedFile = &newFile;
        numClients = 0; //ancora nessun client connesso, viene solo richiesto di creare il file
        this->nomeFile = nomeFile;
    }
    emit SigEsitoCreaDoc(esito,*crdt /* passo il CRDT come copia, quindi non il puntatore*/);
    QObject::disconnect(this, &WorkerDoc::SigEsitoCreaDoc, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
}


/*
     * crea l'oggetto CRDT(il cui costruttore inizializza la lista di Char sulla base del contenuto del file)
     * emette il segnale SigEsitoApriDoc (esito, rappresentazione del file) dove:
     *  se esito negativo allora esito="Failed" e doc= CRDT()
     *  se esito positivo allora esito="Success" e doc= COPIA dell'oggetto CRDT interno
     * fa QObject::disconnect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
     * */
void WorkerDoc::workerDocPrimaAperturaDoc(QString nomeFile, WorkerSocket* wsP){
    QString esito("");
    this->openedFile = new QFile(nomeFile);
    QDataStream fileStream(openedFile); //creo uno stream di dati dal file per leggere direttamente il CRDT
    crdt = new CRDT(0);

    //openedFile deve essere già inizializzato, effettuo un controllo per robustezza
    if (!openedFile->open(QIODevice::ReadWrite)){ //controllo anche apertura del file
        esito= "Failed";
    }
    else{
        esito="Success";
        fileStream >> *crdt; //deserializzo il CRDT interno al file
        numClients++;
    }
    emit SigEsitoApriDoc(esito,*crdt /* passo il CRDT come copia, quindi non il puntatore*/);
    QObject::disconnect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
}

/*
     * emette il segnale SigEsitoApriDoc (esito, rappresentazione del file) dove:
     *  se esito negativo allora esito="Failed" e doc= CRDT()
     *  se esito positivo allora esito="Success" e doc= COPIA dell'oggetto CRDT interno
     * fa QObject::disconnect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
     * */

void WorkerDoc::workerDocNsimaAperturaDoc(QUtente user,WorkerSocket* wsP){
    QString esito("");
    //N-esima apertura, quindi sia crdt che openedFile dovrebbero essere inizializzati, faccio controllo robustezza
    if (crdt == nullptr || openedFile == nullptr){
        esito = "Failed";
    }
    else{
        esito = "Success";
        numClients++;
    }
    emit SigEsitoApriDoc (esito, *crdt);
    QObject::disconnect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
}

/*
     * nota:
     * internamente questa classe deve mantenere, tramite gestione opportuna negli slot creaDoc, primaAperturaDoc e nsimaAperturaDoc,
     * un contatore di quanti client ora stanno editando questo documento
     *
     * decrementa contatore e se diventa =0 emette segnale SigNessunClientStaEditando(Qstring NomeFile)
     *
     * */
void WorkerDoc::unClientHaChiusoIlDoc(){
    numClients--;
    if (numClients==0){
        openedFile->close();
        emit SigNessunClientStaEditando(this->nomeFile);
    }
}

void WorkerDoc::opDoc(DocOperation docOp){

    //selezione dell'operazione da fare sul documento: per ora solo remote insert o delete
    std::cout <<"Ricevuto segnale di operazione...\n"<<std::flush;
    std::cout <<docOp.type<<std::flush;

    //openedFile->open(QIODevice::ReadWrite);

    QFile* file=new QFile(this->nomeFile);
    file->open(QIODevice::ReadWrite);
    this->openedFile=file;

    // salvataggio su file del crdt ogni operazione che si effettua su di esso
    QDataStream outStream(this->openedFile);

    std::cout <<this->nomeFile.toStdString()<<std::flush;


    switch(docOp.type){
        case remoteDelete:
            this->crdt->remoteDelete(docOp.character);
            outStream << *crdt;
            break;
        case remoteInsert:
            this->crdt->remoteInsert(docOp.character);

            outStream << *crdt;

            std::cout <<"inserimento terminato\n"<<std::flush;

            break;
        case cursorMoved:
            //Aggiorno la mappa dei cursori
            QTextCursor* cursor = new QTextCursor();
            cursor->setPosition(docOp.cursorPos,QTextCursor::MoveAnchor);
            cursorMap->find(docOp.siteId).value() = *cursor;
    }

    std::cout <<"Invio segnale di esito operazione...\n"<<std::flush;
    file->close();
    emit SigEsitoOpDoc("Failed",docOp);
}

