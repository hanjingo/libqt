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
    Channel(const int sz = 1) : m_data{}, m_sem{sz}, m_capa{sz}
    {
        m_data.reserve(sz);
        for (auto i = 0; i < sz; ++i)
            m_sem.acquire(1);
    }
    ~Channel()
    {}

    inline Channel& operator>>(T& t) { dequeue(t); return *this; }
    inline Channel& operator<<(const T& t) { enqueue(t); return *this; }
    inline int available() { return m_sem.available(); }
    inline int capa() { return m_capa; }

    bool enqueue(const T& t)
    {
        m_sem.release(1);
        m_data.enqueue(t);
        return true;
    }

    bool dequeue(T& t)
    {
        m_sem.acquire(1);
        t = m_data.dequeue();
        return true;
    }

    bool tryDequeue(T& t)
    {
        if (!m_sem.tryAcquire(1))
            return false;

        t = m_data.dequeue();
        return true;
    }

    bool tryDequeue(T& t, const int ms)
    {
        if (!m_sem.tryAcquire(1, ms))
            return false;

        t = m_data.dequeue();
        return true;
    }

    void clear()
    {
        for (int i = 0; i < m_capa; ++i)
        {
            if (!m_sem.tryAcquire(1))
                return;
            m_data.dequeue();
        }
    }

private:
    QQueue<T>              m_data;
    QSemaphore             m_sem;
    int                    m_capa;
};

#endif
