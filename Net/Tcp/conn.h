#ifndef CONN_H
#define CONN_H

#include <QObject>
#include <libqt/Net/Proto/message.h>

class TcpConn : public QObject
{
    Q_OBJECT
public:
    TcpConn();
    ~TcpConn();
};

#endif