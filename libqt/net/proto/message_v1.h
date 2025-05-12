#ifndef MESSAGE_V1_H
#define MESSAGE_V1_H

#include "message.h"

#include <QDataStream>
#include <QByteArray>

#include <QDebug>

// | 2 |  ...  |
// |len|payload|
class MessageV1 : public Message
{
public:
    MessageV1()
        : Message()
        , m_len{0}
        , m_payload{QByteArray()}
    {}
    MessageV1(const QByteArray& payload)
        : Message()
        , m_len{0}
        , m_payload{payload}
    {
        m_len = size();
    }
    ~MessageV1() {}

    inline qint16 headLen() { return 2; }
    inline qint16 len() { return m_len; }
    inline QByteArray& payload() { return m_payload; }

    virtual qint64 size() override
    {
        return headLen() + m_payload.size();
    }
    virtual qint64 encode(QByteArray& buf) override
    {
        m_len = size();

        QDataStream stream(&buf, QIODevice::ReadWrite);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << m_len << m_payload;
        return size();
    }
    virtual qint64 decode(const QByteArray& buf) override
    {
        QDataStream stream(buf);
        stream.setByteOrder(QDataStream::BigEndian);
        stream >> m_len >> m_payload;
        return m_len;
    }

private:
    qint16     m_len;
    QByteArray m_payload;
};

#endif
