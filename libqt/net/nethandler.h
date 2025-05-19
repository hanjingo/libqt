#ifndef NET_HANDLER_H
#define NET_HANDLER_H

#include <QObject>
#include <QAbstractSocket>

class NetHandler : public QObject
{
    Q_OBJECT

public:
    explicit NetHandler(const QAbstractSocket::SocketType typ,
                        const QString& ip,
                        const quint16 port,
                        QObject *parent = nullptr);
    ~NetHandler();

signals:
    void connected();
    void produceBytes(QByteArray);
    void stateChanged(QAbstractSocket::SocketState);

public slots:
    // usr signal handler
    void consumeBytes(QByteArray);

protected slots:
    // sockets signal handler
    void onHostFound();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onStateChanged(QAbstractSocket::SocketState);
    void onError(QAbstractSocket::SocketError);
    void onErrorOccurred(QAbstractSocket::SocketError);
    void onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);

private:
    QAbstractSocket* m_sock;
};

#endif
