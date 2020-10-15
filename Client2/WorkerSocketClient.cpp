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
void WorkerSocketClient::EmitSigEsitoApriDoc(){
    char* controllo;
    BlockReader(socketConnesso).stream() >>controllo;
    if (strcmp(controllo,"ope")==0)
    {
        CRDT doc;
        BlockReader(socketConnesso).stream() >> doc;
        emit SigEsitoApriDoc("Success", doc);
     }
    else
    {
        emit SigEsitoApriDoc("Failed", CRDT());
    }
}

void WorkerSocketClient::EmitSigEsitoCreaDoc(){
    char*controllo;
    BlockReader(socketConnesso).stream() >>controllo;
    if (strcmp(controllo,"crt")==0){
      CRDT doc;
      BlockReader(socketConnesso).stream() >> doc;
      emit SigEsitoCreaDoc( "Success", doc);
   }
   else{
      CRDT doc;
      emit SigEsitoCreaDoc("Failed", doc);
   }
}

void WorkerSocketClient::EmitSigEsitoLogin(){
    char* controllo;
    uint len_controllo = 3;
    QDataStream in(this->socketConnesso);
     BlockReader(socketConnesso).stream() >>controllo;
    if(strcmp(controllo,"suc")==0){
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
void WorkerSocketClient::EmitSigEsitoModificaProfiloUtente(){
    char* controllo;
    uint len_controllo = 3;
    QDataStream in(this->socketConnesso);
     BlockReader(socketConnesso).stream() >>controllo;
    QString val= QString("/");
    if (strcmp(controllo,"suc")==0)
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

void WorkerSocketClient::EmitEsitoChiusuraDocClient(){
    QString esito;
    BlockReader(socketConnesso).stream() >> esito;
    //in.readBytes(opt,prova);
    if (esito.compare("Success")==0)
    {emit SigEsitoChiudiDoc("Success");}
    else {emit SigEsitoChiudiDoc("Failed");}
}

void WorkerSocketClient::EmitSigOpDoc(){
    char* controllo;
    uint len_controllo = 3;
    QDataStream in(this->socketConnesso);
    BlockReader(socketConnesso).stream() >>controllo;
    DocOperation operazione;
    BlockReader(socketConnesso).stream() >> operazione;
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
}

void WorkerSocketClient::EmitSigEsitoRegistrazione(){
    char* controllo;
    uint len_controllo = 3;
    QDataStream in(this->socketConnesso);
     BlockReader(socketConnesso).stream() >>controllo;
    if (strcmp(controllo,"r_a")==0){ emit SigEsitoRegistrazione("Success");}
    else  emit SigEsitoRegistrazione("Failed");
}

void WorkerSocketClient::EmitSigEsitoColorMode(){
    QList <QUser> utenti;
    BlockReader(socketConnesso).stream() >> utenti;
    emit  SigEsitoOpChiHaInseritoCosa(utenti);
}

void WorkerSocketClient::EmitSigUserChiudiDoc(){
    QUser utente;
    BlockReader(socketConnesso).stream() >> utente;
    emit  SigQuestoUserHaChiusoIlDoc(utente);
}

void WorkerSocketClient::EmitSigUserApriDoc(){
    QUser utente;
    BlockReader(socketConnesso).stream() >> utente;
    emit  SigQuestoUserHaApertoIlDoc(utente);
}

void WorkerSocketClient::IniziaLetturaBuffered(){
    int bufferDim;
    BlockReader(socketConnesso).stream() >> bufferDim;
    bufferDimension = bufferDim;
}



void  WorkerSocketClient::leggiMsgApp(){
    while(socketConnesso->bytesAvailable()){
        quint16 inOperation;
        QDataStream in(this->socketConnesso);
        BlockReader(socketConnesso).stream() >> inOperation;
        switch(inOperation){
        case Esito_apri_doc:{EmitSigEsitoApriDoc(); break;}
        case Esito_crea_doc:{EmitSigEsitoCreaDoc(); break;}
        case Esito_login:{EmitSigEsitoLogin(); break;}
        case Esito_modifica_profilo_utente:{EmitSigEsitoModificaProfiloUtente(); break;}
        case Esito_operazione_doc:{EmitSigOpDoc(); break;}
        case Esito_registrazione:{EmitSigEsitoRegistrazione(); break;}
        case Esito_operazione_colorMode:{EmitSigEsitoColorMode(); break;}
        case User_chiudi_doc:{EmitSigUserChiudiDoc(); break;}
        case User_apri_doc:{EmitSigUserApriDoc(); break;}
        case Esito_chiusura_doc_client:{EmitEsitoChiusuraDocClient(); break;}
        case Lettura_buffered:{IniziaLetturaBuffered(); break;}
        }
    }
}








/*   ------------------------ OUTPUT OPERATION ---------------------------------  */

void WorkerSocketClient::opDocLocaleBuffered(QList<DocOperation> opList){
    BlockWriter(socketConnesso).stream() << Leggi_buffered;
    BlockWriter(socketConnesso).stream() << opList.size();
    while(!opList.isEmpty()){
        DocOperation op = opList.first();
        opList.pop_front();
        opDocLocale(op);
    }
}

void WorkerSocketClient::opDocLocale(DocOperation operazione)
{
    QDataStream in(this->socketConnesso);
    BlockWriter(socketConnesso).stream() << Operazione_doc;
    BlockWriter(socketConnesso).stream()<< operazione;
}

void WorkerSocketClient::apriDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);
    BlockWriter(socketConnesso).stream() << Apri_doc;
    BlockWriter(socketConnesso).stream() << nomeFile;
}

void WorkerSocketClient::creaDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);
    BlockWriter(socketConnesso).stream() << Crea_doc;
    BlockWriter(socketConnesso).stream() << nomeFile;
}

void WorkerSocketClient::login(QUtente user)
{
    QDataStream in(this->socketConnesso);
    BlockWriter(socketConnesso).stream() << Login;
    BlockWriter(socketConnesso).stream() << user;

}

void WorkerSocketClient::modificaProfiloUtente(QUtente user1)
{
    QDataStream in(this->socketConnesso);
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
    BlockWriter(socketConnesso).stream() << Modifica_profilo_utente;
    BlockWriter(socketConnesso).stream() << user1;
    BlockWriter(socketConnesso).stream() << static_cast<qint64>(arr1.size());
    BlockWriter(socketConnesso).stream() << arr1;
}

void WorkerSocketClient::registrazione(QUtente user)
{
    QDataStream in(this->socketConnesso);
    QByteArray arr;
    QBuffer buffer(&arr);
    QImage image;
    if(user.getNomeImg()!= nullptr)
    {
        image.load(user.getNomeImg());
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, user.getNomeImg().split('.',QString::SkipEmptyParts)[1].toLocal8Bit().data());
     }
    BlockWriter(socketConnesso).stream() << Registrazione;
    BlockWriter(socketConnesso).stream() << user;
    BlockWriter(socketConnesso).stream() << static_cast<qint64>(arr.size());
    BlockWriter(socketConnesso).stream() << arr;
}

void WorkerSocketClient::chiudiDoc(QString nomeFile)
{
    QDataStream in(this->socketConnesso);
    BlockWriter(socketConnesso).stream() << Chiusura_doc_client;
}

void WorkerSocketClient::opChiHaInseritoCosa()
{
    QDataStream in(this->socketConnesso);
    BlockWriter(socketConnesso).stream() << Color_mode;
}
