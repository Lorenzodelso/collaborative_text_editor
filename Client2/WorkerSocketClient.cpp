#include "WorkerSocketClient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QImage>
#include <QDebug>


void WorkerSocketClient::connessioneAlServer() {

    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QUser>();
    qRegisterMetaType<QList<QString>>();
    qRegisterMetaType<CRDT>();
    qRegisterMetaType<DocOperation>();

    this->socketConnesso = new QTcpSocket( this );

    this->socketConnesso->connectToHost(QHostAddress::LocalHost, 3030 );

    bool connected = (socketConnesso->state() == QTcpSocket::ConnectedState);

    if(connected==true)

      emit SigEsitoConnessioneAlServer("Success");

    else  emit SigEsitoConnessioneAlServer("Failed");

    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp,Qt::QueuedConnection);
}


void WorkerSocketClient::disconnessioneDalServer()
{
    this->socketConnesso->disconnect();
    this->socketConnesso->disconnectFromHost();
}

void  WorkerSocketClient::leggiMsgApp(){
    while(socketConnesso->bytesAvailable()){
        qDebug()<<"inizio lettura";

        char* msg;

        char* opt;

        QDataStream in(this->socketConnesso);

        uint prova = 3;
        in.readBytes(msg, prova);

        if (strcmp(msg,"ead")==0)
        {
            in.readBytes(opt, prova);

            if (strcmp(opt,"ope")==0)
            {
                CRDT doc;

                BlockReader(socketConnesso).stream() >> doc;

                emit SigEsitoApriDoc("Success", doc);
             }
            else
            {
                CRDT doc=*new CRDT();

                emit SigEsitoApriDoc("Failed", doc);
            }
        }

        if (strcmp(msg,"ecd")==0){

            in.readBytes(opt,prova);

            if (strcmp(opt,"crt")==0)
            {
                CRDT doc;

                BlockReader(socketConnesso).stream() >> doc;

                emit SigEsitoCreaDoc( "Success", doc);
           }
           else
           {
              CRDT doc =*new CRDT();

              emit SigEsitoCreaDoc("Failed", doc);
           }
        }

        if(strcmp(msg,"e_l")==0)
        {
            in.readBytes(opt, prova);

            if(strcmp(opt,"suc")==0)
            {
                QList<QString> nomiFilesEditati;

                QUtente user = *new QUtente();

                BlockReader(socketConnesso).stream() >> user;

                BlockReader(socketConnesso).stream() >> nomiFilesEditati;

                emit SigEsitoLogin( "Success"/*esito*/, user, nomiFilesEditati);
            }

            else
            {
                QList <QString> nomiFilesEditati;

                QUtente user = *new QUtente();

                emit SigEsitoLogin("Failed", user, nomiFilesEditati);
            }
        }

        if (strcmp(msg,"eor")==0) // da rivedere, esito operazione remota
        {
            DocOperation operazione;

            BlockReader(socketConnesso).stream() >> operazione;

            emit SigOpDocRemota(operazione);
        }

        if (strcmp(msg,"e_c")==0)
        {

            in.readBytes(opt,prova);

            if (strcmp(opt,"suc")==0) emit SigEsitoChiudiDoc("Success");

            else  emit SigEsitoChiudiDoc("Failed");

        }
        if (strcmp(msg,"mop")==0)
        {
            in.readBytes(opt, prova);

            if (strcmp(opt,"suc")==0)
            {
                QUtente userNew;

                BlockReader(socketConnesso).stream() >> userNew;

                emit SigEsitoModificaProfiloUtente("Success",userNew);
            }
            else
            {
                QUtente user=*new QUtente();

                emit  SigEsitoModificaProfiloUtente("Failed", user);
            }
        }

        if (strcmp(msg,"e_o")==0)
        {
            in.readBytes(opt, prova);

            DocOperation operazione;

            qDebug()<<"esito_operazione: "<<opt<<"\n";
            if (strcmp(opt,"suc")==0)
            {
                BlockReader(socketConnesso).stream() >> operazione;

                qDebug()<<"carattere ricevuto lato client: "<<operazione.character.getValue()<<"Con site id :"<<operazione.getSiteId()<<"\n";

                std::cout<<"Esito operazione dal server: "<<operazione.character.getValue().toLatin1()<<std::flush;

                emit  SigEsitoOpDocLocale("Success",operazione);
            }
            else
            {
                BlockReader(socketConnesso).stream() >> operazione;

                qDebug()<<"carattere ricevuto lato client: "<<operazione.character.getValue()<<"Con site id :"<<operazione.getSiteId()<<"\n";

                emit  SigEsitoOpDocLocale("Failed",operazione);
            }
        }

        if(strcmp(msg,"e_r")==0)
        {
            in.readBytes(opt, prova);

            if (strcmp(opt,"r_a")==0) emit SigEsitoRegistrazione("Success");

            else  emit SigEsitoRegistrazione("Failed");
        }

        if (strcmp(msg,"c_i")==0)
        {
            QList <QUser> utenti;

            BlockReader(socketConnesso).stream() >> utenti;

            emit  SigEsitoOpChiHaInseritoCosa(utenti);
        }

        if (strcmp(msg,"ucd")==0)
        {
            QUser utente;

            BlockReader(socketConnesso).stream() >> utente;

            emit  SigQuestoUserHaChiusoIlDoc(utente);
        }

        if (strcmp(msg,"uad")==0)
        {
            QUser utente;

            BlockReader(socketConnesso).stream() >> utente;

            emit  SigQuestoUserHaApertoIlDoc(utente);
        }
    }
}

void WorkerSocketClient::opDocLocale(DocOperation operazione)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("opd",len);

    BlockWriter(socketConnesso).stream()<< operazione;
}

void WorkerSocketClient::apriDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("ope",len);

    BlockWriter(socketConnesso).stream() << nomeFile;
}

void WorkerSocketClient::creaDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("cre",len);

    BlockWriter(socketConnesso).stream() << nomeFile;
}

void WorkerSocketClient::login(QUtente user)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("log",len);

    BlockWriter(socketConnesso).stream() << user;
}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("mod",len);

    BlockWriter(socketConnesso).stream() << user1;
}

void WorkerSocketClient::registrazione(QUtente user)
{
    char fullpath[100];
    char name[100];

    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("reg",len);

/*
    GetFullPathName(user.getNomeImg(), 100, fullpath, nullptr);
    QImage image(fullpath);
    QBuffer buffer;
    image.save(&buffer, "PNG");
    in << buffer.size()
    in << buffer.data();*/

    qDebug()<< "inizio trasmissione";

    BlockWriter(socketConnesso).stream() << user;
}

void WorkerSocketClient::chiudiDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("c_d",len);
}

void WorkerSocketClient::opChiHaInseritoCosa()
{
    QDataStream in(this->socketConnesso);

    uint len=3;

    in.writeBytes("c_i",len);
}
