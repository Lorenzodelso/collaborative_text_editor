//
// Created by Lorenzo Del Sordo on 2019-10-02.
//

#include <iostream>
#include "CRDT.h"

CRDT::CRDT(){

}

CRDT::CRDT(quint16 id): siteID(id), strategy(RANDOM_STRATEGY),counter(0),boundary(10),base(32),text(""),alignement(0) {
    this->listChar.clear();
    QRandomGenerator* rand = new QRandomGenerator();
}
CRDT::CRDT(quint16 id, QVector<Char> listChar): siteID(id), strategy(RANDOM_STRATEGY),counter(0),boundary(10),base(32),alignement(0){
    this->listChar = listChar;
    this->text.clear();
    for(Char ch : this->listChar){
        QChar c = ch.getValue();
        this->text.append(c);
    }
    QRandomGenerator* rand = new QRandomGenerator();
}
quint16 CRDT::getSiteID(){return this->siteID;}
void CRDT::setSiteID(quint16 ID){this->siteID = ID;}
QVector<Char> CRDT::getListChar(){return this->listChar;}
void CRDT::setListChar(QVector<Char> listChar){this->listChar = listChar;}

QVector<quint16> CRDT::findPosBefore(quint16 index){
    QVector<quint16> pos;
    if (index != 0){
        pos = this->listChar[index-1].getPos();
    }else{
        pos.clear();
    }
    return pos;
}

QVector<quint16> CRDT::findPosAfter(quint16 index) {
    QVector<quint16> pos;
    auto len = this->listChar.size();
    if (len < index){
        std::cout<< "Index error";
    }else{
        if (len==index){
            pos.clear();
        }else {
            pos = this->listChar[index].getPos();
        }
    }
    return pos;
}

quint16 CRDT::retrieveStrategy(quint16 level) {
    quint16 returnStrategy;
    switch (this->strategy){
        case RANDOM_STRATEGY:
        {
            quint16 num = quint16( rand->bounded(0,1) );
            returnStrategy = num==0 ? PLUS_STRATEGY : MINUS_STRATEGY;
            break;
        }
        default:
            returnStrategy = this->strategy;
            break;
    }
    return returnStrategy;
}

quint16 CRDT::generateIdBetween(quint16 id1, quint16 id2, quint16 strategy) {
    auto difference =id2-id1;
    if ((difference) < this->boundary) {
        id1 = id1 + 1;
    } else {
        if (strategy == MINUS_STRATEGY) {
            id1 = id2 - this->boundary;
        } else {
            id1 = id1 + 1;
            id2 = id1 + this->boundary;
        }
    }
    //return floor(random()%(id2-id1))+id1;
    quint16 num = quint16( rand->bounded(id1,id2) );
    return num;
}


QVector<quint16> CRDT::generatePosBetween(QVector<quint16> pos1, QVector<quint16> pos2, QVector<quint16>& newPos,quint16 level) {
    quint16 strategy = retrieveStrategy(level);
    quint16 id1;
    quint16 id2;
    auto base = pow(2, level)*this->base;

    if (!pos1.isEmpty()) {
        id1 = pos1[0];
    }else{
        id1 = 0;
    }
    if (!pos2.isEmpty()) {
        id2 = pos2[0];
    }else{
        id2 = base;
    }
    auto difference = id2 - id1;

    if (difference > 1){
        quint16 newDigit = generateIdBetween(id1,id2,strategy);
        newPos.push_back(newDigit);
        return newPos;
    } else if (difference == 1){
        newPos.push_back(id1);
        if(!pos1.empty())
            pos1.pop_front();
        else pos1.clear();
        pos2.clear();
        return this->generatePosBetween(pos1, pos2, newPos,level+1);
    }else if(difference == 0){
        newPos.push_back(id1);
        if(!pos1.empty())
            pos1.pop_front();
        else pos1.clear();
        if(!pos2.empty())
            pos2.pop_front();
        else pos2.clear();
        return this->generatePosBetween(pos1, pos2, newPos,level+1);
    }
}

Char* CRDT::generateChar(QChar value,QTextCharFormat format,quint16 index){
    QVector<quint16> pos1 = findPosBefore(index);
    QVector<quint16> pos2 = findPosAfter(index);
    QVector<quint16> newPos;
    newPos.clear();
    QVector<quint16> newPos1 = generatePosBetween(pos1,pos2,newPos,0);

    return new Char(this->siteID,this->counter++,newPos1,value,format);
}

int CRDT::findInsertIndex(Char ch) {
    quint16 left = 0;
    quint16 right = this->listChar.size()-1;
    quint16 mid;
    int compareNum;

    if((this->listChar.size()==0) || (ch.compareTo(this->listChar[left]) == -1)){
        return left;
    }else if(ch.compareTo(this->listChar[right])>0){
        return right+1;
    }

    while (left + 1 < right) {
        mid = floor(left + (right - left) / 2);
        compareNum = ch.compareTo(this->listChar[mid]);

        if (compareNum == 0) {
            return mid;
        } else if (compareNum > 0) {
            left = mid;
        } else {
            right = mid;
        }
    }

    return ch.compareTo(this->listChar[left]) == 0 ? left : right;
}

