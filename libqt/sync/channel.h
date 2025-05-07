#ifndef CHANNEL_H
#define CHANNEL_H

#include <QAtomicInteger>
#include <QSemaphore>
#include <QQueue>
#include <QDebug>

template <typename T>
class Channel
{
public:
    Channel(const int sz = 1) : m_data{}, m_sem{sz}, m_bClosed{0}
    {
        m_data.reserve(sz);
        for (auto i = 0; i < sz; ++i)
            m_sem.acquire(1);
    }
    ~Channel()
    {
    }

    inline Channel& operator>>(T& t)
    {
        dequeue(t);
        return *this;
    }

    inline Channel& operator<<(const T& t)
    {
        enqueue(t);
        return *this;
    }

    inline bool isClosed() { return m_bClosed.load() == 1; }

    void close()
    {
        if (m_bClosed.load() == 1)
            return;

        m_bClosed.store(1);
    }

    bool enqueue(const T& t)
    {
        qDebug() << "enqueue start";
        if (isClosed())
            return false;

        m_sem.release(1);
        m_data.enqueue(t);
        qDebug() << "enqueue end";
        return true;
    }

    bool dequeue(T& t)
    {
        if (isClosed())
            return false;

        m_sem.acquire(1);
        t = m_data.dequeue();
        return true;
    }

    bool tryDequeue(T& t)
    {
        if (isClosed() || !m_sem.tryAcquire(1))
            return false;

        t = m_data.dequeue();
        return true;
    }

    bool tryDequeue(T& t, const int usec)
    {
        if (isClosed() || !m_sem.tryAcquire(1, usec))
            return false;

        t = m_data.dequeue();
        return true;
    }

private:
    QAtomicInteger<quint8> m_bClosed;
    QQueue<T>              m_data;
    QSemaphore             m_sem;
};

#endif
