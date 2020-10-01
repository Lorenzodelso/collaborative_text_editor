//
// Created by Lorenzo Del Sordo on 2020-03-08.
//

#include "WorkerDoc.h"

WorkerDoc::WorkerDoc(){

}

WorkerDoc::~WorkerDoc(){
    delete cursorMap;
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
    crdt->localInsert('\0',QTextCharFormat(),0);
    QFile newFile(nomeFile); //creo nuovo file
    QString esito("");
    if ( !newFile.open(QIODevice::WriteOnly) ){ //controllo esito dell'operazione di apertura
        esito = "Failed";
    }
    else{
        esito="Success";
        numClients = 1; //ancora nessun client connesso, viene solo richiesto di creare il file
        this->nomeFile = nomeFile;
        QDataStream outStream(&newFile);
        outStream<<*crdt;
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
    QFile file(nomeFile);
    QDataStream fileStream(&file); //creo uno stream di dati dal file per leggere direttamente il CRDT
    crdt = new CRDT(0);
    QObject::connect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
    /*la corrispondente disconnect, necessaria per evitare che il worker doc attivi lo slot indicato in ogni worker socket
     * a cui è collegato, viene fatta qui in fondo*/
    //openedFile deve essere già inizializzato, effettuo un controllo per robustezza
    if (!file.open(QIODevice::ReadOnly)){ //controllo anche apertura del file
        esito= "Failed";
    }
    else{
        esito="Success";
        fileStream >> *crdt; //deserializzo il CRDT interno al file
        this->nomeFile = nomeFile;
        numClients = 1;
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

void WorkerDoc::workerDocNsimaAperturaDoc(QUtente user,WorkerSocket* wsP, QMap<QUser, WorkerSocket*> utentiGiaOnline, QMap<QUser, WorkerSocket*> nuovoUtenteOnline){
    QString esito("");
    QObject::connect(this, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
    /*la corrispondente disconnect, necessaria per evitare che il worker doc attivi lo slot indicato in ogni worker socket
     * a cui è collegato, viene fatta qui in fondo*/
    //N-esima apertura, quindi sia crdt che openedFile dovrebbero essere inizializzati, faccio controllo robustezza
    if (crdt == nullptr || nomeFile == nullptr){
        esito = "Failed";
        emit SigEsitoApriDoc(esito, *crdt);
    }
    else{
        esito = "Success";
        emit SigEsitoApriDoc(esito, *crdt);
        numClients++;
        QMap<QUser, WorkerSocket*>::iterator i1;
        for (i1 = utentiGiaOnline.begin(); i1 != utentiGiaOnline.end(); ++i1) {
            QObject::connect(this, &WorkerDoc::SigQuestoUserHaApertoIlDoc, i1.value(), &WorkerSocket::questoUserHaApertoIlDoc);
            emit SigQuestoUserHaApertoIlDoc(QUser(user.getUserId(),user.getUsername()));
            QObject::disconnect(this, &WorkerDoc::SigQuestoUserHaApertoIlDoc,i1.value(), &WorkerSocket::questoUserHaApertoIlDoc);

        }
        QMap<QUser, WorkerSocket*>::iterator i2;
        for (i2 = nuovoUtenteOnline.begin(); i2 != nuovoUtenteOnline.end(); ++i2) {
            QMap<QUser, WorkerSocket*>::iterator i3;
            for (i3 = utentiGiaOnline.begin(); i3 != utentiGiaOnline.end(); ++i3) {
                QObject::connect(this, &WorkerDoc::SigQuestoUserHaApertoIlDoc, i2.value(), &WorkerSocket::questoUserHaApertoIlDoc);
                emit SigQuestoUserHaApertoIlDoc(i3.key());
                QObject::disconnect(this, &WorkerDoc::SigQuestoUserHaApertoIlDoc,i2.value(), &WorkerSocket::questoUserHaApertoIlDoc);
            }
        }
    }
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
        delete crdt;
        emit SigNessunClientStaEditando(this->nomeFile);
    }
}

void WorkerDoc::opDoc(DocOperation docOp){

    QFile file(this->nomeFile);
    file.open(QIODevice::WriteOnly);
    // salvataggio su file del crdt ogni operazione che si effettua su di esso
    QDataStream outStream(&file);
    switch(docOp.type){
        case remoteDelete:
    {
            this->crdt->remoteDelete(docOp.character);
            outStream << *crdt;
            break;
    }
        case remoteInsert:
    {
            this->crdt->remoteInsert(docOp.character);
            outStream << *crdt;
            break;
    }
        case changedFormat:
    {
            this->crdt->remoteFormatChange(docOp.character);
            outStream << *crdt;
            break;
    }
        case cursorMoved:
    {
        /*
            //Aggiorno la mappa dei cursori
            QTextCursor* cursor = new QTextCursor();
            cursor->setPosition(docOp.cursorPos,QTextCursor::MoveAnchor);
            cursorMap->find(docOp.siteId).value() = *cursor;
            break;
            */
    }
        case alignementChanged:
    {
            this->crdt->setCharAlign(docOp.alignementType,docOp.cursorPos);
            outStream << *crdt;
            break;
    }
    }
    emit SigEsitoOpDoc("Success",docOp);
}

