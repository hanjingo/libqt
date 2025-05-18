#ifndef CODEC_H
#define CODEC_H

#include <QObject>
#include <QBuffer>
#include <QQueue>
#include <QReadWriteLock>

#include "libqt/net/proto/message.h"

#ifndef MTU 1024
#define MTU 1024
#endif

class Codec : public QObject
{
    Q_OBJECT

public:
    using FnMsgFactory = std::function<Message*()>;

public:
    Codec(FnMsgFactory fn, int bufSz = 1024, QObject *parent = nullptr);
    ~Codec();

signals:
    void produceMsg(Message*);
    void produceBytes(QByteArray);

private slots:
    void consumeMsg(Message*);
    void consumeBytes(QByteArray);

private:
    QReadWriteLock   m_lock;
    QBuffer          m_buf;
    int              m_bufSz;
    FnMsgFactory     m_factory;
};

#endif
