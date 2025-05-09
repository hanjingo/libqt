#include "tcpconn.h"

#include <QHostAddress>
#include <QDebug>

#include "libqt/net/tcp/ip.h"

TcpConn::TcpConn(const int rBufSz, const int wBufSz, QObject *parent)
    : m_sock(new QTcpSocket(parent))
    , m_buf{}
    , m_chRead{rBufSz}
    , m_chWrite{wBufSz}
    , m_chReadedMsg{rBufSz}
    , m_chWritedMsg{wBufSz}
    , m_chPreReadMsg{rBufSz}
    , m_chPreWriteMsg{wBufSz}
{
    init();
}

TcpConn::TcpConn(QTcpSocket* base, const int rBufSz, const int wBufSz, QObject *parent)
    : m_sock(base)
    , m_buf{}
    , m_chRead{rBufSz}
    , m_chWrite{wBufSz}
    , m_chReadedMsg{rBufSz}
    , m_chWritedMsg{wBufSz}
    , m_chPreReadMsg{rBufSz}
    , m_chPreWriteMsg{wBufSz}
{
    init();
}

TcpConn::~TcpConn()
{
    // connect QTcpSocket Signal
    disconnect(m_sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    disconnect(m_sock, SIGNAL(stateChanged(SocketState)), this, SLOT(onStateChanged(SocketState)));
    disconnect(m_sock, SIGNAL(errorOccurred(SocketError)), this, SLOT(onSocketError(SocketError)));

    disconnect(this, SIGNAL(pollEvent()), this, SLOT(onPollEvent()));

    m_buf.close();
    delete m_sock;
    m_sock = nullptr;
}

void TcpConn::init()
{
    connect(m_sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::QueuedConnection);
    connect(m_sock, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(m_sock, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));

    connect(this, SIGNAL(pollEvent()), this, SLOT(onPollEvent()), Qt::QueuedConnection);
    m_buf.open(QIODevice::ReadWrite);
}

bool TcpConn::connectToHost(const QString& ip, const quint16 port)
{
    // ipv4 support only
    if (!IPv4::isValid(ip))
        return false;

    m_sock->connectToHost(QHostAddress(ip), port);
    return true;
}

void TcpConn::disconnectFromHost()
{
    if (!isValid())
        return;

    onPollEvent();
    m_sock->disconnectFromHost();
}

void TcpConn::close()
{
    m_chRead.clear();
    m_chWrite.clear();
    m_chReadedMsg.clear();
    m_chPreReadMsg.clear();
    m_chWritedMsg.clear();

    m_sock->close();
}

// void TcpConn::cancelWrite(Message* msg)
// {
//     Message* elem = nullptr;
//     for (int i = 0; i < m_chPreWriteMsg.capa() && m_chPreWriteMsg.tryDequeue(elem); ++i)
//     {
//         if (elem == msg)
//             continue;

//         m_chPreWriteMsg << elem;
//     }
// }

void TcpConn::read(Message* msg, quint64 ms, quint16 retry)
{
    m_chPreReadMsg << msg;

    msg = nullptr;
    quint16 count = 0;
    do {
        ++count;
    } while(!m_chReadedMsg.tryDequeue(msg, ms) && count <= retry);
}

void TcpConn::write(Message* msg)
{
    m_chPreWriteMsg << msg;
}

void TcpConn::read(QByteArray& buf, quint64 ms, quint16 retry)
{
    quint16 count = 0;
    do {
        ++count;
    } while(!m_chRead.tryDequeue(buf, ms) && count < retry);
}

void TcpConn::write(QByteArray& buf)
{
    m_chWrite << buf;
}

void TcpConn::onReadyRead()
{
    if (!isValid())
        return;

    // read bytes channel
    auto buf = m_sock->readAll();
    if (buf.isEmpty())
        return;
    m_chRead << buf;
    emit readed(this);
}

void TcpConn::onSocketError(QAbstractSocket::SocketError err)
{

}

void TcpConn::onStateChanged(QAbstractSocket::SocketState stat)
{
    switch (stat)
    {
    case SocketState::UnconnectedState: { break; }
    case SocketState::HostLookupState: { break; }
    case SocketState::ConnectingState: { break; }
    case SocketState::ConnectedState: { poll(); break; }
    case SocketState::BoundState: { break; }
    case SocketState::ListeningState: { break; }
    case SocketState::ClosingState: { break; }
    }
}

void TcpConn::onPollEvent()
{
    if (!isValid())
        return;

    // pre read msg channel
    Message* msg = nullptr;
    do {
        if (msg == nullptr && !m_chPreReadMsg.tryDequeue(msg))
            break;

        if (msg == nullptr)
            break;

        if (msg->size() > m_buf.size())
        {
            QByteArray buf;
            if (!m_chRead.tryDequeue(buf) || buf.isEmpty())
            {
                m_chPreReadMsg << msg; // put back
                break;
            }

            m_buf.write(buf);
            continue;
        }

        QByteArray data = m_buf.read(msg->size());
        if (msg->decode(data) < 1)
            break;

        m_chReadedMsg << msg;
        msg = nullptr;
    } while(true);


    // pre write msg channel
    do {
        Message* msg = nullptr;
        if (!m_chPreWriteMsg.tryDequeue(msg) || msg == nullptr)
            break;

        QByteArray data;
        msg->encode(data);
        m_chWrite << data;
        m_chWritedMsg << msg;
    } while(true);

    // write bytes channel
    do {
        QByteArray data;
        if (!m_chWrite.tryDequeue(data) || data.isEmpty())
            break;

        if (m_sock->write(data) < 1)
            break;

        emit writed(this);
    } while(true);

    // flush socket
    if (m_sock != nullptr)
        m_sock->flush();
}
