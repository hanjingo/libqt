#ifndef HANDLER_H
#define HANDLER_H

#include <QDebug>
#include <QObject>
#include <QThread>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <libqt/net/tcp/ip.h>
#include <libqt/net/tcp/tcpserver.h>
#include <libqt/net/tcp/tcpconn.h>

#include <libqt/net/proto/message_v1.h>

enum cmd {
    CMD_HELLO_REQ = 0X1,
    CMD_HELLO_RSP,
};

template <typename T>
QByteArray toByteArray(const T& t);

template <typename T>
void fromByteArray(QByteArray& buf, T& t);

struct HelloReq
{
    QString name;
    qint16  age;
    QVector<qint32> size;

    HelloReq()
    {}
    HelloReq(QString arg1, qint16 arg2, QVector<qint32> arg3)
        : name{arg1}, age{arg2}, size{arg3}
    {}

    QJsonObject toJson()
    {
        QJsonObject obj;
        obj["name"] = name;
        obj["age"] = age;
        QJsonArray arr;
        for (auto e : size)
            arr.append(e);
        obj["size"] = arr;
        return obj;
    }

    void fromJson(QJsonObject& obj)
    {
        name = obj["name"].toString();
        age = obj["age"].toInt();
        size.clear();
        for (auto e : obj["size"].toArray())
            size.append(e.toInt());
    }
};

struct HelloRsp
{
    qint16 result;

    QJsonObject toJson()
    {
        QJsonObject obj;
        obj["result"] = result;
        return obj;
    }

    void fromJson(QJsonObject& obj)
    {
        result = obj["result"].toInt();
    }
};

class Handler : public QObject
{
    Q_OBJECT

public:
    explicit Handler(QObject *parent = nullptr) {}

    void bind(TcpServer* serv);
    void bind(TcpConn* conn);

private slots:
    void onConnReaded(TcpConn*);
    void onConnWrited(TcpConn*);

private:
    TcpServer* m_serv;
};

#endif
