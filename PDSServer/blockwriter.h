#ifndef BLOCKWRITER_H
#define BLOCKWRITER_H

#include <QDataStream>
#include <QBuffer>

class BlockWriter
{
public:

    BlockWriter(QIODevice *io);

    ~BlockWriter();

    QDataStream &stream();

private:
    QBuffer buffer;
    QDataStream _stream;
    QIODevice *io;
};


#endif
