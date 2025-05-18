#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <functional>

#include <QHash>
#include <QObject>
#include <QTcpSocket>
#include <QThreadPool>
#include <QEventLoop>

#include "libqt/sync/channel.h"
#include "libqt/net/proto/message.h"
#include "libqt/net/codec.h"

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QThreadPool* pool = QThreadPool::globalInstance(),
                       int bufSz = 1024,
                       QObject *parent = nullptr);
    ~TcpClient();

    inline QAbstractSocket::SocketState state() { return m_state; }

    void dial(const QString& ip,
              const quint16 port);
    void dial(const QString& ip,
              const quint16 port,
              const int ms);
    void dial(const QString& ip,
              const quint16 port,
              const Codec::FnMsgFactory fn);
    void dial(const QString& ip,
              const quint16 port,
              const int ms,
              const Codec::FnMsgFactory fn);
    void write(const QByteArray& data);
    bool read(QByteArray& data);
    bool read(QByteArray& data, int ms);

    void writeMsg(Message* msg);
    Message* readMsg();
    Message* readMsg(int ms);

    void close() { emit this->finish(); }

signals:
    void connected();
    void produceBytes(QByteArray);
    void produceMsg(Message*);
    void finish();

private slots:
    void consumeBytes(QByteArray);
    void consumeMsg(Message*);
    void onStateChanged(QAbstractSocket::SocketState);

private:
    Q_DISABLE_COPY(TcpClient)

    QThreadPool*                 m_threads;
    Channel<QByteArray>          m_dataBuf;
    Channel<Message*>            m_msgBuf;
    QAbstractSocket::SocketState m_state;
};

#endif
