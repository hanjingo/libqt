#ifndef MESSAGE_V1_H
#define MESSAGE_V1_H

#include "message.h"

#include <QDataStream>
#include <QByteArray>

#include "libqt/crypto/crc.h"

// | 2 |2 |    4     |  ...  | 2 |
// |len|id|serial num|payload|crc|
class MessageV1 : public Message
{
public:
    MessageV1()
        : Message()
        , m_len{0}
        , m_id{0}
        , m_serial{0}
        , m_payload{QByteArray()}
        , m_crc{0}
    {}
    MessageV1(qint16 id, const QByteArray& payload)
        : Message()
        , m_len{0}
        , m_id{id}
        , m_serial{0}
        , m_payload{payload}
        , m_crc{0}
    {}
    ~MessageV1() {}

    inline qint16 headLen() { return 10; }
    inline qint16 len() { return m_len; }
    inline qint16 id() { return m_id; }
    inline qint32 serialNum() { return m_serial; }
    inline QByteArray& payload() { return m_payload; }
    inline qint16 crc() { return m_crc; }

    virtual qint64 size() override
    {
        return headLen() + m_payload.size();
    }
    virtual qint64 encode(QByteArray& buf) override
    {
        QDataStream stream(&buf, QIODevice::ReadWrite);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << m_len << m_id << m_serial << m_payload;
        m_crc = Crc::calc16(buf);
        stream << m_crc;
        return size();
    }
    virtual qint64 decode(QByteArray& buf) override
    {
        QDataStream stream(&buf, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream >> m_len >> m_id >> m_serial >> m_payload >> m_crc;
        return m_len;
    }

private:
    qint16     m_len;
    qint16     m_id;
    qint32     m_serial;
    QByteArray m_payload;
    qint16     m_crc;
};

#endif
