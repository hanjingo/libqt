#ifndef TCP_CONN_H
#define TCP_CONN_H

#include <functional>

#include <QObject>
#include <QIODevice>
#include <QTcpSocket>
#include <QBuffer>

#include "libqt/sync/channel.h"
#include "libqt/net/proto/message.h"

class TcpConn : public QObject
{
    Q_OBJECT

public:
    using SocketState = QAbstractSocket::SocketState;

public:
    explicit TcpConn(const int rBufSz = 1, const int wBufSz = 1, QObject *parent = nullptr);
    explicit TcpConn(QTcpSocket* base, const int rBufSz = 1, const int wBufSz = 1, QObject *parent = nullptr);
    virtual ~TcpConn();

    inline bool isValid() { return m_sock->state() == SocketState::ConnectedState; }
    inline void poll() { emit pollEvent(); }

    void read(Message* msg, quint64 ms = 0, quint16 retry = 0);
    void write(Message* msg);
    void read(QByteArray& buf, quint64 ms = 0, quint16 retry = 0);
    void write(QByteArray& buf);
    void init();
    bool connectToHost(const QString& ip, const quint16 port);
    void disconnectFromHost();
    void close();

signals:
    void pollEvent();
    void readed(TcpConn*);
    void writed(TcpConn*);

private slots:
    void onPollEvent();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError);
    void onStateChanged(QAbstractSocket::SocketState);

private:
    QTcpSocket*         m_sock;
    QBuffer             m_buf;
    Channel<QByteArray> m_chRead;
    Channel<QByteArray> m_chWrite;

    Channel<Message*>   m_chReadedMsg;
    Channel<Message*>   m_chWritedMsg;
    Channel<Message*>   m_chPreReadMsg;
    Channel<Message*>   m_chPreWriteMsg;
};

#endif
