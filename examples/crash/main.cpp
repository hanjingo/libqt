#include <QCoreApplication>
#include <QDebug>

#include <libqt/core/crash.h>

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "crash example";

    // CrashHandler::preventSetUnhandledExceptionFilter();
    CrashHandler::instance()->setLocalPath(QCoreApplication::applicationDirPath());

    int* p = new int(1);
    p = nullptr;
    qDebug() << *p;

    return a.exec();
}
