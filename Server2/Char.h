#ifndef CHAR_H
#define CHAR_H


#include <QVector>
#include <QChar>
#include <QDataStream>
#include <QTextCharFormat>
//#include "Identifier.h"

class Char {

private:
    quint16 siteId;
    quint16 counter;
    QVector<quint32> position;
    QChar value;
    QTextCharFormat format;
    quint16 alignementType;

public:
    Char();
    Char(quint16 siteId, quint16 counter, QVector<quint32> position, QChar value,QTextCharFormat format);
    Char(const Char& source);
    int compareTo(Char other);
    QVector<quint32> getPos();
    QChar getValue();
    void setFormat(QTextCharFormat format);
    QTextCharFormat getFormat();
    quint16 getSiteId();
    quint16 getAlign();
    void setAlign(quint16 alignementType);
    friend QDataStream& operator<<(QDataStream& out,const Char& ch);
    friend QDataStream& operator>>(QDataStream& in, Char& ch);

};



#endif // CHAR_H
