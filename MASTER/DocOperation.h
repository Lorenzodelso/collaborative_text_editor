#ifndef DOCOPERATION_H
#define DOCOPERATION_H


#include <QTypeInfo>
#include "Char.h"
#include <iostream>

/* ENUMERAZIONE UTILE PER DEFINIRE IN MANIERA SIGNIFICATIVA PER L'UTENTE L'OPERAZIONE.
 INCLUDENDO IL FILE SI POSSONO USARE remoteInsert o remoteDelete AL POSTO DEGLI QUINT16 PER UN CODICE PIU' CHIARO.  */
enum typeList :quint16{
    remoteInsert = 0,
    remoteDelete = 1,
    changedFormat = 2,
    cursorMoved = 3
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

    DocOperation();
    DocOperation(quint16 type, Char ch, QTextCharFormat oldFormat,quint16 siteId, quint16 cursorPos, quint16 cursorAnch);
    void printOperation();

};

#endif // DOCOPERATION_H
