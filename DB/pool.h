#ifndef POOL_H
#define POOL_H

#include <functional>
#include <condition_variable>

#include <QtSql/QSqlDatabase>
#include <QQueue>
#include <QReadWriteLock>
#include <QObject>
#include <QDateTime>

class DBPool : public QObject
{
    QOBJECT
public:
    using FCreateDB = std::function<QSqlDatabase()>;
    using FCheckDB = std::function<bool(DBPool*)>;

public:
    DBPool(QObject* p = Q_NULLPTR);
    DBPool(FCreateDB&& fCreate, FIsValidDB&& fCheck, QObject* p = Q_NULLPTR);
    DBPool::DBPool(const qint64 initNum, 
                   FCreateDB&& fCreate, 
                   FCheckDB&& fCheck, 
                   QObject* p = Q_NULLPTR);
    ~DBPool();

    bool empty();
    bool full();
    qint64 count();
    void setCreateDBFn(FCreateDB&& fn);
    void setCheckDBFn(FCheckDB&& fn);
    qint64 getCapa();
    void setCapa(const qint64 sz);
    QSqlDatabase getDB(qint32 durMs = 0);
    void putDB(QSqlDatabase db);
    QDateTime getLastUseTime();

public signals:
    void sigDBCreated();
    void sigDBClosed();

private:
    bool addDB();
    bool removeDB();
    void sentinel();

private:
    QReadWriteLock          m_rwlock;
    qint64                  m_capa;
    qint64                  m_dbNum;
    QQueue<QSqlDatabase>    m_dbQueue;
    QDateTime               m_lastUseTime;
    std::condition_variable m_cond;

    FCreateDB               m_fCreateDB;
    FIsValidDB              m_fCheckDB;
};

#endif