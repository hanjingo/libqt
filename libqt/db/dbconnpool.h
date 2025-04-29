#ifndef DB_CONN_POOL_H
#define DB_CONN_POOL_H

#include <functional>
#include <condition_variable>
#include <mutex>

#include <QQueue>
#include <QObject>
#include <QPointer>
#include <QThreadPool>
#include <QReadWriteLock>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

class DBConnPool : public QObject
{
    Q_OBJECT

public:
    using FQuery = std::function<void(QSqlQuery&)>;
    using FFactory = std::function<QSqlDatabase()>;

public:
    DBConnPool(QObject* p = Q_NULLPTR);
    DBConnPool(const std::size_t connNum,
               const FFactory&& factory,
               QThreadPool* threads = nullptr,
               QObject* p = Q_NULLPTR);
    ~DBConnPool();

    static QPointer<DBConnPool> instance()
    {
        static QPointer<DBConnPool> inst(new DBConnPool());
        return inst;
    }

    void setFactoryFn(const FFactory&& fn);
    void setConnNum(const std::size_t num);
    void setThreadPool(QThreadPool* threads);
    void exec(const QString& sql, const FQuery&& fn);

    QSqlDatabase acquire(const int timeout_ms = 0);
    void giveback(const QSqlDatabase&& db);

signals:
    void sigLastError(QString err);

private:
    std::mutex                         m_lock;
    std::condition_variable            m_cond;
    QQueue<QSqlDatabase>               m_conns;
    QThreadPool*                       m_threads;
    FFactory                           m_factory;
};

#endif
