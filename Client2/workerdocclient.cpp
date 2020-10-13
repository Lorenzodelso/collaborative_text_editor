#include "workerdocclient.h"

WorkerDocClient::WorkerDocClient(CRDT* algoritmoCRDT, quint16 siteId,QMap<quint16,QTextCursor*>*cursorMap):
    algoritmoCRDT(algoritmoCRDT),siteId(siteId), colorWriting(false), cursorMap(cursorMap){
}

void WorkerDocClient::editorFormatChanged(QTextCharFormat format){
    currentEditorFormat = format;
}

void WorkerDocClient::colorWritingStarted(){ colorWriting = true;}
void WorkerDocClient::colorWritingEnded(){ colorWriting = false;}

void WorkerDocClient::comunicaCRDTInserimentoLocale(int pos, int numInserted){
    bool bufferMode = numInserted > SIGNAL_LIMIT;
    QList<DocOperation> opList;
    int numOperationTreated = 0;
    auto inserted = document->toPlainText().mid(pos,numInserted);
    for (int i=0;i<numInserted;i++){
        QTextCharFormat format = currentEditorFormat;
        if (colorWriting){
            format.setForeground(QBrush(QColor("black")));
        }
        QChar ch = inserted[i];
        DocOperation docOp = algoritmoCRDT->localInsert(ch,format,pos+i);
        if (bufferMode){
            opList.append(docOp);
            numOperationTreated++;
            if(numOperationTreated >= BUFFER_DIMENSION || i==numInserted-1){
                emit SigOpDocLocaleBuffered(opList);
                opList.clear();
                numOperationTreated = 0;
            }
        }else
            emit SigOpDocLocale(docOp);
    }
}

void WorkerDocClient::comunicaCRDTRimozioneLocale(int pos, int numRemoved){
    bool bufferMode = numRemoved > SIGNAL_LIMIT;
    QList<DocOperation> opList;
    int numOperationTreated = 0;
    for(int i=0;i<numRemoved;i++){
        DocOperation docOp = algoritmoCRDT->localErase(pos);
        if (bufferMode){
            opList.append(docOp);
            numOperationTreated++;
            if (docOp.character.getValue()=='\n'){
                opList.append(DocOperation(pos,0,siteId));
                numOperationTreated++;
            }
            if(numOperationTreated >= BUFFER_DIMENSION || i==numRemoved-1){
                emit SigOpDocLocaleBuffered(opList);
                opList.clear();
                numOperationTreated = 0;
            }
        }else{
            emit SigOpDocLocale(docOp);
            if (docOp.character.getValue()=='\n'){
                emit SigOpDocLocale(DocOperation(pos,0,this->siteId));
            }
          }
    }
}

void WorkerDocClient::CRDTInsertRemove(int pos, int rem, int add,QString strAdded,QTextCharFormat format){
    if(rem==0 && add>0){
        comunicaCRDTInserimentoLocale(pos,add);
    }
    else if(rem>0 && add==0){
        comunicaCRDTRimozioneLocale(pos,rem);
    }
    else if(add>0 && rem>0){
        if(add==rem){
            comunicaCRDTRimozioneLocale(pos,rem);
            comunicaCRDTInserimentoLocale(pos,add);
        }
        else{
            if(pos==0){
                comunicaCRDTInserimentoLocale(pos,add);
            }
            else{
                comunicaCRDTRimozioneLocale(pos,rem);
                comunicaCRDTInserimentoLocale(pos,add);
            }
        }
    }
}

void WorkerDocClient::opDocRemota(DocOperation operation){
    disconnect(document,&QTextDocument::contentsChange,
                   this, &WorkerDocClient::CRDTInsertRemove );
    switch(operation.type){
     case remoteInsert:
     {
       quint16 index = algoritmoCRDT->remoteInsert(operation.character);
       QTextCursor *cursor = cursorMap->find(operation.siteId).value();
       auto colors = QColor::colorNames();
       cursor->setPosition(index);
       if (colorWriting == true){
           QTextCharFormat coloredFormat(operation.character.getFormat());
           coloredFormat.setForeground(QBrush(QColor(colors[operation.character.getSiteId()])));
           cursor->mergeCharFormat(coloredFormat);
           cursor->insertText(operation.character.getValue());
       }
       else{
           cursor->insertText(operation.character.getValue(),operation.character.getFormat());
       }
       break;
     }
     case remoteDelete:
     {
       quint16 index =algoritmoCRDT->remoteDelete(operation.character);
       QTextCursor *cursor = cursorMap->find(operation.siteId).value();
       cursor->setPosition(index);
       cursor->deleteChar();
       break;
    }
  }
    connect(document,&QTextDocument::contentsChange,
                   this, &WorkerDocClient::CRDTInsertRemove);
}

void WorkerDocClient::opDocRemotaBuffered(QList<DocOperation> opList){
    while(!opList.isEmpty()){
        DocOperation op = opList.first();
        opList.pop_front();
        opDocRemota(op);
    }
}
