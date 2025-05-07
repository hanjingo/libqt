#include "tcpsocket.h"

TcpSocket::TcpSocket(const int rBufSz, const int wBufSz, QObject *parent)
    : QTcpSocket(parent)
    , m_rCh{rBufSz}
    , m_wCh{wBufSz}
{
    connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::QueuedConnection);
    connect(this, SIGNAL(readyWrite()), this, SLOT(onReadyWrite()), Qt::QueuedConnection);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
}

TcpSocket::~TcpSocket()
{
    disconnect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    disconnect(this, SIGNAL(readyWrite()), this, SLOT(onReadyWrite()));
    disconnect(this, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    disconnect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
               this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
}

void TcpSocket::disconnectFromHost()
{
    onReadyWrite();
    flush();
    QAbstractSocket::disconnectFromHost();
}

void TcpSocket::close()
{
    QIODevice::close();
    m_rCh.close();
    m_wCh.close();
}

void TcpSocket::read(QByteArrayList& li)
{
    do {
        QByteArray buf;
        if (!m_rCh.tryDequeue(buf))
            break;
        li.append(buf);
    } while(true);
}

void TcpSocket::write(QByteArrayList& li)
{
    for (auto buf : li)
    {
        m_wCh << buf;
        emit readyWrite();
    }
}

void TcpSocket::onReadyRead()
{
    qDebug() << "onReadyRead start";
    if (state() != QAbstractSocket::ConnectedState)
        return;

    qDebug() << "onReadyRead before readAll";
    m_rCh << readAll();
    qDebug() << "onReadyRead after readAll";
    emit readed(this);
    qDebug() << "onReadyRead end";
}

void TcpSocket::onReadyWrite()
{
    if (state() != QAbstractSocket::ConnectedState)
        return;

    qDebug() << "onReadyWrite start";
    do {
        qDebug() << "onReadyWrite before write";
        QByteArray data;
        if (!m_wCh.tryDequeue(data) || data.isEmpty() || QIODevice::write(data) < 1)
            break;

        qDebug() << "onReadyWrite after write";
        emit writed(this);
    } while(true);
    qDebug() << "onReadyWrite end";
}

void TcpSocket::onSocketError(QAbstractSocket::SocketError err)
{

}

void TcpSocket::onStateChanged(QAbstractSocket::SocketState stat)
{
    switch (stat)
    {
    case UnconnectedState: { break; }
    case HostLookupState: { break; }
    case ConnectingState: { break; }
    case ConnectedState: { qDebug() << "onStateChanged to ConnectedState send"; emit readyWrite(); break; }
    case BoundState: { break; }
    case ListeningState: { break; }
    case ClosingState: { break; }
    }
}
