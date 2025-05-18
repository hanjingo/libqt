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
    Channel(const int sz = 1)
        : m_data{}, m_sem{sz}, m_capa{sz}
    {
        m_data.reserve(sz);
        for (auto i = 0; i < sz; ++i)
            m_sem.acquire(1);
    }
    ~Channel()
    {
        clear();
    }

    inline Channel& operator>>(T& t) { dequeue(t); return *this; }
    inline Channel& operator<<(const T& t) { enqueue(t); return *this; }
    inline int available() { return m_sem.available(); }
    inline int capa() { return m_capa; }

    inline void enqueue(const T& t)
    {
        m_sem.release(1);
        m_data.enqueue(t);
    }

    inline void enqueue(T&& t)
    {
        m_sem.release(1);
        m_data.enqueue(std::move(t));
    }

    inline void dequeue(T& t)
    {
        m_sem.acquire(1);
        t = m_data.dequeue();
    }

    inline bool tryDequeue(T& t)
    {
        if (!m_sem.tryAcquire(1))
            return false;

        t = m_data.dequeue();
        return true;
    }

    inline bool tryDequeue(T& t, const int ms)
    {
        if (!m_sem.tryAcquire(1, ms))
            return false;

        t = m_data.dequeue();
        return true;
    }

    inline void clear()
    {
        T t;
        while(tryDequeue(t)){}
    }

private:
    Q_DISABLE_COPY(Channel)

    QQueue<T>              m_data;
    QSemaphore             m_sem;
    int                    m_capa;
};

#endif
