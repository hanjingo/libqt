#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <thread>
#include <chrono>

#include <libqt/db/dbconnpool.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "DB example";

    static qint64 id;
    DBConnPool::instance()->setFactoryFn([]()->QSqlDatabase{
        id++;
        auto db = QSqlDatabase::addDatabase("QSQLITE", QString::number(id));
        db.setDatabaseName("./007.db");
        db.open();
        qDebug() << "sqlite conn build";
        return db;
    });

    DBConnPool pool;
    pool.setFactoryFn([]()->QSqlDatabase{
        return QSqlDatabase();
    });
    DBConnPool::instance()->setConnNum(2);
    DBConnPool::instance()->setThreadPool(QThreadPool::globalInstance());

    auto db1 = DBConnPool::instance()->acquire(100);
    qDebug() << "db1.isValid()=" << db1.isValid();

    auto db2 = DBConnPool::instance()->acquire(100);
    qDebug() << "db2.isValid()=" << db2.isValid();

    QDateTime tmStart = QDateTime::currentDateTime();
    auto db3 = DBConnPool::instance()->acquire(100);
    QDateTime tmEnd = QDateTime::currentDateTime();
    qDebug() << "db3.isValid()=" << db3.isValid()
             << ", time passed=" << tmStart.msecsTo(tmEnd);

    std::thread([](){
        auto db4 = DBConnPool::instance()->acquire(100);
        qDebug() << "db4.isValid()=" << db4.isValid();
    }).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    DBConnPool::instance()->giveback(std::move(db1));

    return a.exec();
}
