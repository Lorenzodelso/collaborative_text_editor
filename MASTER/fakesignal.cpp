#include "Fakesignal.h"
#include <iostream>

Fakesignal::Fakesignal()
{

}

void Fakesignal::mandaSegnale(){
    QString nome("prova.txt");
    emit segnale(nome);
}

void Fakesignal::riceviSegnale(QString esito, CRDT crdt){
    std::cout << "Esito del segnale mandato al WorkerDoc: " + esito.toStdString() + "\n" <<std::flush;
    std::cout << "CRDT ricevuto dal WorkerDoc: " + crdt.text.toStdString() + "\n" <<std::flush;
}

void Fakesignal::mandaOperazione(DocOperation docOp){
    //std::cout << "Mando segnale...\n"<<std::flush;
    emit operazioneSegnale(docOp);
}

void Fakesignal::riceviEsitoOp(QString esito, DocOperation docOp){
    docOp.printOperation();
}
