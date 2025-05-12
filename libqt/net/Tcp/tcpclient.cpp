#include "tcpclient.h"

#include "libqt/sync/rwlocker.h"

TcpClient::TcpClient(QThreadPool* pool, QObject *parent)
    : m_lock{}
    , m_conns{}
    , m_threads{nullptr}
{
    init();
}

TcpClient::~TcpClient()
{
    WLocker locker{m_lock};
    for (auto conn : m_conns)
    {
        doDel(conn);
        conn->close();
    }
}

TcpConn* TcpClient::dial(const QString& ip, const quint16 port, const int rBufSz, const int wBufSz)
{
    TcpConn* conn = new TcpConn(rBufSz, wBufSz);
    if (!conn->connectToHost(ip, port))
    {
        conn->close();
        delete conn;
        return nullptr;
    }

    return conn;
}

void TcpClient::range(FnRangeConns fn)
{
    RLocker locker{m_lock};
    for (auto conn : m_conns)
        if (!fn(conn))
            break;
}

void TcpClient::add(TcpConn* conn)
{
    WLocker locker{m_lock};
    connect(conn, SIGNAL(readed(TcpConn*)), this, SIGNAL(connReaded(TcpConn*)), Qt::QueuedConnection);
    connect(conn, SIGNAL(writed(TcpConn*)), this, SIGNAL(connWrited(TcpConn*)), Qt::QueuedConnection);
    connect(conn, SIGNAL(disconnected(TcpConn*)), this, SLOT(onSocketDisconnected(TcpConn*)), Qt::QueuedConnection);
    m_conns.insert(conn);
}

void TcpClient::del(TcpConn* conn)
{
    WLocker locker{m_lock};
    doDel(conn);
}

void TcpClient::clear()
{
    WLocker locker{m_lock};
    for (auto conn : m_conns)
        doDel(conn);
}

void TcpClient::loop()
{
    RLocker locker{m_lock};
    for (auto conn : m_conns)
        conn->poll();
}

void TcpClient::init()
{
    if (m_threads != nullptr)
        m_threads->start([this](){ this->loop(); });
}

void TcpClient::doDel(TcpConn* conn)
{
    disconnect(conn, SIGNAL(readed(TcpConn*)), this, SIGNAL(connReaded(TcpConn*)));
    disconnect(conn, SIGNAL(writed(TcpConn*)), this, SIGNAL(connWrited(TcpConn*)));
    disconnect(conn, SIGNAL(disconnected(TcpConn*)), this, SLOT(onSocketDisconnected(TcpConn*)));
    conn->close();
    m_conns.remove(conn);
}

void TcpClient::onSocketDisconnected(TcpConn* conn)
{
    del(conn);
    emit connDisconnected(conn);
}
