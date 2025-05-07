#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QTcpServer>
#include <QSet>

#include "tcpsocket.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);

    inline void setSocketReadBufSz(const int sz) { m_rBufSz = sz; }
    inline void setSocketWriteBufSz(const int sz) { m_wBufSz = sz; }
    inline QSet<TcpSocket*> socks() { return m_socks; }
    virtual QTcpSocket *nextPendingConnection() override;

signals:
    void sockReaded(TcpSocket*);
    void sockWrited(TcpSocket*);

private slots:
    void onNewConnection();
    void onSocketDisconnected();

private:
    QSet<TcpSocket*> m_socks;
    int m_rBufSz = 1;
    int m_wBufSz = 1;
};

#endif
