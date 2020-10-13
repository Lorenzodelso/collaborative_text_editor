#ifndef WORKERDOCCLIENT_H
#define WORKERDOCCLIENT_H

#include <QObject>
#include "CRDT.h"
#include <QDebug>
#include <QTextDocument>
#include <QTextCursor>

#define BUFFER_DIMENSION 50
#define SIGNAL_LIMIT 200

class WorkerDocClient:public QObject
{
    Q_OBJECT
public:
    WorkerDocClient(CRDT* algoritmoCRDT, quint16 siteId,QMap<quint16,QTextCursor*>* cursorMap);
    CRDT* algoritmoCRDT;
    QTextCharFormat currentEditorFormat;
    quint16 siteId;
    bool colorWriting;
    QMap<quint16,QTextCursor*>* cursorMap;

protected:
    void comunicaCRDTInserimentoLocale(int pos, int numInserted);
    void comunicaCRDTRimozioneLocale(int pos, int numRemoved);

public slots:
    void CRDTInsertRemove(int pos, int rem, int add,QString strAdded,QTextCharFormat format);
    void opDocRemota(DocOperation operation);
    void opDocRemotaBuffered(QList<DocOperation> opList);
    void editorFormatChanged(QTextCharFormat currentFormat);
    void colorWritingStarted();
    void colorWritingEnded();

signals:
    void SigOpDocLocale(DocOperation docOp);
    void SigOpDocLocaleBuffered(QList<DocOperation> opList);
    void SigScriviOpDocRemota(DocOperation docOp);
    void SigScriviOpDocRemotaBuffered(QList<DocOperation> opList);

};


#endif // WORKERDOCCLIENT_H
