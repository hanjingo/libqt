#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>

class Message
{
public:
    virtual qint64 size() = 0;
    virtual qint64 encode(QByteArray& buf) = 0;
    virtual qint64 decode(const QByteArray& buf)  = 0;
};

#endif
