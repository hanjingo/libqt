#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QDateTime>

#include <libqt/core/objpool.h>

struct Person
{
    Person()
    {}
    Person(QString arg1, qint32 arg2, float arg3) : name{arg1}, age{arg2}, salary{arg3}
    {}
    ~Person()
    {}

    QString toString() { return QString("name:%1, age:%2, salary:%3").arg(name).arg(age).arg(salary); }

    QString  name;
    qint32   age;
    float    salary;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "objpool example";

    ObjPool<Person> pool{2};
    for (qint32 i = pool.size(); i < pool.capa(); ++i)
        pool.construct(QString("harry potter"), i, 10000.1);

    Person p1 = pool.acquire();
    qDebug() << "p1:" << p1.toString();

    Person p2 = pool.acquire();
    qDebug() << "p2:" << p2.toString();

    Person tmp;
    qDebug() << "pool.tryAcquire(tmp) = " << pool.tryAcquire(tmp);

    auto start = QDateTime::currentDateTime();
    auto ret = pool.tryAcquire(tmp, 500);
    auto end = QDateTime::currentDateTime();
    qDebug() << "pool.tryAcquire(tmp, 500) with ret=" << ret
             << ", time passed ms=" << start.msecsTo(end);

    pool.giveback(std::move(p1));
    pool.giveback(std::move(p2));
    Person p3 = pool.acquire();
    qDebug() << "p3:" << p1.toString();

    // producer
    ObjPool<Person> tPool{10};
    QThreadPool::globalInstance()->start([&tPool](){
        for (qint32 i = 0; i < 10; ++i)
        {
            tPool.construct(QString("harry potter"), i, 10000.1);
        }
    });

    // consumer1
    QThreadPool::globalInstance()->start([&tPool](){
        while (true)
        {
            auto p = tPool.acquire();
            qDebug() << "thread1 get obj:" << p.toString();
        }
    });

    // consumer2
    QThreadPool::globalInstance()->start([&tPool](){
        while (true)
        {
            auto p = tPool.acquire();
            qDebug() << "thread2 get obj:" << p.toString();
        }
    });

    return a.exec();
}