int CRDT::findIndexByPosition(Char ch) {
    quint16 left = 0;
    quint16 right = this->listChar.size() - 1;
    quint16 mid;
    int compareNum;

    if (this->listChar.size()==0) {
        std::cout<<"Character does not exist in CRDT";
    }

    while (left + 1 < right) {
        mid = floor(left + (right - left) / 2);
        compareNum = ch.compareTo(this->listChar[mid]);

        if (compareNum == 0) {
            return mid;
        } else if (compareNum > 0) {
            left = mid;
        } else {
            right = mid;
        }
    }

    if (ch.compareTo(this->listChar[left]) == 0) {
        return left;
    } else if (ch.compareTo(this->listChar[right]) == 0) {
        return right;
    } else {
        std::cout<<"Character does not exist in CRDT.";
    }
}


DocOperation CRDT::localInsert(QChar value, QTextCharFormat format, quint16 index) {
    Char* QCharacter = generateChar(value,format,index);
    Char Qc = *QCharacter;

    //Per gestire i casi di cambiamento di allineamento quando ancora non c'è scritto nulla
    if(alignement!=0){
        Qc.setAlign(alignement);
        alignement=0;
    }
    this->listChar.insert(index,Qc);
    this->text.insert(index, QCharacter->getValue());

    QTextCharFormat* oldFormat = new QTextCharFormat();
    oldFormat->setFontFamily("fontFamily");
    oldFormat->setFontItalic(true);
    oldFormat->setFontWeight(777);
    oldFormat->setFontUnderline(false);
    oldFormat->setFontPointSize(21.1);
    DocOperation* docOp = new DocOperation(0,Qc,QTextCharFormat(),this->siteID,0,0);
    return *docOp;
}

DocOperation CRDT::localErase(quint16 index) {
    Char Qc = listChar[index];
    this->listChar.remove(index);
    this->text.remove(index,1);
    //Mi occupo qui di segnalare al WorkerSocket dell'operazione
    DocOperation* docOp = new DocOperation(1,Qc,QTextCharFormat(),this->siteID,0,0);
    return *docOp;
}

quint16 CRDT::remoteInsert(Char value) {
    quint16 index = findInsertIndex(value);
    this->listChar.insert(index,value);
    this->text.insert(index, value.getValue());
    return index;
}

quint16 CRDT::remoteDelete(Char value) {
    quint16 index = findIndexByPosition(value);
    this->listChar.remove(index);
    this->text.remove(index,1);
    return index;
}

QDataStream& operator<<(QDataStream& out,const CRDT& crdt){
    //Overloading dell'operatore per permettere la serializzazione dell'oggetto CRDT
    //Usato nel salvataggio dell'oggetto in memoria nel momento in cui devo salvare il file
    out << crdt.siteID << crdt.listChar;
    return out;
}

QDataStream& operator>>(QDataStream& in, CRDT& crdt){
    //Overloading dell'operatore per permettere la serializzazione dell'oggetto CRDT
    //Usato nel caricamento dell'oggetto in memoria nel momento in cui devo caricare le informazioni nella struttura dati
   quint16 siteID;
   QVector<Char> listChar;

   in >> siteID;
   in >> listChar;
   crdt = *new CRDT(siteID,listChar);
   return in;
}

void CRDT::saveCRDTonFile(QString nomeFile){
    //Funzione usata per salvare la struttura CRDT su file
    QFile file(nomeFile);
    QDataStream qstream(&file);
    file.open(QIODevice::WriteOnly);
    qstream << *this;
    file.close();
}

void CRDT::readCRDTfromFile(QString nomeFile){
    //Funzione usata per leggere la struttura dati salvata su file del CRDT
    //Entrambe le funzioni si occupano di gestire i file (apertura e chiusura) prima e dopo le operazioni
    QFile file(nomeFile);
    QDataStream instream(&file);
    file.open(QIODevice::ReadOnly);
    instream >> *this;
    file.close();
}

DocOperation CRDT::localFormatChange(QTextCharFormat format, quint16 index){
    QTextCharFormat oldFormat = listChar[index].getFormat();
    this->listChar[index].setFormat(format);
    DocOperation* docOp = new DocOperation(2,listChar[index],oldFormat,this->siteID,0,0);
    return *docOp;
}

quint16 CRDT::remoteFormatChange(Char ch){
    quint16 index = findInsertIndex(ch);
    this->listChar[index].setFormat(ch.getFormat());
    return index;
}

void CRDT::setCharAlign(quint16 alignementType, quint16 index){
    if(this->listChar.length()>index)
        this->listChar[index].setAlign(alignementType);
    else{
        alignement = alignementType;
    }
}




