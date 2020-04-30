
#include <fstream>
#include <iostream>
#include <cstdio>


WorkerSocket::WorkerSocket(){

    qRegisterMetaType<QUtente>();
    socketConnessoP = new QTcpSocket( this );
    user = *new QUtente();


}

void WorkerSocket::WorkerSocketAttivati(quint64 socketDescriptor){

    //DEBUG
    std::cout << "Ricevuto segnale workerSocketAttivati"<<std::flush;
    //DEBUG

    socketConnessoP->setSocketDescriptor(socketDescriptor);

    connect(socketConnessoP, &QTcpSocket::readyRead, this, &WorkerSocket::leggiMsgApp);
}



void WorkerSocket::leggiMsgApp() {

    QDataStream in(this->socketConnessoP);
    //QString msg = socketConnessoP->readLine();
    char* msg;
    uint prova = 3;
    in.readBytes(msg,prova);

    //DEBUG
    std::cout<<"\nRicevuto segnale readyRead - Slot leggiMsgApp"<<std::flush;
    std::cout<<"\n"+msg.toStdString()<<std::flush;
    //DEBUG

    if (msg.compare("ope",Qt::CaseSensitivity::CaseSensitive)==0) {

        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        emit SigApriDoc(msg1, this , this->user);

        return;

    }

    if (msg.compare("cre",Qt::CaseSensitivity::CaseSensitive) == 0) {

        //QString msg1 = socketConnessoP->readLine();
        char* msg1;
        uint prova = 3;
        in.readBytes(msg1,prova);

        emit SigCreaDoc(msg1, this, this->user);

        return;

    }

    if (msg.compare("log",Qt::CaseSensitivity::CaseSensitive) == 0) { //ok

        QUtente user;

        in >> user ;

        if (user.getUsername() != NULL && user.getPassword() != NULL && user.getNomeImg() == NULL) {

            emit SigLogin(this, user);
            return;
        }

    }

    if (msg.compare("reg",Qt::CaseSensitivity::CaseSensitive) == 0) //ok
    {

        QUtente user;
        qint64 dimension;
        Qbytearray data;

        in >> dimension
        in >> dimension

        //in.readBytes(data,dimension);
       // img = QImage::fromData(data,"PNG");

        this->image=img;

        in >> user;

        //DEBUG
        std::cout<<"\nUser ricevuto: "+user.getUsername().toStdString()<<std::flush;
        //DEBUG
        QRegExp e("([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.doc|.docx|.pdf|.png)$");

        if (user.getUsername() != NULL && user.getPassword() != NULL && e.indexIn(user.getNomeImg())) {

            emit SigRegistrazione(this, user);

            return;

        }


    }

    if (msg.compare("opd",Qt::CaseSensitivity::CaseSensitive) == 0) { //opd


        DocOperation operazione;
        in >> operazione;
        emit SigOpDoc(operazione);

        return;


    }

    if (msg.compare("c_i",Qt::CaseSensitivity::CaseSensitive) == 0) { //ok

        emit SigOpChiHaInseritoCosa(this,this->user);
        return;
    }

    if (msg.compare("mod",Qt::CaseSensitivity::CaseSensitive) == 0) //ok
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

    if(msg.compare("c_d",Qt::CaseSensitivity::CaseSensitive) == 0)
    {
        SigChiusuraDocumentoDaParteDelClient(this, this->user);
        return;

    }

    if (msg.compare("c_c",Qt::CaseSensitivity::CaseSensitive) == 0)
    {
        SigChiusuraConnessioneDaParteDelClient(this, this->user);
        return;
    }

    SigChiusuraConnessioneDaParteDelClient(this, this->user);

}


void WorkerSocket::rispondiEsitoLogin(QUtente user, QList<QString> nomiFilesEditati){

    QDataStream os(socketConnessoP);

    socketConnessoP->write("e_l");


    if(nomiFilesEditati.contains("Failed"))
    {
        socketConnesso->write("fld");

        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }

    else{

        socketConnesso->write("suc");

        this->user = user;

        os << user << nomiFilesEditati;

    }
}


void WorkerSocket::rispondiEsitoApriDoc(QString esito/*esito*/, CRDT doc/*rappresentazione del file*/){

    QDataStream os(socketConnessoP);

    socketConnesso->write("ead"); //ok

    if (esito.compare("success")==0){

        socketConnesso->write("ope");

        os << doc;
    }

    else{

        socketConnesso->write("fld");
    }
}


void WorkerSocket::rispondiEsitoCreaDoc(QString esito, CRDT doc/*rappresentazione del file*/){


    QDataStream os(socketConnessoP);
    socketConnesso->write("ecd"); // ok
    if (esito.compare("success")==0)
    {
        socketConnesso->write("crt"); //ok
        os << doc;
    }
    else{
        socketConnesso->write("fld");
    }
}


void WorkerSocket::rispondiEsitoRegistrazione(QString esito, QString nomeImg)
{
    QDataStream os(socketConnessoP);
    socketConnesso->write("e_r"); //ok
    if (esito.compare("OK")==0)
    {
        socketConnesso->write("r_a");
        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }
    else{

        os << esito<< "\n";

        this->image.save(QDir::curPath+"/"+nomeImg);

        socketConnessoP->disconnect();
        socketConnessoP->disconnectFromHost();
    }
}


void WorkerSocket::rispondiEsitoOpDoc(QString esito, DocOperation operazione)
{

    QDataStream os(socketConnessoP);

    socketConnesso->write("e_o"); //ok

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

        socketConnesso->write("fld");
    }
}


void WorkerSocket::rispondiEsitoModificaProfiloUtente(QUtente userNew)
{

    QUtente userOLD = this->user;

    QDataStream os(socketConnessoP);

    socketConnesso->write("mop"); // ok


    if((userOLD.getUsername() == userNew.getUsername() &&

        userOLD.getPassword() == userNew.getPassword() &&

        userNew.getNomeImg()  == userOLD.getNomeImg())){

        socketConnesso->write("suc");

        os<< userNew;

    }//esito negativo

    else if( userNew.getNomeImg() != userOLD.getNomeImg()){

        //rename(userOLD.getNomeImg(),  userNew.getNomeImg() )
        //TO DO: Stesso riname di prima
    }
    this->user=userNew;
}


void WorkerSocket::rispondiEsitoChiusuraDocumentoDaParteDelClient(QString esito/*esito*/){
    QDataStream os(socketConnessoP);
    socketConnesso->write("c_d"); //ok
    os<<esito;
}


void WorkerSocket::questoUserHaApertoIlDoc(QUser user) {
    QDataStream os(socketConnessoP);
    socketConnesso->write("uad");
    os << user;
}


void WorkerSocket::questoUserHaChiusoIlDoc(QUser user) {

    QDataStream os(socketConnessoP);
    socketConnesso->write("ucd");
    os << user;
}


void WorkerSocket::rispondiChiHaInseritoCosa(QList<QUser> users/*lista degli utenti che hanno editato in passato e/o stanno editando questo doc*/){

    QDataStream os(socketConnessoP);
    socketConnesso->write("c_i"); //ok
    os << users;
}
