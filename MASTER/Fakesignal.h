#ifndef FAKESIGNAL_H
#define FAKESIGNAL_H

#include <QObject>
#include "CRDT.h"
#include "DocOperation.h"

class Fakesignal
        : public QObject
        {
            Q_OBJECT

        public:
            Fakesignal();
            void mandaSegnale();
            void mandaOperazione(DocOperation docOp);

        signals:
            void segnale(QString nome);
            void operazioneSegnale(DocOperation docOp);

        public slots:
            void riceviSegnale(QString esito, CRDT crdt);
            void riceviEsitoOp(QString esito, DocOperation docOp);

        };

#endif // FAKESIGNAL_H
