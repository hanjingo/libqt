#include <QCoreApplication>
#include <QDebug>

#include <libqt/core/defer.h>

void fn1(){ qDebug() << "fn1"; }
void fn2(){ qDebug() << "fn2"; }
void fn3(){ qDebug() << "fn3"; }

void test1()
{
    DEFER(
        DEFER(
            fn3();
            )

        fn2();
    )

    fn1();
}

int conn() { int fd = 1; qDebug() << "conn with fd=" << fd; return fd; }
void free(int fd) { qDebug() << "auto free with fd=" << fd; }

void test2()
{
    auto fd = conn();
    DEFER(
        free(fd);
    )

    if (fd > 0)
    {
        qDebug() << "use fd but forget to release, return!";
        return;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "defer example";

    test1();

    qDebug() << "\n";
    test2();

    return a.exec();
}
