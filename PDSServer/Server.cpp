//
// Created by Edoardo Dazzara on 26/02/2020.
//


#include "Server.h"


Server::Server() {
    currUserId=0;


    QFile file("utenti.txt");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) /*apro il file con le informazioni sugli utenti (se non esiste lo creo)*/
        {
        /*TO DO gestione errore*/
        std::cerr << "Non sono riuscito a creare/aprire il file 'utenti.txt'" <<std::flush;
        }

    QTextStream in(&file);
    /*popolo mappa users con gli utenti in questo momento esistenti*/
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList words = line.split(" ", QString::SkipEmptyParts);
        users.insert((qint32)words[0].toInt(),QUtenteServer((qint32)words[0].toInt(),words[1],words[2], words[3], words[4]));
        currUserId=(qint32)words[0].toInt(); /*eventuale aggiornamento currUserId (non capita mai perchè non eseguirò mai più questo costruttore)*/
    }

    file.close();

    mainSocketP= new QTcpServerMio();
    if (!mainSocketP->listen(QHostAddress::LocalHost,3030)) {
            return;
        }
    QObject::connect(mainSocketP, &QTcpServerMio::SigNuovaConnessione, this, &Server::nuovaConnessione);
}

void Server::nuovaConnessione(quint64 socketDescriptor){
QThread * tP = new QThread(); /*creo nuovo thread che gestirà tale connessione*/
tP->start();
WorkerSocket* wsP = new WorkerSocket(); /*creo nuovo WorkerSocket per tale connessione*/
wsP->moveToThread(tP); /*da ora l'oggetto WorkerSocket è del thread creato*/

threadsSocket.insert(wsP, tP);

/*quando il thread ha finito (connessione chiusa) elimino oggetto WorkerSocket*/
connect(tP, &QThread::finished, wsP, &QObject::deleteLater);

/*queste connect devono restare sempre attive perchè sono dal WorkerSocket verso this*/
QObject::connect(wsP, &WorkerSocket::SigApriDoc, this, &Server::apriDoc);
QObject::connect(wsP,  &WorkerSocket::SigCreaDoc, this, &Server::creaDoc);
QObject::connect(wsP,  &WorkerSocket::SigLogin, this, &Server::login);
QObject::connect(wsP,  &WorkerSocket::SigRegistrazione, this, &Server::registrazione);
QObject::connect(wsP,  &WorkerSocket::SigChiusuraDocumentoDaParteDelClient, this, &Server::chiusuraDocumentoDaParteDelClient);
QObject::connect(wsP,  &WorkerSocket::SigChiusuraConnessioneDaParteDelClient, this, &Server::chiusuraConnessioneDaParteDelClient);
QObject::connect(wsP,  &WorkerSocket::SigChiusuraConnessioneDaParteDelServer, this, &Server::chiusuraConnessioneDaParteDelServer);
QObject::connect(wsP,  &WorkerSocket::SigModificaProfiloUtente, this, &Server::modificaProfiloUtente);
QObject::connect(wsP,  &WorkerSocket::SigOpChiHaInseritoCosa, this, &Server::chiHaInseritoCosa);

/*
 * questa connect mi serve solo ora perchè è da this al WorkerSocket
 * quindi se la mantenessi sempre attiva emettendo il segnale
 * eseguirei la slot corrispondente IN OGNI WorkerSocket
 * */

QObject::connect(this, &Server::SigWorkerSocketAttivati, wsP, &WorkerSocket::WorkerSocketAttivati);

emit SigWorkerSocketAttivati(socketDescriptor);

/*infatti qui la disattivo*/
QObject::disconnect(this, &Server::SigWorkerSocketAttivati, wsP, &WorkerSocket::WorkerSocketAttivati);

}


