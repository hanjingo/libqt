#include "tcpclient.h"

#include "libqt/net/nethandler.h"

#include <QTimer>
#include <QHostAddress>

TcpClient::TcpClient(QThreadPool* pool, int bufSz, QObject *parent)
    : m_threads{pool}
    , m_dataBuf{bufSz}
    , m_msgBuf{bufSz}
{
}

TcpClient::~TcpClient()
{
}

void TcpClient::dial(const QString& ip, const quint16 port)
{
    m_threads->start([this, ip, port](){
        QEventLoop loop;
        connect(this, SIGNAL(finish()), &loop, SLOT(quit()));

        NetHandler h{QAbstractSocket::TcpSocket, ip, port};

        // socket signal
        connect(&h, SIGNAL(connected()), this, SIGNAL(connected()), Qt::DirectConnection);
        connect(&h, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this, SLOT(onStateChanged(QAbstractSocket::SocketState)), Qt::DirectConnection);

        // Way1: TcpClient ,--> NetHandler
        connect(this, SIGNAL(produceBytes(QByteArray)), &h, SLOT(consumeBytes(QByteArray)), Qt::DirectConnection);
        connect(&h, SIGNAL(produceBytes(QByteArray)), this, SLOT(consumeBytes(QByteArray)), Qt::DirectConnection);

        loop.exec();
        qDebug() << "handler loop end";
    });
}

void TcpClient::dial(const QString& ip, const quint16 port, const int ms)
{
    dial(ip, port);

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), this, SIGNAL(finish()));
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    QObject::connect(this, SIGNAL(connected()), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(connected()), &timer, SLOT(stop()));

    timer.start(ms);
    loop.exec();
}

void TcpClient::dial(const QString& ip, const quint16 port, const Codec::FnMsgFactory fn)
{
    m_threads->start([this, ip, port, fn](){
        QEventLoop loop;
        connect(this, SIGNAL(finish()), &loop, SLOT(quit()));

        NetHandler h{QAbstractSocket::TcpSocket, ip, port};
        Codec codec{fn};

        // socket signal
        connect(&h, SIGNAL(connected()), this, SIGNAL(connected()), Qt::DirectConnection);
        connect(&h, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this, SLOT(onStateChanged(QAbstractSocket::SocketState)), Qt::DirectConnection);

        // Way2: TcpClient <--> Codec <--> NetHandler
        connect(this, SIGNAL(produceMsg(Message*)), &codec, SLOT(consumeMsg(Message*)), Qt::DirectConnection);
        connect(&codec, SIGNAL(produceBytes(QByteArray)), &h, SLOT(consumeBytes(QByteArray)), Qt::DirectConnection);

        connect(&h, SIGNAL(produceBytes(QByteArray)), &codec, SLOT(consumeBytes(QByteArray)), Qt::DirectConnection);
        connect(&codec, SIGNAL(produceMsg(Message*)), this, SLOT(consumeMsg(Message*)), Qt::DirectConnection);

        loop.exec();
        qDebug() << "handler loop end";
    });
}

void TcpClient::dial(const QString& ip, const quint16 port, const int ms, const Codec::FnMsgFactory fn)
{
    dial(ip, port, fn);

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), this, SIGNAL(finish()));
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    QObject::connect(this, SIGNAL(connected()), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(connected()), &timer, SLOT(stop()));

    timer.start(ms);
    loop.exec();
}

void TcpClient::write(const QByteArray& data)
{
    emit this->produceBytes(data);
}

bool TcpClient::read(QByteArray& data)
{
    return m_dataBuf.tryDequeue(data);
}

bool TcpClient::read(QByteArray& data, int ms)
{
    return m_dataBuf.tryDequeue(data, ms);
}

void TcpClient::writeMsg(Message* msg)
{
    emit this->produceMsg(msg);
}

Message* TcpClient::readMsg()
{
    Message* msg = nullptr;
    m_msgBuf.tryDequeue(msg);
    return msg;
}

Message* TcpClient::readMsg(int ms)
{
    Message* msg = nullptr;
    m_msgBuf.tryDequeue(msg, ms);
    return msg;
}

void TcpClient::consumeBytes(QByteArray data)
{
    qDebug() << "TcpClient::consumeBytes " << data;
    m_dataBuf << data;

    emit this->readyRead();
}

void TcpClient::consumeMsg(Message* msg)
{
    qDebug() << "TcpClient::consumeMsg " << msg;
    m_msgBuf << msg;

    emit this->readyRead();
}

void TcpClient::onStateChanged(QAbstractSocket::SocketState state)
{
    m_state = state;
}
