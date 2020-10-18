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
    qRegisterMetaType<QList<DocOperation>>();

    this->socketConnesso = new QTcpSocket(this);
    connect(socketConnesso,&QTcpSocket::connected, this, &WorkerSocketClient::socketConnected);
    this->socketConnesso->connectToHost(QHostAddress(ipAddr), 3030 );
}

void WorkerSocketClient::socketConnected(){
    connected = (socketConnesso->state() == QTcpSocket::ConnectedState);
    if(connected==true)
        emit SigEsitoConnessioneAlServer("Success");
    else  emit SigEsitoConnessioneAlServer("Failed");
    socketConnesso->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(socketConnesso, &QTcpSocket::readyRead, this,  &WorkerSocketClient::leggiMsgApp,Qt::QueuedConnection);
}

WorkerSocketClient::~WorkerSocketClient(){
    if(connected == true)
        this->socketConnesso->disconnectFromHost();
    delete socketConnesso;
    delete currentImg;
}






/* -------------------------------- INPUT OPERATION ------------------------------------------*/
void WorkerSocketClient::EmitSigEsitoApriDoc( QDataStream& in){
    char* controllo;
    uint len_controllo = 3;
    in.setByteOrder(QDataStream::BigEndian);

    in.readBytes(controllo, len_controllo);
    if (strcmp(controllo,"ope")==0)
    {
        CRDT doc;
        in >> doc;
        emit SigEsitoApriDoc("Success", doc);
     }
    else
    {
        emit SigEsitoApriDoc("Failed", CRDT());
    }
}

void WorkerSocketClient::EmitSigEsitoCreaDoc(QDataStream& in){
    char* controllo;
    uint len_controllo = 3;

    in.readBytes(controllo,len_controllo);
    if (strcmp(controllo,"crt")==0){
      CRDT doc;
      in >> doc;
      emit SigEsitoCreaDoc( "Success", doc);
   }
   else{
      CRDT doc;
      if (!in.commitTransaction())
             return;
      emit SigEsitoCreaDoc("Failed", doc);
   }
}

