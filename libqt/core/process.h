#ifndef PROCESS_H
#define PROCESS_H

#include <QVector>
#include <QProcess>
#include <QCoreApplication>
#include <QRegularExpression>
#include <functional>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

class Process : public QProcess
{
    Q_OBJECT

  public:
    explicit Process(QObject *parent = nullptr)
        : QProcess(parent){};

    static qint64 ppid()
    {
#if defined(Q_OS_WIN)
        qint64 ppid  = 0;
        DWORD  pid   = static_cast<DWORD>(QCoreApplication::applicationPid());
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if(hSnap == INVALID_HANDLE_VALUE)
            return ppid;
        PROCESSENTRY32 pe = {sizeof(pe)};
        if(Process32First(hSnap, &pe))
        {
            do
            {
                if(pe.th32ProcessID == pid)
                {
                    ppid = static_cast<qint64>(pe.th32ParentProcessID);
                    break;
                }
            } while(Process32Next(hSnap, &pe));
        }
        CloseHandle(hSnap);
        return ppid;
#else
        return static_cast<qint64>(::getppid());
#endif
    }

    static void kill(qint64 pid)
    {
#if defined(Q_OS_WIN)
        HANDLE hProcess =
            OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));
        if(hProcess)
        {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
        }
#else
        ::kill(static_cast<pid_t>(pid), SIGKILL);
#endif
    }

    static qint64 system(const QString &cmd)
    {
        return QProcess::execute(cmd);
    }

    static void spawn(const QString &cmd)
    {
        QProcess::startDetached(cmd);
    }

    static void list(
        QVector<qint64>                         &result,
        std::function<bool(const QStringList &)> match =
            [](const QStringList &) { return true; })
    {
        QProcess proc;
#if defined(Q_OS_WIN)
        proc.start(QStringLiteral("tasklist"),
                   QStringList() << "/FO"
                                 << "CSV"
                                 << "/NH");
        proc.waitForFinished();
        const QStringList lines =
            QString::fromLocal8Bit(proc.readAllStandardOutput())
                .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        for(const QString &line : lines)
        {
            QStringList cols = line.split(QLatin1Char(','));
            for(auto &c : cols)
                c = c.trimmed().remove(QLatin1Char('"'));
            if(cols.size() < 2 || !match(cols))
                continue;
            bool         ok  = false;
            const qint64 pid = cols[1].toLongLong(&ok);
            if(ok)
                result.append(pid);
        }
#else
        proc.start(QStringLiteral("ps"),
                   QStringList() << "-eo"
                                 << "comm,pid");
        proc.waitForFinished();
        const QStringList lines =
            QString::fromLocal8Bit(proc.readAllStandardOutput())
                .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        bool firstLine = true;
        for(const QString &line : lines)
        {
            if(firstLine)
            {
                firstLine = false;
                continue;
            }
            const QStringList cols =
                line.trimmed().split(QRegularExpression(QStringLiteral("\\s+")),
                                     Qt::SkipEmptyParts);
            if(cols.size() < 2 || !match(cols))
                continue;
            bool         ok  = false;
            const qint64 pid = cols[1].toLongLong(&ok);
            if(ok)
                result.append(pid);
        }
#endif
    }
};

#endif // PROCESS_H