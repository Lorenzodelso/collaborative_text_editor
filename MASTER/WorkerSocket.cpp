#include "WorkerSocket.h"
#include <fstream>
#include <iostream>
#include <cstdio>


WorkerSocket::WorkerSocket(){


    socketConnessoP = new QTcpSocket( this );
    user = *new QUtente();

}

void WorkerSocket::WorkerSocketAttivati(quintptr socketDescriptor){

    socketConnessoP->setSocketDescriptor(socketDescriptor);

    connect(socketConnessoP, SIGNAL(readyRead()), this, SLOT(leggiMsgApp()), Qt::DirectConnection);
}



void WorkerSocket::leggiMsgApp() {

    QDataStream in(this->socketConnessoP);
    QString msg = socketConnessoP->readLine();

    if (msg.compare("open",Qt::CaseSensitivity::CaseSensitive)==0) {

        QString msg1 = socketConnessoP->readLine();

        emit SigApriDoc(msg1, this , this->user);

        return;

    }

    if (msg.compare("create",Qt::CaseSensitivity::CaseSensitive) == 0) {

        QString msg1 = socketConnessoP->readLine();

        emit SigCreaDoc(msg1, this, this->user);

        return;

    }

    if (msg.compare("login",Qt::CaseSensitivity::CaseSensitive) == 0) {

        QUtente user;

        in >> user ;

        if (user.getUsername() != NULL && user.getPassword() != NULL && user.getNomeImg() == NULL) {

            emit SigLogin(this, user);
            return;
        }

    }

    if (msg.compare("register",Qt::CaseSensitivity::CaseSensitive) == 0)
    {

        QUtente user;

        in >> user;

        QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");

        if (user.getUsername() != NULL && user.getPassword() != NULL && e.indexIn(user.getNomeImg())) {
            // TO DO: salvare immagine su disco
            emit SigRegistrazione(this, user);

            return;

        }


    }

    if (msg.compare("opdoclocale",Qt::CaseSensitivity::CaseSensitive) == 0) {


        DocOperation operazione;
        in >> operazione;
        emit SigOpDoc(operazione);

        return;


    }

    if (msg.compare("chi_inserito",Qt::CaseSensitivity::CaseSensitive) == 0) {

        emit SigOpChiHaInseritoCosa(this,this->user);
        return;
    }

    if (msg.compare("modify",Qt::CaseSensitivity::CaseSensitive) == 0)
    {

        QUtente userOLD,userNEW;

        in >> userOLD;
        in >> userNEW;

        if ( userOLD.getUsername() !=  userNEW.getUsername() &&
             userOLD.getPassword()    !=  userNEW.getPassword()  &&
             userNEW.getNomeImg()     ==  userOLD.getNomeImg()){
            emit SigModificaProfiloUtente(this, userOLD, userNEW);
        }
        return;

    }

    if(msg.compare("close_document",Qt::CaseSensitivity::CaseSensitive) == 0)
    {
         SigChiusuraDocumentoDaParteDelClient(this, this->user);
         return;

    }

    if (msg.compare("close_connection",Qt::CaseSensitivity::CaseSensitive) == 0)
    {
         SigChiusuraConnessioneDaParteDelClient(this, this->user);
         return;
    }

    SigChiusuraConnessioneDaParteDelClient(this, this->user);

}


    void WorkerSocket::rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati){

        QDataStream os(socketConnessoP);

        os<< "esito_login"<<"\n";


        if(nomiFilesEditati.contains("Failed"))
        {
            os << "failed" << "\n";

            socketConnessoP->disconnect();
            socketConnessoP->disconnectFromHost();
        }

        else{

            os << "success" << "\n";

            this->user = user;

            os << user << nomiFilesEditati;

        }
    }


    void WorkerSocket::rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/){

        QDataStream os(socketConnessoP);

        os<< "esito_apri_doc"<<"\n";

        if (esito.compare("success")==0){

            os << "open" << "\n";

            os << doc;
        }

        else{

            os << "failed" <<"\n";
        }
}


    void WorkerSocket::rispondiEsitoCreaDoc(QString esito, CRDT doc/*rappresentazione del file*/){


        QDataStream os(socketConnessoP);
        os<< "esito_Crea_doc"<<"\n";
        if (esito.compare("success")==0)
        {
            os << "create" << "\n";
            os << doc;
        }
        else{
             os << "failed" <<"\n";
        }
    }


    void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
    {
        QDataStream os(socketConnessoP);
        os<< "esito_registrazione"<<"\n";
        if (esito.compare("OK")==0)
        {
            os << "Registrazione_avvenuta" << "\n";
            socketConnessoP->disconnect();
            socketConnessoP->disconnectFromHost();
        }
        else{
            os << esito<< "\n";
            //TO DO: cambiare nome immagine ricevuta
            //rename(userOLD.getNomeImg(),  userNEW.getNomeImg() );
            socketConnessoP->disconnect();
            socketConnessoP->disconnectFromHost();
        }
    }


    void WorkerSocket::rispondiEsitoOpDoc(QString esito, DocOperation operazione)
    {

        QDataStream os(socketConnessoP);

        os << "esito_operazione" << "\n";

        if (esito.compare("success")==0)
        {
            if(operazione.getSiteId() == this->user.getUserId()) {

                   os << esito;
            }

            else{
                    os << esito << operazione;
            }
        }


        else if(user.getUserId() == this->user.getUserId()){

            os << "failed" <<"\n";
        }
    }


    void WorkerSocket::rispondiEsitoModificaProfiloUtente(QUtente userNew)
    {

        QUtente userOLD = this->user;

        QDataStream os(socketConnessoP);

        os<< "modificaprofilo"<<"\n";


        if((userOLD.getUsername() == userNew.getUsername() &&

            userOLD.getPassword() == userNew.getPassword() &&

            userNew.getNomeImg()  == userOLD.getNomeImg())){

                os << "successo"<< userNew;

        }//esito negativo

        else if( userNew.getNomeImg() != userOLD.getNomeImg()){

                //rename(userOLD.getNomeImg(),  userNew.getNomeImg() )
                //TO DO: Stesso riname di prima
            }
            this->user=userNew;
    }


    void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito/*esito*/){
     QDataStream os(socketConnessoP);
     os<< "esitochiusura"<<"\n";
     os<<esito;
        }


    void WorkerSocket::questoUserHaApertoIlDoc(QUser user) {
            QDataStream os(socketConnessoP);
            os << "user_aperto_doc" << "\n";
            os << user;
        }


    void WorkerSocket::questoUserHaChiusoIlDoc(QUser user) {

    QDataStream os(socketConnessoP);
    os << "user_chiuso_doc" << "\n";
    os << user;
}


    void WorkerSocket::rispondiChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/){

        QDataStream os(socketConnessoP);
        os << "chi_ha_inserito_cosa" << "\n";
        os << users;
}

