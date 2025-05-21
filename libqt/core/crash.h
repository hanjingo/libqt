#ifndef CRASH_H
#define CRASH_H

#include <QCoreApplication>
#include <QObject>
#include <QEvent>
#include <QFile>
#include <QTextStream>

#include <functional>

/*
    // crash handler
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QByteArray localMsg = msg.toLocal8Bit();
        QFile outFile(QDir(QCoreApplication::applicationDirPath()).filePath(QString("livermore-qt_crash_%1.log").arg(QDateTime::currentMSecsSinceEpoch())));
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << context.file << ":" << context.line << ": " << localMsg << "\n";
    });
*/

class CrashMonitor : public QAbstractEventDispatcher
{
public:
    CrashMonitor(QObject *parent = nullptr)
        : QAbstractEventDispatcher(parent)
    {}
    ~CrashMonitor()
    {}

    bool processEvents(QEventLoop::ProcessEventsFlags flags) override
    {
        if (flags & QEventLoop::EventLoopExec && QCoreApplication::instance()->isEventDriven())
        {
            QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
            QCoreApplication::sendPostedEvents(nullptr, QEvent::User);
            return true;
        }
        return false;
    }

    void installCrashHandler()
    {
        setEventHandler(this);
        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
            Q_UNUSED(context);
            if (type == QtFatalMsg) {
                QFile outFile("fatal.log");
                outFile.open(std::ios::append);
                QTextStream ts(&outFile);
                ts << msg << "\n";
                // 在这里添加生成dump的代码，例如使用MiniDumpWriteDump等API（Windows平台）
                qFatal("Exiting due to fatal error."); // 强制退出程序
            }
        });
    }
};

#endif
