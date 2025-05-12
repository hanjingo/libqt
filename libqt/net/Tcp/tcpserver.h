#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>

#include <QTcpServer>
#include <QSet>
#include <QReadWriteLock>
#include <QThreadPool>

#include "tcpconn.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    using FnRangeConns = std::function<bool(TcpConn*)>;

public:
    explicit TcpServer(QThreadPool* pool = nullptr, QObject *parent = nullptr);

    inline void setConnReadBufSz(const int sz) { m_rBufSz = sz; }
    inline void setConnWriteBufSz(const int sz) { m_wBufSz = sz; }

    void range(FnRangeConns fn);
    void loop();

private:
    void init();

signals:
    void connDisconnected(TcpConn*);
    void connReaded(TcpConn*);
    void connWrited(TcpConn*);

private slots:
    void onNewConnection();
    void onSocketDisconnected(TcpConn*);

private:
    Q_DISABLE_COPY(TcpServer)

    QReadWriteLock      m_lock;
    QSet<TcpConn*>      m_conns;
    QThreadPool*        m_threads;
    int                 m_rBufSz;
    int                 m_wBufSz;
};

#endif
