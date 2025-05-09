#ifndef RWLOCKER_H
#define RWLOCKER_H

#include <QReadWriteLock>

class WLocker{
public:
    WLocker(QReadWriteLock& lock)
        :m_lock(lock)
    {
        m_lock.lockForWrite();
    }

    ~WLocker()
    {
        m_lock.unlock();
    }

private:
    QReadWriteLock& m_lock;
};

class RLocker{
public:
    RLocker(QReadWriteLock& lock)
        : m_lock(lock)
    {
        m_lock.lockForRead();
    }

    ~RLocker()
    {
        m_lock.unlock();
    }

private:
    QReadWriteLock& m_lock;
};

#endif
