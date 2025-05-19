#include "nethandler.h"

#include <QHostAddress>

NetHandler::NetHandler(const QAbstractSocket::SocketType typ,
                       const QString& host,
                       const quint16 port,
                       QObject *parent)
    : m_sock{new QAbstractSocket(typ, parent)}
{
    connect(m_sock, SIGNAL(connected()), this, SLOT(onConnected()), Qt::DirectConnection);
    connect(m_sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::DirectConnection);
    connect(m_sock, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)), Qt::DirectConnection);
    connect(m_sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::DirectConnection);

    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 1); // noblock
    m_sock->connectToHost(QHostAddress(host), port);
}

NetHandler::~NetHandler()
{
    if (m_sock != nullptr)
    {
        m_sock->waitForBytesWritten(100);
        m_sock->deleteLater();

        disconnect(m_sock, SIGNAL(connected()), this, SLOT(onConnected()));
        disconnect(m_sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        disconnect(m_sock, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                   this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
        disconnect(m_sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

        delete m_sock;
        m_sock = nullptr;
    }
}

// ----------------------------- socket signal --------------------------------
void NetHandler::onHostFound()
{
}

void NetHandler::onConnected()
{
    emit this->connected();
}

void NetHandler::onDisconnected()
{
    // do reconnect
    QHostAddress ip = m_sock->peerAddress();
    quint16 port = m_sock->peerPort();
    m_sock->connectToHost(ip, port);
}

void NetHandler::onReadyRead()
{
    auto data = m_sock->readAll();
    emit produceBytes(data);
}

void NetHandler::onStateChanged(QAbstractSocket::SocketState state)
{
    emit stateChanged(state);
}

void NetHandler::onError(QAbstractSocket::SocketError)
{
    qDebug() << "onError";
}

void NetHandler::onErrorOccurred(QAbstractSocket::SocketError)
{
    qDebug() << "onErrorOccurred";
}

void NetHandler::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    qDebug() << "onProxyAuthenticationRequired";
}

// ------------------------- user defined slot --------------------------

void NetHandler::consumeBytes(QByteArray data)
{
    m_sock->write(data);
    m_sock->flush();
}
