#ifndef HANDLER_H
#define HANDLER_H

#include <QDebug>
#include <QObject>
#include <QThread>

#include <libqt/net/tcp/ip.h>
#include <libqt/net/tcp/tcpserver.h>
#include <libqt/net/tcp/tcpsocket.h>

class Handler : public QObject
{
    Q_OBJECT

public:
    explicit Handler(QObject *parent = nullptr) {}

    template <typename T>
    static QByteArray toByteArray(const T& t);

    template <typename T>
    static void fromByteArray(QByteArray& buf, T& t);

    void bind(TcpServer* serv);

private slots:
    void onSockReaded(TcpSocket* sock);
    void onSockWrited(TcpSocket*);
};

#endif
