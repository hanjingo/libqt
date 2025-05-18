#include "codec.h"

#include "libqt/sync/rwlocker.h"

#include <QDebug>

Codec::Codec(FnMsgFactory fn, int bufSz, QObject *parent)
    : m_lock{}
    , m_buf{}
    , m_bufSz{bufSz}
    , m_factory{fn}
{
    m_buf.open(QBuffer::ReadWrite);
}

Codec::~Codec()
{
}

void Codec::consumeMsg(Message* msg)
{
    qDebug() << "onWrite Message";
    WLocker locker{m_lock};
    QByteArray data;
    msg->encode(data);
    emit this->produceBytes(data);
}

void Codec::consumeBytes(QByteArray data)
{
    qDebug() << "Codec::consumeBytes " << data;
    WLocker locker{m_lock};
    if (m_buf.size() < MTU * m_bufSz)
        m_buf.write(data);

    auto msg = m_factory();
    if (msg == nullptr)
        return;

    auto sz = msg->decode(m_buf.data());
    if (sz < 1)
        return;

    m_buf.read(sz);
    emit this->produceMsg(msg);
}
