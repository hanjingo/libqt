#include "handler.h"

#include <QThreadPool>

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

void SrvHandler::bind(TcpServer* serv)
{
    m_serv = serv;
    connect(m_serv, SIGNAL(connReaded(TcpConn*)), this, SLOT(onConnReaded(TcpConn*)));
    connect(m_serv, SIGNAL(connWrited(TcpConn*)), this, SLOT(onConnWrited(TcpConn*)));
}

void SrvHandler::onConnReaded(TcpConn* conn)
{
    static int count = 0;
    count++;
    qDebug() << "serv onConnReaded with count=" << count;

    if (count == 1)
    {
        QByteArray req;
        conn->read(req);
        qDebug() << "serv conn:" << conn << " recv:" << req;

        QByteArray rsp = toByteArray(QString("world"));
        qDebug() << "serv conn:" << conn << " send resp QByteArray:" << rsp;
        conn->write(rsp);
        conn->poll();
        return;
    }

    MessageV1* msgReq = new MessageV1();
    conn->read(msgReq);
    QJsonObject reqObj = QJsonDocument::fromJson(msgReq->payload()).object();
    HelloReq req{};
    req.fromJson(reqObj);
    qDebug() << "serv conn:" << conn << " recv req Message:" << msgReq->payload();

    HelloRsp rsp{};
    rsp.result = 1;
    QJsonDocument doc{rsp.toJson()};
    Message* msgRsp = new MessageV1(doc.toJson());
    qDebug() << "serv conn:" << conn << " send resp Message:" << doc.toJson();
    conn->write(msgRsp);
}

void SrvHandler::onConnWrited(TcpConn* conn)
{
    qDebug() << "serv onConnWrited";
}

void CliHandler::bind(TcpClient* cli)
{
    m_cli = cli;
    connect(m_cli, SIGNAL(connReaded(TcpConn*)), this, SLOT(onConnReaded(TcpConn*)));
    connect(m_cli, SIGNAL(connWrited(TcpConn*)), this, SLOT(onConnWrited(TcpConn*)));
}

void CliHandler::onConnReaded(TcpConn* conn)
{
    qDebug() << "cli onConnReaded";

    MessageV1* msgRsp = new MessageV1();
    conn->read(msgRsp);
    QJsonObject rspObj = QJsonDocument::fromJson(msgRsp->payload()).object();
    HelloReq rsp{};
    rsp.fromJson(rspObj);
    qDebug() << "cli conn:" << conn << " recv rsp Message:" << msgRsp->payload();
}

void CliHandler::onConnWrited(TcpConn* conn)
{
    qDebug() << "cli onConnWrited";
}
