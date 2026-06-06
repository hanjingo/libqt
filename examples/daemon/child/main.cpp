#include <QCoreApplication>
#include <QDebug>
#include <QThread>

#include <libqt/core/process.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "child start";
    while(true)
    {
        qDebug() << "child running...";
        QThread::sleep(1);
    }

    qDebug() << "child end";

    return a.exec();
}
