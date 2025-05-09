#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <thread>
#include <chrono>
#include <QThread>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <libqt/net/tcp/ip.h>
#include <libqt/net/tcp/tcpserver.h>
#include <libqt/net/tcp/tcpconn.h>
#include <libqt/net/proto/message_v1.h>

#include "handler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "tcp client-server example";

    Handler hSrv{};
    TcpServer serv{QThreadPool::globalInstance()};
    hSrv.bind(&serv);
    serv.listen(QHostAddress::Any, 10086);

    // send QByteArray
    auto conn1 = new TcpConn(10, 10);
    Handler hConn1{};
    hConn1.bind(conn1);
    conn1->connectToHost("127.0.0.1", 10086);

    QByteArray hello = toByteArray(QString("hello"));
    conn1->write(hello);
    qDebug() << "conn1:" << conn1 << " send QByteArray:" << hello;

    QByteArray world;
    while (world.isEmpty())
    {
        QCoreApplication::processEvents();
        conn1->read(world, 100);
    }
    qDebug() << "conn1:" << conn1 << " recv QByteArray:" << world;


    // send Message
    auto conn2 = new TcpConn(10, 10);
    Handler hConn2{};
    hConn2.bind(conn2);
    conn2->connectToHost("127.0.0.1", 10086);

    QVector<qint32> sz;
    sz.append(180);
    sz.append(70);
    HelloReq req{QString("harry"), 30, sz};
    QJsonDocument docReq{req.toJson()};
    Message* msg1 = new MessageV1(CMD_HELLO_REQ, docReq.toJson());
    conn2->write(msg1);
    qDebug() << "conn2:" << conn2 << " send QJson:" << docReq;

    QByteArray rspData;
    Message* msg2 = nullptr;
    conn2->read(msg2, 100, 10);
    HelloRsp rsp{};
    QJsonObject rspObj = QJsonDocument::fromJson(rspData).object();
    rsp.fromJson(rspObj);
    qDebug() << "conn2:" << conn2 << " recv QJson:" << rspObj;

    return a.exec();
}
