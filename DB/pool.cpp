#include "pool.h"

#include <QDateTime>

DBPool::DBPool(QObject* p)
    : QObject(p)
    , m_rwlock{}
    , m_capa{0}
    , m_fCreateDB{[]() -> QSqlDatabase { return QSqlDatabase(); }}
    , m_fCheckDB{[](DBPool*) -> bool { return true; }}
{
}

DBPool::DBPool(FCreateDB&& fCreate, FCheckDB&& fCheck, QObject* p)
    : QObject(p)
    , m_rwlock{}
    , m_capa{0}
    , m_fCreateDB{std::move(fCreate)}
    , m_fCheckDB{std::move(fCheck)}
{
}

DBPool::DBPool(const qint64 initNum, FCreateDB&& fCreate, FCheckDB&& fCheck, QObject* p)
    : QObject(p)
    , m_rwlock{}
    , m_capa{initNum}
    , m_fCreateDB{std::move(fCreate)}
    , m_fCheckDB{std::move(fCheck)}
{
    for (qint64 i = 0; i < initNum; ++i)
        addDB();
}

DBPool::~DBPool()
{
    while (!m_dbQueue.empty())
    {
        removeDB();
    }

    while (m_dbNum > 0)
    {
        msleep(100);
        removeDB();
    }
}

bool DBPool::empty()
{
    m_rwlock.lockForRead();
    bool ret = m_dbQueue.empty();
    m_rwlock.unlock();
    return ret;
}

bool DBPool::full()
{
    m_rwlock.lockForRead();
    bool ret = (m_dbQueue.size() == m_capa);
    m_rwlock.unlock();
    return ret;
}

qint64 DBPool::count()
{
    m_rwlock.lockForRead();
    qint64 ret = m_dbQueue.size()
    m_rwlock.unlock();
    return ret;
}

void DBPool::setCreateDBFn(FCreateDB&& fn)
{
    m_rwlock.lockForWrite();
    m_fCreateDB = std::move(fn);
    m_rwlock.unlock();
}

void DBPool::setCheckDBFn(FCheckDB&& fn)
{
    m_rwlock.lockForWrite();
    m_fCheckDB = std::move(fn);
    m_rwlock.unlock();
}

qint64 DBPool::getCapa()
{
    m_rwlock.lockForRead();
    qint64 ret = m_capa;
    m_rwlock.unlock();
    return ret;
}

void DBPool::setCapa(const qint64 sz)
{
    m_rwlock.lockForWrite();
    m_capa = sz;
    removeDB();
    m_rwlock.unlock();
}

QSqlDatabase DBPool::getDB(qint32 durMs)
{    
    m_rwlock.lockForWrite();
    sentinel();
    if (!m_dbQueue.empty())
    {
        QSqlDatabase db = m_dbQueue.dequeue();
        m_rwlock.unlock();
        return db;
    }
    
    if (addDB())
    {
        QSqlDatabase db = m_dbQueue.dequeue();
        m_rwlock.unlock();
        return db;
    }    
    m_rwlock.unlock();

    std::unique_lock<std::mutex> lock(m_cond.mutex());
    m_cond.wait_for(lock, std::chrono::milliseconds(durMs));

    m_rwlock.lockForWrite();
    if (m_dbQueue.empty())
    {
        m_rwlock.unlock();
        return QSqlDatabase();
    }
    QSqlDatabase db = m_dbQueue.dequeue();
    m_rwlock.unlock();
    return db;
}

void DBPool::putDB(QSqlDatabase db)
{
    m_rwlock.lockForWrite();
    if (db.isValid())
    {
        m_dbQueue.enqueue(db);
        while (m_dbNum > m_capa)
            removeDB();

        m_cond.notify_all();
    }
    m_rwlock.unlock();
}

QDateTime DBPool::getLastUseTime()
{
    return m_lastUseTime;
}

bool DBPool::addDB()
{
    if (m_dbNum > m_capa)
        return false;

    QSqlDatabase db = m_createConn();
    if (!db.isValid())
        return false;
    
    m_dbQueue.enqueue(db);
    m_dbNum++;
    while (m_dbNum > m_capa)
        removeDB();

    m_cond.notify_all();
    emit sigDBCreated();
    return true;
}

void DBPool::removeDB()
{
    if (m_dbQueue.empty())
        return false;

    QSqlDatabase db = m_dbQueue.dequeue();
    if (db.isOpen())
        db.close();
    m_dbNum--;

    emit sigDBClosed();
    return true;
}

void DBPool::sentinel()
{
    if (m_fCheckDB(this))
    {
        m_lastTime = QDateTime::currentDateTime();
        return;
    }

    removeDB();
    addDB();
    sentinel();
}