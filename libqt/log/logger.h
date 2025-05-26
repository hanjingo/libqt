#ifndef LOGGER_H
#define LOGGER_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QObject>
#include <QDebug>
#include <QString>
#include <QReadWriteLock>

#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

#ifndef LOG_QUEUE_SIZE
#define LOG_QUEUE_SIZE 102400
#endif

#ifndef LOG_THREAD_NUM
#define LOG_THREAD_NUM 1
#endif

class Logger;
void logHandleMsg(QtMsgType typ, const QMessageLogContext& ctx, const QString& str);

enum LogLvl {
    LOG_LVL_TRACE = 0,
    LOG_LVL_DEBUG,
    LOG_LVL_INFO,
    LOG_LVL_WARN,
    LOG_LVL_ERR,
    LOG_LVL_CRITICAL,
};

class Logger : public QObject
{
    Q_OBJECT

public:
    using SinkPtr = spdlog::sink_ptr;
    using BaseLoggerPtr = std::shared_ptr<spdlog::logger>;

public:
    Logger() = delete;

    Logger(BaseLoggerPtr base, QObject* p = Q_NULLPTR) 
        : QObject(p)
        , m_base{base}
    {}

    Logger(const QString& name, 
           const bool async = false, 
           const uint64_t queue_size = LOG_QUEUE_SIZE, 
           const uint64_t thread_num = LOG_THREAD_NUM,
           QObject* p = Q_NULLPTR)
        : QObject(p)
    {
        std::vector<SinkPtr> sinks{};

        if (async)
        {
            spdlog::init_thread_pool(queue_size, thread_num);

            m_base = std::make_shared<spdlog::async_logger>(
                std::string(name.toUtf8().constData()),
                sinks.begin(), 
                sinks.end(),
                spdlog::thread_pool(), 
                spdlog::async_overflow_policy::overrun_oldest);
        }
        else
        {
            m_base = std::make_shared<spdlog::logger>(
                std::string(name.toUtf8().constData()), 
                sinks.begin(), 
                sinks.end());
        }

        m_base->set_level(spdlog::level::level_enum(LOG_LVL_TRACE));
    }

    ~Logger()
    {
        flush();
        spdlog::drop_all();
    }

    static Logger* instance()
    {
        static Logger inst{spdlog::default_logger()};
        return &inst;
    }

    static SinkPtr createStdoutSink()
    {
        return std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }

    static SinkPtr createRotateFileSink(const QString& baseFileName,
                                        const quint64 max_size,
                                        const quint64 max_files,
                                        const bool rotate_on_open = false)
    {
        return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            std::string(baseFileName.toUtf8().constData()), max_size, max_files, rotate_on_open);
    }

    static SinkPtr createDailyFileSink(const QString& baseFileName,
                                          const quint64 rotation_hour,
                                          const quint64 rotation_minute,
                                          const bool truncate = false,
                                          const uint16_t max_files = 0)
    {
        return std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            std::string(baseFileName.toUtf8().constData()), rotation_hour, rotation_minute, truncate, max_files);
    }

    static void catchQtMsg()
    {
        qInstallMessageHandler(logHandleMsg);
    }

    inline const QString name() { return QString(m_base->name().c_str()); }

    inline void addSink(const SinkPtr&& sink)
    {
        m_lock.lockForWrite();
        m_base->sinks().push_back(std::move(sink));
        m_lock.unlock();
    }

    inline void removeSink(const SinkPtr sink) {
        m_lock.lockForWrite();
        m_base->sinks().erase(std::remove(m_base->sinks().begin(), m_base->sinks().end(), sink), m_base->sinks().end());
        m_lock.unlock();
    }

    inline void clearSink()
    {
        m_lock.lockForWrite();
        m_base->sinks().clear();
        m_lock.unlock();
    }
    
    inline void setLevel(const LogLvl lvl)
    {
        m_base->set_level(spdlog::level::level_enum(lvl));
    }

    inline const LogLvl getLevel()
    {
        return static_cast<LogLvl>(m_base->level());
    }

    // see also: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    inline void setPattern(const char* patterm)
    {
        m_base->set_pattern(patterm);
    }

    inline void flush()
    {
        m_base->flush();
    }

    inline void flushOn(const LogLvl lvl)
    {
        m_base->flush_on(spdlog::level::level_enum(lvl));
    }

public:
    template<typename... Args>
    inline void trace(const char* fmt, Args&& ... args)
    {
        m_base->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void debug(const char* fmt, Args&& ... args)
    {
        m_base->debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void info(const char* fmt, Args&& ... args)
    {
        m_base->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void warn(const char* fmt, Args&& ... args)
    {
        m_base->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void error(const char* fmt, Args&& ... args)
    {
        m_base->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(const char* fmt, Args&& ... args)
    {
        m_base->critical(fmt, std::forward<Args>(args)...);
    }

private:
    Q_DISABLE_COPY_MOVE(Logger)

    BaseLoggerPtr  m_base;
    QReadWriteLock m_lock;
};

static void logHandleMsg(QtMsgType typ, const QMessageLogContext& ctx, const QString& str)
{
    switch (typ)
    {
    case QtDebugMsg: { Logger::instance()->debug(str.toUtf8().constData()); break; }
    case QtWarningMsg: { Logger::instance()->warn(str.toUtf8().constData()); break; }
    case QtCriticalMsg: { Logger::instance()->critical(str.toUtf8().constData()); break; }
    case QtInfoMsg: { Logger::instance()->info(str.toUtf8().constData()); break; }
    default: break;
    }
}

#define LOG_TRACE(...) \
    Logger::instance()->trace(__VA_ARGS__)

#define LOG_DEBUG(...) \
    Logger::instance()->debug(__VA_ARGS__)

#define LOG_INFO(...) \
    Logger::instance()->info(__VA_ARGS__)

#define LOG_WARN(...) \
    Logger::instance()->warn(__VA_ARGS__)

#define LOG_ERROR(...) \
    Logger::instance()->error(__VA_ARGS__)

#define LOG_CRITICAL(...) \
    Logger::instance()->critical(__VA_ARGS__)

#define LOG_FLUSH() \
    Logger::instance()->flush();

#endif