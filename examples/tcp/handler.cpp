#include "handler.h"

template <typename T>
QByteArray Handler::toByteArray(const T& t)
{
    QByteArray buf;
    QDataStream stream(&buf, QIODevice::ReadWrite);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << t;
    return buf;
}

template <typename T>
void Handler::fromByteArray(QByteArray& buf, T& t)
{
    QDataStream stream(&buf, QIODevice::ReadWrite);
    stream.setByteOrder(QDataStream::BigEndian);
    stream >> t;
}

void Handler::bind(TcpServer* serv)
{
    connect(serv, SIGNAL(sockReaded(TcpSocket*)), this, SLOT(onSockReaded(TcpSocket*)));
    connect(serv, SIGNAL(sockWrited(TcpSocket*)), this, SLOT(onSockWrited(TcpSocket*)));
}

void Handler::onSockReaded(TcpSocket* sock)
{
    QByteArray buf;
    sock->read(buf);
    qDebug() << "on sock readed:" << buf;

    buf = toByteArray(QString("world"));
    sock->write(buf);
}
void Handler::onSockWrited(TcpSocket*)
{
    qDebug() << "on sock writed";
}
