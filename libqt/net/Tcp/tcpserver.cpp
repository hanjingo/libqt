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
    connect(conn, SIGNAL(disconnected(TcpConn*)), this, SLOT(onSocketDisconnected(TcpConn*)), Qt::QueuedConnection);
    m_conns.insert(conn);
}

void TcpServer::onSocketDisconnected(TcpConn* conn)
{
    WLocker locker{m_lock};
    disconnect(conn, SIGNAL(readed(TcpConn*)), this, SIGNAL(connReaded(TcpConn*)));
    disconnect(conn, SIGNAL(writed(TcpConn*)), this, SIGNAL(connWrited(TcpConn*)));
    disconnect(conn, SIGNAL(disconnected(TcpConn*)), this, SLOT(onSocketDisconnected(TcpConn*)));
    conn->close();
    m_conns.remove(conn);

    emit connDisconnected(conn);
}

void TcpServer::range(FnRangeConns fn)
{
    RLocker locker{m_lock};
    for (auto conn : m_conns)
        if (!fn(conn))
            break;
}

void TcpServer::loop()
{
    RLocker locker{m_lock};
    for (auto conn : m_conns)
    {
        conn->poll();
    }
}
