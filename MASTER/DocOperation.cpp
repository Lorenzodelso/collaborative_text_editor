#include "DocOperation.h"

DocOperation::DocOperation()
{

}

DocOperation::DocOperation(quint16 type, Char ch, QTextCharFormat oldFormat): type(type), character(ch), oldFormat(oldFormat)
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
   }
   std::cout << "Operation type: "+ result.toStdString()+ "on the char: " + ch.toLatin1() +"\n"<<std::flush;
}
