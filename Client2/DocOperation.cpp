#include "DocOperation.h"

DocOperation::DocOperation()
{

}

DocOperation::DocOperation(quint16 type, Char ch, QTextCharFormat oldFormat,quint16 siteId,quint16 cursorPos, quint16 cursorAnch):
    type(type), character(ch), oldFormat(oldFormat),siteId(siteId), cursorPos(cursorPos),cursorAnch(cursorAnch),alignementType(0)
{

}

DocOperation::DocOperation(quint16 cursorPos,quint16 alignementType,quint16 siteId):
    type(alignementChanged),cursorPos(cursorPos),alignementType(alignementType),siteId(siteId)
{

}

void DocOperation::printOperation(){
   QChar ch = this->character.getValue();
   QString result("");
   switch(this->type){
    case remoteInsert:
       result = "Insert";
       break;
   case remoteDelete:
       result = "Delete";
       break;
   case changedFormat:
       result = "Change-format";
       break;
   case cursorMoved:
       result = "Cursor moved";
       break;
   }
   std::cout << "Operation type: "+ result.toStdString()+ "on the char: " + ch.toLatin1() +"\n"<<std::flush;
}

QDataStream& operator<<(QDataStream& out, DocOperation docOp){
    //QTextCharFormat -> valori che ci interessano:
    //Font family
    QString fontFamily = docOp.oldFormat.fontFamily();
    //Is font italic?
    bool isItalic=docOp.oldFormat.fontItalic();
    //Font weight (bold or normal)
    quint32 fontWeight = docOp.oldFormat.fontWeight();
    //undelined?
    bool isUndelined = docOp.oldFormat.fontUnderline();
    //font size
    double fontSize = docOp.oldFormat.fontPointSize();

    out << docOp.type;
    out<< docOp.character;
    out<< docOp.siteId;
    out<< docOp.cursorPos;
    out<< docOp.cursorAnch;
    out<<fontFamily;
    out<< isItalic;
    out<< fontWeight;
    out<< isUndelined;
    out<< fontSize;

    out<< docOp.alignementType;
    return out;
}

QDataStream& operator>>(QDataStream& in, DocOperation& docOp){
    quint16 type;
    Char character;
    QTextCharFormat format;
    in >> type;
    in >> character;

    quint16 siteId;
    quint16 cursorPos;
    quint16 cursorAnch;

    quint16 alignementType;

    in >> siteId >> cursorPos >> cursorAnch;

    QString fontFamily;
    in >> fontFamily;
    bool isItalic;
    in >> isItalic;
    quint32 fontWeight;
    in >> fontWeight;
    bool isUndelined;
    in >> isUndelined;
    double fontSize;
    in >> fontSize;

    in >> alignementType;
    if(alignementType!=0){
        docOp = DocOperation(cursorPos,alignementType,siteId);
    }else{
        format.setFontFamily(fontFamily);
        format.setFontItalic(isItalic);
        format.setFontWeight(int(fontWeight));
        format.setFontUnderline(isUndelined);
        format.setFontPointSize(fontSize);

        docOp = DocOperation(type,character,format,siteId,cursorPos,cursorAnch);
    }

    return in;
}


quint16 DocOperation::getSiteId(){
    return this->siteId;
}
