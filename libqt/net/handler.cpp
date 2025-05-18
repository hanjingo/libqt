#include "handler.h"

#include <QHostAddress>

Handler::Handler(const QAbstractSocket::SocketType typ,
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

Handler::~Handler()
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
void Handler::onHostFound()
{
}

void Handler::onConnected()
{
    emit this->connected();
}

void Handler::onDisconnected()
{
    // do reconnect
    QHostAddress ip = m_sock->peerAddress();
    quint16 port = m_sock->peerPort();
    m_sock->connectToHost(ip, port);
}

void Handler::onReadyRead()
{
    auto data = m_sock->readAll();
    emit produceBytes(data);
}

void Handler::onStateChanged(QAbstractSocket::SocketState state)
{
    emit stateChanged(state);
}

void Handler::onError(QAbstractSocket::SocketError)
{
    qDebug() << "onError";
}

void Handler::onErrorOccurred(QAbstractSocket::SocketError)
{
    qDebug() << "onErrorOccurred";
}

void Handler::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    qDebug() << "onProxyAuthenticationRequired";
}

// ------------------------- user defined slot --------------------------

void Handler::consumeBytes(QByteArray data)
{
    m_sock->write(data);
    m_sock->flush();
}
