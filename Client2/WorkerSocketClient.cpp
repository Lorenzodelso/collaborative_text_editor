#include "WorkerSocketClient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QImage>
#include <QDebug>
#include <QDir>

void WorkerSocketClient::connessioneAlServer(QString ipAddr) {

    qRegisterMetaType<QUtente>();
    qRegisterMetaType<QUser>();
    qRegisterMetaType<QList<QString>>();
    qRegisterMetaType<QList<QUser>>();
    qRegisterMetaType<CRDT>();
    qRegisterMetaType<DocOperation>();

    this->socketConnesso = new QTcpSocket(this);
    connect(socketConnesso,&QTcpSocket::connected, this, &WorkerSocketClient::socketConnected);
    this->socketConnesso->connectToHost(QHostAddress(ipAddr), 3030 );
}

void WorkerSocketClient::socketConnected(){
    connected = (socketConnesso->state() == QTcpSocket::ConnectedState);
    if(connected==true)
        emit SigEsitoConnessioneAlServer("Success");
    else  emit SigEsitoConnessioneAlServer("Failed");
    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp,Qt::QueuedConnection);
}

WorkerSocketClient::~WorkerSocketClient(){
    if(connected == true)
        this->socketConnesso->disconnectFromHost();
    delete socketConnesso;
    delete currentImg;
}

void  WorkerSocketClient::leggiMsgApp(){
    while(socketConnesso->bytesAvailable()){
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
                /*debug*/
                qDebug()<<"esito apri doc: Success";
                /*debug*/
                emit SigEsitoApriDoc("Success", doc);
             }
            else
            {
                emit SigEsitoApriDoc("Failed", CRDT());
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
              CRDT doc;
              emit SigEsitoCreaDoc("Failed", doc);
           }
        }
        if(strcmp(msg,"e_l")==0)
        {
            in.readBytes(opt, prova);
            if(strcmp(opt,"suc")==0)
            {
                QList<QString> nomiFilesEditati;
                QUtente user;
                qint64 dimension;
                QByteArray data;
                QImage* image = new QImage();
                QString val= QString("/");

                BlockReader(socketConnesso).stream() >> user;
                BlockReader(socketConnesso).stream() >> dimension;
                BlockReader(socketConnesso).stream() >> data;
                std::cout<< "dentro";
                if(user.getNomeImg() != NULL){
                      image->loadFromData(data,user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
                      std::cout<< QDir::currentPath().toStdString()+val.toStdString()+user.getNomeImg().toStdString();
                      std::cout<<image->save(QDir::currentPath()+val+user.getNomeImg(), user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
                      this->currentImg=image;
                }

                BlockReader(socketConnesso).stream() >> nomiFilesEditati;
                this->user = user;
                emit SigEsitoLogin( "Success"/*esito*/, user, nomiFilesEditati);
            }

            else
            {
                QList <QString> nomiFilesEditati;
                QUtente user;
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
            QString esito;
            BlockReader(socketConnesso).stream() >> esito;
            //in.readBytes(opt,prova);
            if (esito.compare("Success")==0)
            {emit SigEsitoChiudiDoc("Success");}
            else {emit SigEsitoChiudiDoc("Failed");}

        }
        if (strcmp(msg,"mop")==0)
        {

            in.readBytes(opt, prova);
            QString val= QString("/");
            if (strcmp(opt,"suc")==0)
            {
                QUtente userNew;
                qint64 dimension;
                QByteArray data;
                QImage* image = new QImage();
                QString val= QString("/");

                BlockReader(socketConnesso).stream() >> userNew;
                BlockReader(socketConnesso).stream() >> dimension;
                BlockReader(socketConnesso).stream() >> data;

                this->user=userNew;

                image->loadFromData(data,this->user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
                image->save(QDir::currentPath()+val+user.getNomeImg(), user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
                this->currentImg=image;

                emit SigEsitoModificaProfiloUtente("Success",userNew);
            }
            else
            {
                QUtente user;
                emit SigEsitoModificaProfiloUtente("Failed", user);
            }
        }
        if (strcmp(msg,"e_o")==0)
        {
            uint len =3;
            in.readBytes(opt, prova);
            DocOperation operazione;
            BlockReader(socketConnesso).stream() >> operazione;
           if(this->user.getUserId()!=operazione.getSiteId()){
               //in.writeBytes("opd",len);
               //BlockWriter(socketConnesso).stream()<< operazione;
               emit SigOpDocRemota(operazione);
            }
            else {
                if (strcmp(opt,"suc")==0)
                    emit SigEsitoOpDocLocale("Success",operazione);
                else
                    emit SigEsitoOpDocLocale("Failed",operazione);
            }
        }

        if(strcmp(msg,"e_r")==0)
        {
            in.readBytes(opt, prova);
            if (strcmp(opt,"r_a")==0){ emit SigEsitoRegistrazione("Success");}
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
        delete [] msg;
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

    QByteArray arr;
    QByteArray arr1;
    QBuffer buffer(&arr);
    QBuffer buffer1(&arr1);
    QString val= QString("/");
    QImage image1;

    image1.load(user1.getNomeImg());
    buffer1.open(QIODevice::WriteOnly);
    image1.save(&buffer1, user1.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());

    if(this->currentImg != NULL ){
        buffer.open(QIODevice::WriteOnly);
        this->currentImg->save(&buffer,this->user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        if(QString(arr).compare(QString(arr1)) == 0){
             arr1= QByteArray();
        }
    }
    BlockWriter(socketConnesso).stream() << user1;
    BlockWriter(socketConnesso).stream() << static_cast<qint64>(arr1.size());
    BlockWriter(socketConnesso).stream() << arr1;
}

void WorkerSocketClient::registrazione(QUtente user)
{
    QDataStream in(this->socketConnesso);
    QByteArray arr;
    QBuffer buffer(&arr);
    uint len=3;
    in.writeBytes("reg",len);
    QImage image;

    if(user.getNomeImg()!= nullptr)
    {
        image.load(user.getNomeImg());
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
     }

    BlockWriter(socketConnesso).stream() << user;
    BlockWriter(socketConnesso).stream() << static_cast<qint64>(arr.size());
    BlockWriter(socketConnesso).stream() << arr;
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
