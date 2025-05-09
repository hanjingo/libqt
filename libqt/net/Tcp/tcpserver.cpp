#include "tcpserver.h"

#include "libqt/sync/rwlocker.h"

TcpServer::TcpServer(QThreadPool* pool, QObject *parent)
    : QTcpServer(parent)
    , m_lock{}
    , m_conns{}
    , m_threads{nullptr}
    , m_rBufSz{1}
    , m_wBufSz{1}
{
    init();
}

void TcpServer::init()
{
    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    if (m_threads != nullptr)
    {
        m_threads->start([this](){ this->loop(); });
    }
}

void TcpServer::onNewConnection()
{
    WLocker locker{m_lock};
    auto conn = new TcpConn(QTcpServer::nextPendingConnection(), m_rBufSz, m_wBufSz, this);
    connect(conn, SIGNAL(readed(TcpConn*)), this, SIGNAL(connReaded(TcpConn*)), Qt::QueuedConnection);
    connect(conn, SIGNAL(writed(TcpConn*)), this, SIGNAL(connWrited(TcpConn*)), Qt::QueuedConnection);
    m_conns.insert(conn);
}

void TcpServer::onSocketDisconnected()
{
    WLocker locker{m_lock};
    for (auto conn : m_conns)
    {
        if (conn->isValid())
            continue;

        disconnect(conn, SIGNAL(readed(TcpConn*)), this, SIGNAL(connReaded(TcpConn*)));
        disconnect(conn, SIGNAL(writed(TcpConn*)), this, SIGNAL(connWrited(TcpConn*)));
        m_conns.remove(conn);
    }
}

void TcpServer::loop()
{
    WLocker locker{m_lock};
    for (auto conn : m_conns)
    {
        conn->poll();
    }
}
