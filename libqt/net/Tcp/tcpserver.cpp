#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

QTcpSocket *TcpServer::nextPendingConnection()
{
    qDebug() << "new TcpSocket";
    return new TcpSocket(m_rBufSz, m_wBufSz, this);
}

void TcpServer::onNewConnection()
{
    TcpSocket* sock = static_cast<TcpSocket*>(nextPendingConnection());
    connect(sock, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(sock, SIGNAL(readed(TcpSocket*)), this, SIGNAL(sockReaded(TcpSocket*)), Qt::QueuedConnection);
    connect(sock, SIGNAL(writed(TcpSocket*)), this, SIGNAL(sockWrited(TcpSocket*)), Qt::QueuedConnection);
    m_socks.insert(sock);
    qDebug() << "on sock conn";
}

void TcpServer::onSocketDisconnected()
{
    qDebug() << "on sock disconn";
    for (auto sock : m_socks)
    {
        if (sock->state() == QAbstractSocket::ConnectedState)
            continue;

        disconnect(sock, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
        disconnect(sock, SIGNAL(readed(TcpSocket*)), this, SIGNAL(sockReaded(TcpSocket*)));
        disconnect(sock, SIGNAL(writed(TcpSocket*)), this, SIGNAL(sockWrited(TcpSocket*)));
        m_socks.remove(sock);
    }
}