void Server::apriDoc(QString nomeFile , WorkerSocket* wsP, QUtente user){

    /*affinchè questa richiesta vada a buon fine deve esistere già un file con nome nomeFile */


    if (QFile::exists(nomeFile)) {
        /*esiste già un file con quel nome*/

        QThread *tP;

        if (documents.contains(nomeFile)) /*ho già oggetto QThread per tale documento*/{


            WorkerDoc *wdP =documents.value(nomeFile) ;

            userEdits.insert(user.getUserId(),wdP);
            QList<QString> l=userEdited.value(user.getUserId());
            l.push_back(nomeFile);

            /*
             * questa connect mi serve solo ora perchè è da this al WorkerDoc
             * quindi se la mantenessi sempre attiva emettendo il segnale
             * eseguirei la slot corrispondente IN OGNI WorkerDoc
             * */
            QObject::connect(this, &Server::SigWorkerDocNsimaAperturaDoc, wdP, &WorkerDoc::workerDocNsimaAperturaDoc);


            QObject::connect(wsP, &WorkerSocket::SigOpDoc, wdP, &WorkerDoc::opDoc);
            QObject::connect(wdP, &WorkerDoc::SigEsitoOpDoc, wsP, &WorkerSocket::rispondiEsitoOpDoc);


            QObject::connect(wdP, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
            /*la corrispondente disconnect, necessaria per evitare che il worker doc attivi lo slot indicato in ogni worker socket
             * a cui è collegato, viene fatta dentro a &WorkerDoc::workerDocNsimaAperturaDoc*/


            emit SigWorkerDocNsimaAperturaDoc(user, wsP);


            /*infatti qui la disattivo*/
            QObject::disconnect(this, &Server::SigWorkerDocNsimaAperturaDoc, wdP, &WorkerDoc::workerDocNsimaAperturaDoc);

            QUser curusr = QUser(user.getUserId(),user.getUsername());
            /*mando ad ogni workerSocket associato a questo documento (tranne quello di questo user)
             * il segnale che dice che questo user ora è online*/

            //TO DO dovrebbe essere una lista di qint32
            QList<unsigned int> usrs = userEdits.keys(documents.value(nomeFile));
            /*utenti ora online per questo documento*/
            QList<unsigned int>::iterator i;
            for (i = usrs.begin(); i != usrs.end(); ++i){
                if(*i==user.getUserId()){/*non devo fare niente per l'utente corrente*/}
                else{
                  WorkerSocket* wsPointer = userConnections.value(*i);
                  QObject::connect(this, &Server::SigQuestoUserHaApertoIlDoc, wsPointer, &WorkerSocket::questoUserHaApertoIlDoc);
                  emit SigQuestoUserHaApertoIlDoc(curusr);
                  QObject::disconnect(this, &Server::SigQuestoUserHaApertoIlDoc, wsPointer, &WorkerSocket::questoUserHaApertoIlDoc);

                }

            }

            /*mando al workerSocket di questo user il segnale che dice che un utente ha aperto questo documento
             * tante volte quanti sono gli user (tranne lui) che ora lo stanno editando
             * */
            //TO DO dovrebbe essere una lista di qint32
            for (i = usrs.begin(); i != usrs.end(); ++i){
                if(*i==user.getUserId()){/*non devo fare niente per l'utente corrente*/}
                else{
                    QUtenteServer u = users.value(*i);
                    QObject::connect(this, &Server::SigQuestoUserHaApertoIlDoc, wsP, &WorkerSocket::questoUserHaApertoIlDoc);
                    emit SigQuestoUserHaApertoIlDoc(QUser(u.getUserId(),u.getUsername()));
                    QObject::disconnect(this, &Server::SigQuestoUserHaApertoIlDoc, wsP, &WorkerSocket::questoUserHaApertoIlDoc);

                }

            }
        }
        else { /*non ho ancora oggetto QThread per tale documento*/
            tP = new QThread();

            WorkerDoc *wdP = new WorkerDoc();
            wdP->moveToThread(tP);

            documents.insert(nomeFile, wdP);
            userEdits.insert(user.getUserId(),wdP);
            threadsDoc.insert(wdP,tP);

            QList<QString> l=userEdited.value(user.getUserId());
            l.push_back(nomeFile);

            /*
             * questa connect mi serve solo ora perchè è da this al WorkerDoc
             * quindi se la mantenessi sempre attiva emettendo il segnale
             * eseguirei la slot corrispondente IN OGNI WorkerDoc
             * */
            QObject::connect(this, &Server::SigWorkerDocPrimaAperturaDoc, wdP, &WorkerDoc::workerDocPrimaAperturaDoc);

            QObject::connect(wsP, &WorkerSocket::SigOpDoc, wdP, &WorkerDoc::opDoc);
            QObject::connect(wdP, &WorkerDoc::SigEsitoOpDoc, wsP, &WorkerSocket::rispondiEsitoOpDoc);


            QObject::connect(wdP, &WorkerDoc::SigNessunClientStaEditando, this, &Server::nessunClientStaEditando);

            /*quando il thread ha finito (documento non più editato da alcun client) elimino oggetto worker doc*/
            QObject::connect(tP, &QThread::finished, wdP, &QObject::deleteLater);

            QObject::connect(wdP, &WorkerDoc::SigEsitoApriDoc, wsP, &WorkerSocket::rispondiEsitoApriDoc);
            /*la corrispondente disconnect, necessaria per evitare che il worker doc attivi lo slot indicato in ogni worker socket
             * a cui è collegato, viene fatta dentro a &WorkerDoc::workerDocPrimaAperturaDoc*/

            tP->start();

            emit SigWorkerDocPrimaAperturaDoc(nomeFile, wsP);


            /*infatti qui la disattivo*/
            QObject::disconnect(this, &Server::SigWorkerDocPrimaAperturaDoc, wdP, &WorkerDoc::workerDocPrimaAperturaDoc);

        }


    }
    else{
        /*non esiste già un file con quel nome*/
        QObject::connect(this, &Server::SigCreaORApriDocFallito, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
        emit SigCreaORApriDocFallito("Failed", CRDT());
        QObject::disconnect(this, &Server::SigCreaORApriDocFallito, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
    }

}



void Server::creaDoc(QString nomeFile , WorkerSocket* wsP, QUtente user) {

    /*affinchè questa richiesta vada a buon fine non deve esistere già un file con nome nomeFile*/

    if (!QFile::exists(nomeFile)) {
        /*non esiste già un file con quel nome*/
        QThread *tP = new QThread();

        WorkerDoc *wdP = new WorkerDoc();
        wdP->moveToThread(tP);

        documents.insert(nomeFile, wdP);
        userEdits.insert(user.getUserId(),wdP);
        threadsDoc.insert(wdP,tP);

        QList<QString> l=userEdited.value(user.getUserId());
        l.push_back(nomeFile);

        /*
     * questa connect mi serve solo ora perchè è da this al WorkerDoc
     * quindi se la mantenessi sempre attiva emettendo il segnale
     * eseguirei la slot corrispondente IN OGNI WorkerDoc
     * */
        QObject::connect(this, &Server::SigWorkerDocCreaDoc, wdP, &WorkerDoc::workerDocCreaDoc);


        QObject::connect(wsP, &WorkerSocket::SigOpDoc, wdP, &WorkerDoc::opDoc);
        QObject::connect(wdP, &WorkerDoc::SigEsitoOpDoc, wsP, &WorkerSocket::rispondiEsitoOpDoc);



        QObject::connect(wdP, &WorkerDoc::SigNessunClientStaEditando, this, &Server::nessunClientStaEditando);


        /*quando il thread ha finito (documento non più editato da alcun client) elimino oggetto worker doc*/
        QObject::connect(tP, &QThread::finished, wdP, &QObject::deleteLater);

        QObject::connect(wdP, &WorkerDoc::SigEsitoCreaDoc, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
        /*la corrispondente disconnect, necessaria per evitare che il worker doc attivi lo slot indicato in ogni worker socket
             * a cui è collegato, viene fatta dentro a WorkerDoc::workerDocCreaDoc*/


        tP->start();

        emit SigWorkerDocCreaDoc(nomeFile, wsP);


        /*infatti qui la disattivo*/
        QObject::disconnect(this, &Server::SigWorkerDocCreaDoc, wdP, &WorkerDoc::workerDocCreaDoc);



    }
    else{
        /*esiste già un file con quel nome*/

        QObject::connect(this, &Server::SigCreaORApriDocFallito, wsP, &WorkerSocket::rispondiEsitoCreaDoc);
        emit SigCreaORApriDocFallito("Failed", CRDT());
        QObject::disconnect(this, &Server::SigCreaORApriDocFallito, wsP, &WorkerSocket::rispondiEsitoCreaDoc);

    }



}

void Server::login(WorkerSocket* wsP, QUtente user) {


    /*
     * questa connect mi serve solo ora perchè è da this al WorkerSocket
     * quindi se la mantenessi sempre attiva emettendo il segnale
     * eseguirei la slot corrispondente IN OGNI WorkerSocket
     * */
    QObject::connect(this, &Server::SigEsitoLogin, wsP, &WorkerSocket::rispondiEsitoLogin);
    /*login*/

    quint8 trovato=0;
    QMap<quint32/*clientId*/,QUtenteServer>::iterator i;
    QMap<quint32/*clientId*/,QUtenteServer>::iterator found;
    for (i = users.begin(); i != users.end(); ++i){
    if(i.value().getUsername().compare(user.getUsername())==0){
        trovato=1;
        found =i;
    }
    }
    if(trovato==1){
        /*esiste un utente con quello username quindi controllo password*/
        if(found.value().getPassword().compare(user.getPassword())==0){

            /*salt*/
            /*stessa password*/

            user.setUserId(found->getUserId());
            userConnections.insert(user.getUserId(), wsP);

            emit SigEsitoLogin(user, userEdited.value(user.getUserId()));
        }
        else{
            /*password diverse*/
            QList<QString> l = {"Failed"};
            emit SigEsitoLogin(user, l);
            /*elimino thread che gestisce quel socket */
            QThread* qtcP = threadsSocket.value(wsP);

            threadsSocket.remove(wsP);
            qtcP->quit();
            qtcP->wait();
            delete qtcP;
        }
    }
    else{
        /*non esiste un utente con quello username quindi non posso farlo accedere*/

        QList<QString> l = {"Failed"};
        emit SigEsitoLogin(user, l);
        /*elimino thread che gestisce quel socket */
        QThread* qtcP = threadsSocket.value(wsP);

        threadsSocket.remove(wsP);
        qtcP->quit();
        qtcP->wait();
        delete qtcP;
    }




    /*infatti qui la disattivo*/
    QObject::disconnect(this, &Server::SigEsitoLogin, wsP, &WorkerSocket::rispondiEsitoLogin);
}

void Server::registrazione(WorkerSocket* wsP, QUtente user) {

    /*
    * questa connect mi serve solo ora perchè è da this al WorkerSocket
    * quindi se la mantenessi sempre attiva emettendo il segnale
    * eseguirei la slot corrispondente IN OGNI WorkerSocket
    * */
    QObject::connect(this, &Server::SigEsitoRegistrazione, wsP, &WorkerSocket::rispondiEsitoRegistrazione);


    /*registrazione*/

    quint8 trovato=0;

    QMap<quint32/*clientId*/,QUtenteServer>::iterator i;
    for (i = users.begin(); i != users.end(); ++i){
        if(i.value().getUsername().compare(user.getUsername())==0){
            trovato =1;
        }
    }
    if(trovato==1){
            /*esiste già un utente con quello username quindi non posso registrare quello che ha fatto questa richiesta*/


            emit SigEsitoRegistrazione("ERR", nullptr);
            /*elimino thread che gestisce quel socket */
            QThread* qtcP = threadsSocket.value(wsP);

            threadsSocket.remove(wsP);
            qtcP->quit();
            qtcP->wait();
            delete qtcP;
        }
        else{
            /*non esiste un utente con quello username quindi posso registrare quello che ha fatto questa richiesta*/

            currUserId++;


            QString estensioneImg = user.getNomeImg().split('.', QString::SkipEmptyParts)[1];

            QUtenteServer userServerSide;

            userServerSide.setUserId(currUserId);
            userServerSide.setUsername(user.getUsername());
            userServerSide.setPassword(user.getPassword());
            userServerSide.setSalt("salt");
            userServerSide.setNomeImg(QString::number(user.getUserId()).append(estensioneImg));


            users.insert(currUserId, userServerSide);


            /*aggiorno file utenti*/
            QFile file("utenti.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text))
            {
                /*TO DO gestione errore*/
                std::cerr << "Non sono riuscito ad aprire il file 'utenti.txt'" <<std::flush;
            }
            QTextStream out(&file);
            out << currUserId << ' ' << userServerSide.getUsername() << ' ' << userServerSide.getPassword() << ' ' << userServerSide.getSalt() << userServerSide.getNomeImg() << '\n';
            file.close();

            /*se non trova la chiave detta crea un value di default ovvero una lista senza elementi all'interno*/
            emit SigEsitoRegistrazione("OK",userServerSide.getNomeImg());

        }


    /*infatti qui la disattivo*/
    QObject::disconnect(this, &Server::SigEsitoRegistrazione, wsP, &WorkerSocket::rispondiEsitoRegistrazione);

}

void Server::modificaProfiloUtente(WorkerSocket *wsP, QUtente userOld, QUtente userNew) {

    /*
       * questa connect mi serve solo ora perchè è da this al WorkerSocket
       * quindi se la mantenessi sempre attiva emettendo il segnale
       * eseguirei la slot corrispondente IN OGNI WorkerSocket
       * */
    QObject::connect(this, &Server::SigEsitoModificaProfiloUtente, wsP, &WorkerSocket::rispondiEsitoModificaProfiloUtente);

    /*in quessto metodo aggiorno mappa utenti modificando l'utente in questione*/

    if(!users.contains(userOld.getUserId())){
        /*non esiste tale utente */
        emit SigEsitoModificaProfiloUtente(userOld);
    }
    else {
        /*esiste tale utente*/

        /*se la modifica consiste nel cambio immagine profilo*/
        if (userOld.getNomeImg().compare(userNew.getNomeImg())) {

            /*setto il nome dell'immagine nuova in userNew*/
            QString estensioneImgNuova = userNew.getNomeImg().split('.', QString::SkipEmptyParts)[1];
            userNew.setNomeImg(QString::number(userNew.getUserId()).append(estensioneImgNuova));

            /*elimino immagine vecchia da disco*/
            QFile::remove(userOld.getNomeImg());

        }

        users.remove(userOld.getUserId());

        QUtenteServer userServerSide;

        userServerSide.setUserId(userNew.getUserId());
        userServerSide.setUsername(userNew.getUsername());
        userServerSide.setPassword(userNew.getPassword());
        userServerSide.setSalt("salt");
        userServerSide.setNomeImg(userNew.getNomeImg());
        users.insert(userNew.getUserId(),  userServerSide);



        /*aggiorno file utenti riscrivendolo tutto */

        QFile file("utenti.txt");
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text |
                       QIODevice::Truncate)) /*apro il file con le informazioni sugli utenti e ne cancello il contenuto*/
        {/*TO DO gestione errore*/
            std::cerr << "Non sono riuscito ad aprire il file 'utenti.txt'" <<std::flush;}

        QMap<quint32, QUtenteServer>::iterator i;
        QTextStream out(&file);
        for (i = users.begin(); i != users.end(); ++i) {
            out << currUserId << ' ' << userServerSide.getUsername() << ' ' << userServerSide.getPassword() << ' ' << userServerSide.getSalt() << userServerSide.getNomeImg() << '\n';

        }
        file.close();

        emit SigEsitoModificaProfiloUtente(userNew);
    }
    /*infatti qui la disattivo*/
    QObject::disconnect(this, &Server::SigEsitoModificaProfiloUtente, wsP, &WorkerSocket::rispondiEsitoModificaProfiloUtente);
}

void Server::chiusuraConnessioneDaParteDelClient(WorkerSocket* wsP, QUtente user) {


    QThread* qtcP = threadsSocket.value(wsP);
    userConnections.remove(user.getUserId());

    qtcP->quit();
    qtcP->wait();
    delete qtcP;
    threadsSocket.remove(wsP);



}

void Server::chiusuraConnessioneDaParteDelServer(WorkerSocket* wsP) {


    QThread* qtcP = threadsSocket.value(wsP);

    qtcP->quit();
    qtcP->wait();
    delete qtcP;
    threadsSocket.remove(wsP);



}

void Server::chiusuraDocumentoDaParteDelClient(WorkerSocket* wsP, QUtente user){
    WorkerDoc* wdP= userEdits.value(user.getUserId());
    QThread* qtdP = threadsDoc.value(wdP);
    userEdits.remove(user.getUserId());

    /*
 * questa connect mi serve solo ora perchè è da this al WorkerDoc
 * quindi se la mantenessi sempre attiva emettendo il segnale
 * eseguirei la slot corrispondente IN OGNI WorkerDoc
 * */
    QObject::connect(this, &Server::SigUnClientHaChiuso, wdP, &WorkerDoc::unClientHaChiusoIlDoc);

    emit SigUnClientHaChiuso();
    /*infatti qui la disattivo*/
    QObject::disconnect(this, &Server::SigUnClientHaChiuso, wdP, &WorkerDoc::unClientHaChiusoIlDoc);


    /*
       * questa connect mi serve solo ora perchè è da this al WorkerSocket
       * quindi se la mantenessi sempre attiva emettendo il segnale
       * eseguirei la slot corrispondente IN OGNI WorkerSocket
       * */
    QObject::connect(this, &Server::SigEsitoChiusuraDocumentoDaParteDelClient, wsP, &WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient);

    emit SigEsitoChiusuraDocumentoDaParteDelClient("Success");
    /*infatti qui la disattivo*/
    QObject::disconnect(this, &Server::SigEsitoChiusuraDocumentoDaParteDelClient, wsP, &WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient);

 /*TO DO rivedere caso fallimento*/


    /*recupero il nome del documento di interesse*/

    QString nomeFile =documents.key(userEdits.value(user.getUserId()));

    QUser curusr = QUser(user.getUserId(),user.getUsername());

    /*mando ad ogni workerSocket associato a questo documento (tranne quello di questo user)
     * il segnale che dice che questo user da ora non è più online*/

    //TO DO dovrebbe essere una lista di qint32
    QList<unsigned int> usrs = userEdits.keys(documents.value(nomeFile));
    /*utenti ora online per questo documento*/
    QList<unsigned int>::iterator i;
    for (i = usrs.begin(); i != usrs.end(); ++i){
        if(*i==user.getUserId()){/*non devo fare niente per l'utente corrente*/}
        else{
            WorkerSocket* wsPointer = userConnections.value(*i);
            QObject::connect(this, &Server::SigQuestoUserHaChiusoIlDoc, wsPointer, &WorkerSocket::questoUserHaChiusoIlDoc);
            emit SigQuestoUserHaChiusoIlDoc(curusr);
            QObject::disconnect(this, &Server::SigQuestoUserHaChiusoIlDoc, wsPointer, &WorkerSocket::questoUserHaChiusoIlDoc);
        }

    }
}

void Server::nessunClientStaEditando(QString nomeFile) {

    QThread* qtdP = threadsDoc.value(documents.value(nomeFile));
    documents.remove(nomeFile);
    qtdP->quit();
    qtdP->wait();
    delete qtdP;
    threadsDoc.remove(documents.value(nomeFile));

}

void Server::chiHaInseritoCosa(WorkerSocket* wsP, QUtente user) {
    /*
       * questa connect mi serve solo ora perchè è da this al WorkerSocket
       * quindi se la mantenessi sempre attiva emettendo il segnale
       * eseguirei la slot corrispondente IN OGNI WorkerSocket
       * */
    QObject::connect(this,  &Server::SigChiHaInseritoCosa, wsP ,&WorkerSocket::rispondiChiHaInseritoCosa);
    QList<QUser> res;
    /*recupero il nome del documento di interesse*/

    QString nomeFile =documents.key(userEdits.value(user.getUserId()));

    QMap<quint32/*clientId*/, QList<QString>>::iterator iteratore;

    for(iteratore=userEdited.begin(); iteratore!=userEdited.end(); iteratore++){
        /*per tutti gli utenti*/

        if((*iteratore).contains(nomeFile)){
            /*l'utente corrente ha modificato in passato o sta modificando ora il documento nomeFile*/
            QUtenteServer u= users.value(iteratore.key());
            res.append(QUser(u.getUserId(),u.getUsername()));
        }

    }
    emit SigChiHaInseritoCosa(res);
    /*infatti qui la disattivo*/
    QObject::disconnect(this,  &Server::SigChiHaInseritoCosa, wsP ,&WorkerSocket::rispondiChiHaInseritoCosa);




}
