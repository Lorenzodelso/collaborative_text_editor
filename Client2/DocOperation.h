#ifndef DOCOPERATION_H
#define DOCOPERATION_H


#include <QTypeInfo>
#include "Char.h"
#include <iostream>

/* ENUMERAZIONE UTILE PER DEFINIRE IN MANIERA SIGNIFICATIVA PER L'UTENTE L'OPERAZIONE.
 INCLUDENDO IL FILE SI POSSONO USARE remoteInsert o remoteDelete AL POSTO DEGLI QUINT16 PER UN CODICE PIU' CHIARO.  */
enum typeList :quint16{
    remoteInsert = 5,
    remoteDelete = 1,
    changedFormat = 2,
    cursorMoved = 3,
    alignementChanged=4
};

class DocOperation
{
public:
    quint16 type;
    Char character;
    QTextCharFormat oldFormat; //serve per operazioni di UNDO

    //Per operazioni su movimento cursore
    quint16 siteId;
    quint16 cursorPos;
    quint16 cursorAnch;

    quint16 alignementType;

    DocOperation();
    DocOperation(quint16 type, Char ch, QTextCharFormat oldFormat,quint16 siteId, quint16 cursorPos, quint16 cursorAnch);
    DocOperation(quint16 cursorPos, quint16 alignementType,quint16 siteId);
    void printOperation();
    quint16 getSiteId();

    friend QDataStream& operator<<(QDataStream& out, DocOperation docOp);
    friend QDataStream& operator>>(QDataStream& in, DocOperation& dopOp);

};
Q_DECLARE_METATYPE(DocOperation)

#endif // DOCOPERATION_H
