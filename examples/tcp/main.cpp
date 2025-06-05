#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QHostAddress>
#include <QByteArray>
#include <QtConcurrent/QtConcurrent>

#include <QTcpServer>

#include <libqt/net/tcp/ip.h>
#include <libqt/net/tcp/tcpclient.h>
#include <libqt/sync/dispatcher.h>
#include <libqt/net/nethandler.h>

#include <libqt/net/proto/message.h>
#include <libqt/net/proto/message_v1.h>

template <typename T>
QByteArray toByteArray(const T& t)
{
    QByteArray buf;
    QDataStream stream(&buf, QIODevice::ReadWrite);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << t;
    return buf;
}

template <typename T>
void fromByteArray(QByteArray& buf, T& t)
{
    QDataStream stream(&buf, QIODevice::ReadWrite);
    stream.setByteOrder(QDataStream::BigEndian);
    stream >> t;
}

struct HelloReq
{
    QString name;
    qint16  age;
    QVector<qint32> size;

    HelloReq() {}
    HelloReq(QString arg1, qint16 arg2, QVector<qint32> arg3) : name{arg1}, age{arg2}, size{arg3} {}
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "tcp client-server example";

    qDebug() << "tcp socket bytearray example";
    QTcpServer srv1{};
    QObject::connect(&srv1, &QTcpServer::newConnection, [&srv1](){
        auto conn = srv1.nextPendingConnection();
        QObject::connect(conn, &QTcpSocket::readyRead, [conn](){
            auto data = conn->readAll();
            qDebug() << "srv read data:" << data;

            conn->write(toByteArray(QString("world")));
            conn->flush();
        });
    });
    srv1.listen(QHostAddress::Any, 10086);

   // TcpClient read write byte data
    QtConcurrent::run([](){
        auto cli1 = new TcpClient();
        cli1->dial("127.0.0.1", 10086, 100);

        if (cli1->state() != QAbstractSocket::ConnectedState)
        {
            qDebug() << "Fail to dail 127.0.0.1:10086";
            return;
        } else {
            qDebug() << "Dial 127.0.0.1:10086 succ";
        }

        cli1->write(toByteArray(QString("hello")));

        QByteArray data;
        cli1->read(data, 100);
        qDebug() << "cli1 read data:" << data;

        QEventLoop loop;
        loop.exec();
    });


    // use message protocol
    qDebug() << "tcp socket protocol example";
    QtConcurrent::run([](){
        QTcpServer srv2{};
        QObject::connect(&srv2, &QTcpServer::newConnection, [&srv2](){
            auto conn = srv2.nextPendingConnection();
            QObject::connect(conn, &QTcpSocket::readyRead, [conn](){
                auto data = conn->readAll();
                qDebug() << "srv2 onReadyRead with data=" << data;

                HelloRsp rsp{};
                rsp.result = 1;
                QJsonDocument doc{rsp.toJson()};
                Message* msgRsp = new MessageV1(doc.toJson());
                QByteArray buf;
                msgRsp->encode(buf);
                conn->write(buf);
            });
        });
        srv2.listen(QHostAddress::Any, 10087);

        QEventLoop loop;
        loop.exec();
    });

    // TcpClient read write message
    QtConcurrent::run([](){
        auto cli2 = new TcpClient();
        cli2->dial("127.0.0.1", 10087, 100, []()->Message*{ return new MessageV1(); });

        if (cli2->state() != QAbstractSocket::ConnectedState)
        {
            qDebug() << "Fail to dail 127.0.0.1:10087";
            return;
        } else {
            qDebug() << "Dial 127.0.0.1:10087 succ";
        }

        QVector<qint32> sz;
        sz.append(180);
        sz.append(70);
        HelloReq req{QString("harry"), 30, sz};
        QJsonDocument docReq{req.toJson()};
        Message* msgReq = new MessageV1(docReq.toJson());
        cli2->writeMsg(msgReq);

        MessageV1* msgRsp = static_cast<MessageV1*>(cli2->readMsg(500));
        if (msgRsp == nullptr)
        {
            qDebug() << "Cli2 Fail to read with msgRsp=" << msgRsp;
        } else {
            qDebug() << "msgRsp=" << msgRsp;
            QJsonObject rspObj = QJsonDocument::fromJson(msgRsp->payload()).object();
            HelloRsp rsp{};
            rsp.fromJson(rspObj);

            qDebug() << "Cli2 read " << rsp.toJson();
        }

        QEventLoop loop;
        loop.exec();
    });

    return a.exec();
}

