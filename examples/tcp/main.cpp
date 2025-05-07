#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <thread>
#include <chrono>
#include <QThread>

#include <libqt/net/tcp/ip.h>
#include <libqt/net/tcp/tcpserver.h>
#include <libqt/net/tcp/tcpsocket.h>

#include "handler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "tcp client-server example";

    Handler h{};
    TcpServer serv{};
    h.bind(&serv);
    qDebug() << "listen start";
    serv.listen(QHostAddress::Any, 10086);
    qDebug() << "listen end";
    QThread::msleep(100);

    auto sock = new TcpSocket(10, 10);
    sock->connectToHost(QHostAddress("127.0.0.1"), 10086);
    QByteArray msg1 = Handler::toByteArray(QString("hello"));
    while (true)
    {
        QCoreApplication::processEvents();
        QThread::msleep(1000);
        sock->write(msg1);
    }

    return a.exec();
}
