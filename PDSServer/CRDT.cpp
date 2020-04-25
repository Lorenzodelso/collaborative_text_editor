//
// Created by Lorenzo Del Sordo on 2019-10-02.
//

#include <iostream>
#include "CRDT.h"

CRDT::CRDT(){

}

CRDT::CRDT(quint16 id): siteID(id), strategy(RANDOM_STRATEGY),counter(0),boundary(10),base(32),text("") {
    this->listChar.clear();
}
CRDT::CRDT(quint16 id, QVector<Char> listChar): siteID(id), strategy(RANDOM_STRATEGY),counter(0),boundary(10),base(32){
    this->listChar = listChar;
    this->text.clear();
    for(Char ch : this->listChar){
        QChar c = ch.getValue();
        this->text.append(c);
    }
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
            returnStrategy = round(std::rand())==0 ? PLUS_STRATEGY : MINUS_STRATEGY;
            break;
        default:
            returnStrategy = this->strategy;
            break;
    }
    return returnStrategy;
}

quint16 CRDT::generateIdBetween(quint16 id1, quint16 id2, quint16 strategy) {
    auto difference =id2-id1 > 0 ? id2-id1 : id1-id2;
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
    QRandomGenerator* rand = new QRandomGenerator();
    return quint16( rand->bounded(difference) )+id1;
}


QVector<quint16> CRDT::generatePosBetween(QVector<quint16> pos1, QVector<quint16> pos2, QVector<quint16>& newPos,quint16 level) {
    quint16 strategy = retrieveStrategy(level);
    quint16 id1;
    quint16 id2;
    auto base = pow(2, level)*this->base;

    if (pos1.size()!=0) {
        id1 = pos1[0];
    }else{
        id1 = 0;
    }
    if (pos2.size()!=0) {
        id2 = pos2[0];
    }else{
        id2 = base;
    }
    auto difference = id2 - id1 > 0 ? id2-id1 : id1-id2;

    if (difference > 1){
        quint16 newDigit = generateIdBetween(id1,id2,strategy);
        newPos.push_back(newDigit);
        return newPos;
    } else if (difference == 1){
        newPos.push_back(id1);
        pos1.remove(0);
        pos2.clear();
        return this->generatePosBetween(pos1, pos2, newPos,level+1);
    }else if(difference == 0){
        newPos.push_back(id1);
        pos1.remove(0);
        pos2.remove(0);
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

quint16 CRDT::findInsertIndex(Char ch) {
    quint16 left = 0;
    quint16 right = this->listChar.size()-1;
    quint16 mid;
    quint16 compareNum;

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

quint16 CRDT::findIndexByPosition(Char ch) {
    quint16 left = 0;
    quint16 right = this->listChar.size() - 1;
    quint16 mid, compareNum;

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
    this->listChar.insert(index,Qc);
    this->text.insert(index, QCharacter->getValue());
    //Mi occupo qui di segnalare al WorkerSocket dell'operazione
    DocOperation* docOp = new DocOperation(0,Qc,QTextCharFormat(),0,0,0);
    return *docOp;
}

DocOperation CRDT::localErase(quint16 index) {
    Char Qc = listChar[index];
    this->listChar.remove(index);
    this->text.remove(index,1);
    //Mi occupo qui di segnalare al WorkerSocket dell'operazione
    DocOperation* docOp = new DocOperation(1,Qc,QTextCharFormat(),0,0,0);
    return *docOp;
}

void CRDT::remoteInsert(Char value) {
    quint16 index = findInsertIndex(value);
    this->listChar.insert(index,value);
    this->text.insert(index, value.getValue());
}

void CRDT::remoteDelete(Char value) {
    quint16 index = findIndexByPosition(value);
    this->listChar.remove(index);
    this->text.remove(index,1);
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
    //Mi occupo qui di segnalare al WorkerSocket dell'operazione
    DocOperation* docOp = new DocOperation(2,listChar[index],oldFormat,0,0,0);
    return *docOp;
}

void CRDT::remoteFormatChange(Char ch){
    quint16 index = findInsertIndex(ch);
    this->listChar[index].setFormat(ch.getFormat());
}



