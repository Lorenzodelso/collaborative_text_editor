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


class CRDT
{

private:
    quint16 siteID;
    QVector<Char> listChar;

    quint16 strategy;
    quint16 boundary;
    //int counter;
    quint16 base;

public:
    QString text;
    quint16 counter;

    CRDT();
    CRDT(quint16 id);
    CRDT(quint16 id, QVector<Char> listChar);
    quint16 getSiteID();
    void setSiteID(quint16 ID);
    QVector<Char> getListChar();
    void setListChar(QVector<Char> listChar);
    DocOperation localInsert(QChar value,QTextCharFormat format, quint16 index);
    DocOperation localErase(quint16 index);
    DocOperation localFormatChange(QTextCharFormat format, quint16 index);
    void remoteInsert(Char value);
    void remoteDelete(Char value);
    void remoteFormatChange(Char ch);
    friend QDataStream& operator<<(QDataStream& out,const CRDT& crdt);
    friend QDataStream& operator>>(QDataStream& in, CRDT& crdt);
    void saveCRDTonFile(QString nomeFile);
    void readCRDTfromFile(QString nomeFile);

protected:
    Char* generateChar(QChar value,QTextCharFormat format, quint16 index);
    QVector<quint16> findPosBefore(quint16 index);
    QVector<quint16> findPosAfter(quint16 index);
    QVector<quint16> generatePosBetween(QVector<quint16> pos1, QVector<quint16> pos2,QVector<quint16>& newPos,quint16 level);
    quint16 generateIdBetween(quint16 id1, quint16 id2, quint16 strategy);
    quint16 retrieveStrategy(quint16 level);
    quint16 findInsertIndex(Char ch);
    quint16 findIndexByPosition(Char ch);


};

Q_DECLARE_METATYPE(CRDT)


#endif // CRDT_H
