#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QTcpServer>
#include <QSet>
#include <QReadWriteLock>
#include <QThreadPool>

#include "tcpconn.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit TcpServer(QThreadPool* pool = nullptr, QObject *parent = nullptr);

    inline void setConnReadBufSz(const int sz) { m_rBufSz = sz; }
    inline void setConnWriteBufSz(const int sz) { m_wBufSz = sz; }
    inline QSet<TcpConn*> conns() { return m_conns; }

    void loop();

private:
    void init();

signals:
    void connReaded(TcpConn*);
    void connWrited(TcpConn*);

private slots:
    void onNewConnection();
    void onSocketDisconnected();

private:
    QReadWriteLock      m_lock;
    QSet<TcpConn*>      m_conns;
    QThreadPool*        m_threads;
    int                 m_rBufSz;
    int                 m_wBufSz;
};

#endif
