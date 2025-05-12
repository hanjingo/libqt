#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <functional>

#include <QObject>
#include <QString>
#include <QThreadPool>
#include <QReadWriteLock>

#include "libqt/core/objpool.h"
#include "libqt/net/tcp/tcpconn.h"
#include "libqt/net/proto/message.h"
#include "libqt/net/proto/message_V1.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    using FnRangeConns = std::function<bool(TcpConn*)>;
    using FnMsgFilt = std::function<bool(Message*)>;

public:
    explicit TcpClient(QThreadPool* pool = nullptr, QObject *parent = nullptr);
    ~TcpClient();

    static TcpConn* dial(const QString& ip,
                         const quint16 port,
                         const int rBufSz = 1,
                         const int wBufSz = 1);

    void range(FnRangeConns fn);
    void add(TcpConn*);
    void del(TcpConn*);
    void clear();
    void loop();

private:
    void init();
    void doDel(TcpConn* conn);

signals:
    void connDisconnected(TcpConn*);
    void connReaded(TcpConn*);
    void connWrited(TcpConn*);

private slots:
    void onSocketDisconnected(TcpConn*);

private:
    Q_DISABLE_COPY(TcpClient)

    QReadWriteLock        m_lock;
    QSet<TcpConn*>        m_conns;
    QThreadPool*          m_threads;
};

#endif
