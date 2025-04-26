#ifndef OPERATOR_H
#define OPERATOR_H

#include <functional>
#include <QObject>
#include <QString>
#include <QThreadPool>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class DBOperator : public QObject
{
    Q_OBJECT
public:
    using FQueryCB = std::function<void(QSqlQuery&)>;

public:
    DBOperator(QThreadPool& pool) : m_pool{pool} {}
    ~DBOperator(){}

    bool open(QSqlDatabase& db);
    void close(QSqlDatabase& db);
    void exec(QSqlDatabase& db, const QString& sql, FQueryCB&& fn);
    void transaction(QSqlDatabase& db);
    bool commit(QSqlDatabase& db);
    bool rollback(QSqlDatabase& db);

public signals:
    void sigDBLastError(QString err);

private:
    QThreadPool& m_pool;
};

#endif