void WorkerSocketClient::EmitSigEsitoLogin(QDataStream& in){
    char* controllo;
    uint len_controllo = 3;

    in.readBytes(controllo, len_controllo);
    if(strcmp(controllo,"suc")==0){
        QList<QString> nomiFilesEditati;
        QUtente user;
        qint64 dimension;
        QByteArray data;
        QImage* image = new QImage();
        QString val= QString("/");

        in >> user;
        in >> dimension;
        in >> data;
        std::cout<< "dentro";
        if(user.getNomeImg() != NULL){
              image->loadFromData(data,user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
              std::cout<< QDir::currentPath().toStdString()+val.toStdString()+user.getNomeImg().toStdString();
              std::cout<<image->save(QDir::currentPath()+val+user.getNomeImg(), user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
              this->currentImg=image;
        }
        in >> nomiFilesEditati;
        this->user = user;
        if (!in.commitTransaction())
               return;
        emit SigEsitoLogin( "Success"/*esito*/, user, nomiFilesEditati);
    }
    else
    {
        QList <QString> nomiFilesEditati;
        QUtente user;
        if (!in.commitTransaction())
               return;
        emit SigEsitoLogin("Failed", user, nomiFilesEditati);
    }
}
void WorkerSocketClient::EmitSigEsitoModificaProfiloUtente( QDataStream& in){
    char* controllo;
    uint len_controllo = 3;
    in.readBytes(controllo, len_controllo);

    QString val= QString("/");
    if (strcmp(controllo,"suc")==0)
    {
        QUtente userNew;
        qint64 dimension;
        QByteArray data;
        QImage* image = new QImage();
        QString val= QString("/");

        in >> userNew;
        in >> dimension;
        in >> data;
        if (!in.commitTransaction())
               return;
        this->user=userNew;
        if(dimension>0){
        image->loadFromData(data,this->user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        image->save(QDir::currentPath()+val+user.getNomeImg(), user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        this->currentImg=image;
        }

        emit SigEsitoModificaProfiloUtente("Success",userNew);
    }
    else
    {
        QUtente user;
        emit SigEsitoModificaProfiloUtente("Failed", user);
    }
}

void WorkerSocketClient::EmitEsitoChiusuraDocClient( QDataStream& in){
    QString esito;
    in >> esito;
    if (!in.commitTransaction())
           return;
    if (esito.compare("Success")==0)
    {emit SigEsitoChiudiDoc("Success");}
    else {emit SigEsitoChiudiDoc("Failed");}
}

void WorkerSocketClient::EmitSigOpDoc( QDataStream& in){
    char* controllo;
    uint len_controllo = 3;

    in.readBytes(controllo, len_controllo);
    DocOperation operazione;
    in >> operazione;
    if (!in.commitTransaction())
           return;
   if(this->user.getUserId()!=operazione.getSiteId()){
       if (bufferDimension!=0){
           opList.append(operazione);
           numOpTreated++;
           if(numOpTreated>=bufferDimension){
               numOpTreated = 0;
               bufferDimension = 0;

               emit SigOpDocRemotaBuffered(opList);
               opList.clear();
           }
       }else
            emit SigOpDocRemota(operazione);
    }
    else {
                                //TODO: DA GESTIRE CON ECCEZIONE
        qDebug()<<"Errore!! Non dovrei mai ricevere indietro la mia operazione!!";
    }
}

void WorkerSocketClient::EmitSigEsitoRegistrazione(QDataStream& in){
    char* controllo;
    uint len_controllo = 3;
    in.readBytes(controllo, len_controllo);
    if (strcmp(controllo,"r_a")==0){
        if (!in.commitTransaction())
               return;
        emit SigEsitoRegistrazione("Success");}
    else{
        if (!in.commitTransaction())
               return;
        emit SigEsitoRegistrazione("Failed");
    }
}

void WorkerSocketClient::EmitSigEsitoColorMode(QDataStream& in){
    QList <QUser> utenti;
    in >> utenti;
    if (!in.commitTransaction())
           return;
    emit  SigEsitoOpChiHaInseritoCosa(utenti);
}

void WorkerSocketClient::EmitSigUserChiudiDoc(QDataStream& in){
    QUser utente;
    in >> utente;
    if (!in.commitTransaction())
           return;
    emit  SigQuestoUserHaChiusoIlDoc(utente);
}

void WorkerSocketClient::EmitSigUserApriDoc(QDataStream& in){
    QUser utente;
    in >> utente;
    if (!in.commitTransaction())
           return;
    emit  SigQuestoUserHaApertoIlDoc(utente);

}

void WorkerSocketClient::IniziaLetturaBuffered(QDataStream& in){
    int bufferDim;
    in >> bufferDim;
    bufferDimension = bufferDim;
    if (!in.commitTransaction())
           return;
}



void  WorkerSocketClient::leggiMsgApp(){
        quint16 inOperation;
        QDataStream in(this->socketConnesso);
        in.setByteOrder(QDataStream::BigEndian);
        while(socketConnesso->bytesAvailable()){
        in.startTransaction();

        in >> inOperation;
        switch(inOperation){
        case Esito_apri_doc:{EmitSigEsitoApriDoc(in); break;}
        case Esito_crea_doc:{EmitSigEsitoCreaDoc(in); break;}
        case Esito_login:{EmitSigEsitoLogin(in); break;}
        case Esito_modifica_profilo_utente:{EmitSigEsitoModificaProfiloUtente(in); break;}
        case Esito_operazione_doc:{EmitSigOpDoc(in); break;}
        case Esito_registrazione:{EmitSigEsitoRegistrazione(in); break;}
        case Esito_operazione_colorMode:{EmitSigEsitoColorMode(in); break;}
        case User_chiudi_doc:{EmitSigUserChiudiDoc(in); break;}
        case User_apri_doc:{EmitSigUserApriDoc(in); break;}
        case Esito_chiusura_doc_client:{EmitEsitoChiusuraDocClient(in); break;}
        case Lettura_buffered:{IniziaLetturaBuffered(in); break;}

        }


    }
}








/*   ------------------------ OUTPUT OPERATION ---------------------------------  */

void WorkerSocketClient::opDocLocaleBuffered(QList<DocOperation> opList){
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out<<Leggi_buffered;
    out << opList.size();
    this->socketConnesso->write(block);
    while(!opList.isEmpty()){
        DocOperation op = opList.first();
        opList.pop_front();
        opDocLocale(op);
    }

}

void WorkerSocketClient::opDocLocale(DocOperation operazione)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << Operazione_doc;
    out << operazione;
    this->socketConnesso->write(block);
}

void WorkerSocketClient::apriDoc(QString nomeFile)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << Apri_doc;
    out << nomeFile;
    this->socketConnesso->write(block);
}

void WorkerSocketClient::creaDoc(QString nomeFile)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << Crea_doc;
    out << nomeFile;
    this->socketConnesso->write(block);

}

void WorkerSocketClient::login(QUtente user)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << Login;
    out << user;
    qDebug()<<this->socketConnesso->write(block);

}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    QByteArray arr;
    QByteArray arr1;
    QBuffer buffer(&arr);
    QBuffer buffer1(&arr1);
    QString val= QString("/");
    QImage *image1 = new QImage();
    if(user1.getNomeImg() != NULL){
        image1->load(user1.getNomeImg());
        buffer1.open(QIODevice::WriteOnly);
        image1->save(&buffer1, user1.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
    }

    if(this->currentImg != NULL ){
        buffer.open(QIODevice::WriteOnly);
        this->currentImg->save(&buffer,this->user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
        if(arr.compare(arr1) == 0){
             arr1= QByteArray();
        }
    }
    out << Modifica_profilo_utente;
    out << user1;
    out << static_cast<qint64>(arr1.size());
    out << arr1;
    this->socketConnesso->write(block);
}

void WorkerSocketClient::registrazione(QUtente user)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    QByteArray arr;
    QBuffer buffer(&arr);
    QImage image;
    if(user.getNomeImg()!= nullptr)
    {
        image.load(user.getNomeImg());
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
     }
    out << Registrazione;
    out << user;
    out << static_cast<qint64>(arr.size());
    out << arr;
    this->socketConnesso->write(block);

}

void WorkerSocketClient::chiudiDoc(QString nomeFile)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << Chiusura_doc_client;
    this->socketConnesso->write(block);

}

void WorkerSocketClient::opChiHaInseritoCosa()
{

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << Color_mode;
    this->socketConnesso->write(block);

}
