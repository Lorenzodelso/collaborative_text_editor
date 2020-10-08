#include "Char.h"

#include "Char.h"

Char::Char(){}
Char::Char(quint16 siteId, quint16 counter, QVector<quint32> position, QChar value,QTextCharFormat format):
siteId(siteId), counter(counter),position(position),value(value),format(format),alignementType(0) {}

Char::Char(const Char& source):siteId(source.siteId), counter(source.counter), value(source.value),format(source.format),alignementType(source.alignementType){
    position = QVector<quint32>(source.position);
}

/*METHOD compareTo: prende i due Char e confronta le due posizioni
 * Return values: -1 se this<other
 *                0  se this=other
 *                1  se this>other*/
int Char::compareTo(Char other) {
    auto const pos1=this->position;
    auto const pos2=other.position;

    for (quint32 i=0; i<std::min(pos1.size(),pos2.size());i++){
        auto id1=pos1[i];
        auto id2=pos2[i];

        if (id1<id2)
            return -1;
        else if (id1>id2)
            return 1;
    }

    if (pos1.size()<pos2.size()){
        return -1;
    }else {
        if (pos1.size()>pos2.size())
            return 1;
        else
            //DA CONTROLLARE QUEST'ULTIMA RETURN, POTREBBE NON ESSERE CORRETTA.
            return (this->siteId == other.siteId ? 0 : 1);
    }
}

QVector<quint32> Char::getPos() {
    return this->position;
}

QChar Char::getValue() {
    return value;
}

void Char::setFormat(QTextCharFormat format){
    this->format.merge(format);
    QString name = this->format.fontFamily();
}

QTextCharFormat Char::getFormat(){
    return this->format;
}

quint16 Char::getSiteId(){return this->siteId;}

QDataStream& operator<<(QDataStream& out,const Char& ch){
    out << ch.siteId << ch.counter << ch.position <<ch.value <<ch.alignementType;
    //Passaggio di valori che mi interessano per salvare anche il formato per ogni carattere
    //Chiamateci stacanovisti

    //QTextCharFormat -> valori che ci interessano:
    //Font family
    QString fontFamily = ch.format.fontFamily();
    //Is font italic?
    bool isItalic=ch.format.fontItalic();
    //Font weight (bold or normal)
    quint32 fontWeight = ch.format.fontWeight();
    //undelined?
    bool isUndelined = ch.format.fontUnderline();
    //font size
    qreal fontSize = ch.format.fontPointSize();

    out << fontFamily << isItalic << fontWeight << isUndelined << fontSize;

    return out;
}

QDataStream& operator>>(QDataStream& in, Char& ch){
    quint16 siteId;
    quint16 counter;
    QChar value;
    quint16 alignementType;
    QVector<quint32> pos;
    QTextCharFormat format;
    in >> siteId;
    in >> counter;
    in >> pos;
    in >> value;
    in >> alignementType;

    //Riprendo tutti i valori che mi interessano per il formato
    QString fontFamily;
    in >> fontFamily;
    bool isItalic;
    in >> isItalic;
    quint32 fontWeight;
    in >> fontWeight;
    bool isUndelined;
    in >> isUndelined;
    qreal fontSize;
    in >> fontSize;

    //Setto i valori in un formato di default
    format.setFontFamily(fontFamily);
    format.setFontItalic(isItalic);
    format.setFontWeight(int(fontWeight));
    format.setFontUnderline(isUndelined);
    format.setFontPointSize(fontSize);

    //Costruisco il Char anche con il formato
    ch = Char(siteId,counter,pos,value,format);
    ch.setAlign(alignementType);
    return in;
}

void Char::setAlign(quint16 alignementType){
    this->alignementType = alignementType;
}

quint16 Char::getAlign(){
    return this->alignementType;
}
