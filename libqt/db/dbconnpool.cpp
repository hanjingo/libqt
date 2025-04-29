#include "dbconnpool.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DBConnPool::DBConnPool(QObject* p)
    : QObject(p)
    , m_threads(nullptr)
    , m_factory([]()->QSqlDatabase{ return QSqlDatabase(); })
{
}

DBConnPool::DBConnPool(const std::size_t connNum, const FFactory&& factory, QThreadPool* threads, QObject* p)
    : QObject(p)
    , m_threads(threads)
    , m_factory(std::move(factory))
{
    this->setConnNum(connNum);
}

DBConnPool::~DBConnPool()
{
    m_cond.notify_all();
    while (!m_conns.isEmpty())
    {
        auto conn = m_conns.dequeue();
        if (conn.isOpen())
            conn.close();
    }
}

void DBConnPool::setFactoryFn(const FFactory&& fn)
{
    std::lock_guard<std::mutex> lock{m_lock};
    m_factory = std::move(fn);
}

void DBConnPool::setConnNum(const std::size_t num)
{
    std::lock_guard<std::mutex> lock{m_lock};
    while (m_conns.size() < num)
    {
        QSqlDatabase db = m_factory();
        if (!db.isOpen() && !db.open())
        {
            emit sigLastError(db.lastError().text());
            continue;
        }

        m_conns.enqueue(db);
    }

    while (m_conns.size() > num)
    {
        auto conn = m_conns.dequeue();
        if (conn.isOpen())
            conn.close();
    }
}

void DBConnPool::setThreadPool(QThreadPool* threads)
{
    std::lock_guard<std::mutex> lock{m_lock};
    m_threads = threads;
}

void DBConnPool::exec(const QString& sql, const FQuery&& fn)
{
    auto db = acquire();
    if (!db.isOpen())
    {
        emit sigLastError("Database conn is invalid");
        return;
    }

    auto cb = std::move(fn);
    if (m_threads == nullptr)
    {
        QSqlQuery query{db};
        if (!query.exec(sql))
            emit sigLastError(query.lastError().text());

        cb(query);
        giveback(std::move(db));
        return;
    }

    m_threads->start([this, db, sql, cb](){
        QSqlQuery query{db};
        if (!query.exec(sql))
            emit sigLastError(query.lastError().text());

        cb(query);
        this->giveback(std::move(db));
    });
}

QSqlDatabase DBConnPool::acquire(const int timeout_ms)
{
    if (timeout_ms > 0 && m_conns.isEmpty())
    {
        std::unique_lock<std::mutex> lock{m_lock};
        bool ok = m_cond.wait_for(
            lock,
            std::chrono::milliseconds(timeout_ms),
            [this]{ return !this->m_conns.empty(); }
        );
        if (!ok)
            return QSqlDatabase();
    }

    std::lock_guard<std::mutex> lock{m_lock};
    if (m_conns.isEmpty())
        return QSqlDatabase();

    do {
        auto conn = m_conns.dequeue();
        if (conn.isOpen())
            return conn;

        QSqlDatabase::removeDatabase(conn.connectionName());
        m_conns.enqueue(m_factory());
    } while (true);
}

void DBConnPool::giveback(const QSqlDatabase&& db)
{
    std::lock_guard<std::mutex> lock{m_lock};
    m_conns.enqueue(db);
    m_cond.notify_one();
}
