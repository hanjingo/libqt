#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <QIODevice>
#include <QTcpSocket>

#include "libqt/sync/channel.h"

class TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:
    explicit TcpSocket(const int rBufSz = 1, const int wBufSz = 1, QObject *parent = nullptr);
    virtual ~TcpSocket();
    inline void read(QByteArray& buf) { m_rCh >> buf; }
    inline void write(QByteArray& buf) { m_wCh << buf; emit readyWrite(); }

    void disconnectFromHost();
    void close();
    void read(QByteArrayList& li);
    void write(QByteArrayList& li);

signals:
    void readyWrite();
    void readed(TcpSocket*);
    void writed(TcpSocket*);

private slots:
    void onReadyRead();
    void onReadyWrite();
    void onSocketError(QAbstractSocket::SocketError);
    void onStateChanged(QAbstractSocket::SocketState);

private:
    Channel<QByteArray> m_rCh;
    Channel<QByteArray> m_wCh;
};

#endif
