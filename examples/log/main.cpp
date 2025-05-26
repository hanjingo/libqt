#include <QCoreApplication>

#include <libqt/log/logger.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // do logging by default logger
    Logger::instance()->setLevel(LOG_LVL_TRACE);
    LOG_TRACE("instance LOG_TRACE {}", "hello");
    LOG_DEBUG("instance LOG_DEBUG d{0:d} 0x{0:x} o{0:o} b{0:b}", 10);
    LOG_INFO("instance LOG_INFO {0:03.2f}", 123.45);
    LOG_WARN("instance LOG_WARN {1} {0}", "hello", "world");
    LOG_ERROR("instance LOG_ERROR");
    LOG_CRITICAL("instance LOG_CRITICAL");
    LOG_FLUSH();

    // do sync logging by user defined rotate file logger
    auto lg1 = std::make_shared<Logger>("lg1");
    lg1->addSink(Logger::createStdoutSink());
    lg1->addSink(Logger::createRotateFileSink("logs/lg1.log",
        2 * 1024 * 1024,
        2,
        false));
    lg1->setPattern("%Y-%m-%d %H:%M:%S.%e [%-8l] %v");
    lg1->trace("lg1 LOG_TRACE {}", "hello");
    lg1->debug("lg1 LOG_DEBUG d{0:d} 0x{0:x} o{0:o} b{0:b}", 10);
    lg1->info("lg1 LOG_INFO {0:03.2f}", 123.45);
    lg1->warn("lg1 LOG_WARN {1} {0}", "hello", "world");
    lg1->error("lg1 LOG_ERROR");
    lg1->critical("lg1 LOG_CRITICAL");
    lg1->flush();

    // do sync logging by user defined daily logger
    auto lg2 = std::make_shared<Logger>("lg2", true);
    lg2->addSink(Logger::createStdoutSink());
    lg2->clearSink();
    lg2->addSink(Logger::createDailyFileSink("logs/lg2.log",
        0,
        1));
    for (int i = 0; i < 5; ++i)
    {
        lg2->trace("lg2 LOG_TRACE {}", "hello");
        lg2->debug("lg2 LOG_DEBUG d{0:d} 0x{0:x} o{0:o} b{0:b}", 10);
        lg2->info("lg2 LOG_INFO {0:03.2f}", 123.45);
        lg2->warn("lg2 LOG_WARN {1} {0}", "hello", "world");
        lg2->error("lg2 LOG_ERROR");
        lg2->critical("lg2 LOG_CRITICAL");
        lg2->flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // catch qt output
    Logger::catchQtMsg();
    qDebug() << "qDebug";
    qWarning() << "qWarning";
    qCritical() << "qCritical";
    qInfo() << "qInfo";

    return a.exec();
}
