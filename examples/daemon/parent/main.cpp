#include <QCoreApplication>
#include <QDebug>
#include <QThread>

#include <libqt/core/process.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "parent running...";
#if defined(Q_OS_WIN)
    const QString program = QStringLiteral("child.exe");
#else
    const QString program = QStringLiteral("./child");
#endif
    Process::spawn(program);

    QThread::sleep(3);
    QVector<qint64> pids;
    Process::list(pids, [](const QStringList &pnames) -> bool {
        for(const QString &pname : pnames)
        {
            if(pname.contains("child", Qt::CaseInsensitive))
                return true;
        }
        return false;
    });
    qDebug() << pids;
    for(auto pid : pids)
    {
        qDebug() << "kill " << pid << " before";
        Process::kill(pid);
        qDebug() << "kill " << pid << " after";
    }

    QThread::sleep(1);
    qDebug() << "parent exit...";

    return a.exec();
}
