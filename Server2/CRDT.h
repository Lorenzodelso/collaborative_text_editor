#ifndef CRDT_H
#define CRDT_H

#define RANDOM_STRATEGY 0
#define PLUS_STRATEGY 1
#define MINUS_STRATEGY 2

#include "Char.h"
#include "DocOperation.h"
#include <math.h>
#include <QString>
#include <QFile>
#include <QRandomGenerator>
#define DEFAULT_SITEID 0

enum opType :quint16{
    localInsertType = 5,
    localDeleteType = 1,
    localChangedFormatType = 2,
    localCursorMovedType = 3,
    localAlignementChangedType=4
};

class CRDT
{

private:
    quint16 siteID;
    QVector<Char> listChar;
    quint16 strategy;
    quint16 boundary;
    quint16 base;

public:
    QString text;
    quint16 counter;
    quint16 alignement;
    CRDT();

    CRDT(quint16 id);
    CRDT(quint16 id, QVector<Char> listChar);
    quint16 getSiteID();
    void setSiteID(quint16 ID);
    QVector<Char> getListChar();
    void setListChar(QVector<Char> listChar);
    DocOperation localInsert(QChar value,QTextCharFormat format, quint32 index);
    DocOperation localErase(quint32 index);
    DocOperation localFormatChange(QTextCharFormat format, quint32 index);
    quint32 remoteInsert(Char value);
    quint32 remoteDelete(Char value);
    quint32 remoteFormatChange(Char ch);
    friend QDataStream& operator<<(QDataStream& out,const CRDT& crdt);
    friend QDataStream& operator>>(QDataStream& in, CRDT& crdt);
    void saveCRDTonFile(QString nomeFile);
    void readCRDTfromFile(QString nomeFile);
    void setCharAlign(quint16 alignementType,quint32 index);


protected:
    Char generateChar(QChar value,QTextCharFormat format, quint32 index);
    QVector<quint32> findPosBefore(quint32 index);
    QVector<quint32> findPosAfter(quint32 index);
    QVector<quint32> generatePosBetween(QVector<quint32> pos1, QVector<quint32> pos2,QVector<quint32>& newPos,quint16 level);
    quint32 generateIdBetween(quint32 id1, quint32 id2, quint16 strategy);
    quint16 retrieveStrategy(quint16 level);
    int findInsertIndex(Char ch);
    int findIndexByPosition(Char ch);


};

Q_DECLARE_METATYPE(CRDT)


#endif // CRDT_H
