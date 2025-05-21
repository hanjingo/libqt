#include "nethandler.h"

#include <QHostAddress>

NetHandler::NetHandler(const QAbstractSocket::SocketType typ,
                       const QString& host,
                       const quint16 port,
                       bool reconnect,
                       QObject *parent)
    : m_sock{new QAbstractSocket(typ, parent)}
    , m_breconnect{reconnect}
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
        if (m_sock->state() == QAbstractSocket::ConnectedState)
            m_sock->waitForBytesWritten(100);

        disconnect(m_sock, SIGNAL(connected()), this, SLOT(onConnected()));
        disconnect(m_sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        disconnect(m_sock, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                   this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
        disconnect(m_sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

        m_sock->deleteLater();
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
    if (m_breconnect)
    {
        QHostAddress ip = m_sock->peerAddress();
        quint16 port = m_sock->peerPort();
        m_sock->connectToHost(ip, port);
    }

    emit this->disconnected();
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

void NetHandler::quit()
{
    qDebug() << "quit tcp socket";
    if (m_sock != nullptr)
        m_sock->disconnectFromHost();
}
