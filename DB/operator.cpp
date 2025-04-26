#include "operator.h"

bool DBOperator::open(QSqlDatabase& db)
{
    if (db.isOpen())
        return true;

    if (!db.open())
    {
        emit sigDBLastError(db.lastError().databaseText());
        return false;
    }

    return true;
}

void DBOperator::close(QSqlDatabase& db)
{
    if (!db.isOpen())
        return;

    db.close();
}
    
void DBOperator::exec(QSqlDatabase& db, const QString& sql, FQueryCB&& fn)
{
    if (!db.isOpen())
    {
        emit sigDBLastError("Database is not open");
        return;
    }

    QSqlQuery query(db);
    m_pool.start([query, sql, fn](){
        if (!query.prepare(sql))
        {
            emit sigDBLastError(query.lastError().databaseText());
        }
        fn(query);
    });
}

bool DBOperator::transaction(QSqlDatabase& db)
{
    return db.transaction();
}

bool DBOperator::commit(QSqlDatabase& db)
{
    return db.commit();
}

bool DBOperator::rollBack(QSqlDatabase& db)
{
    return db.rollback();
}