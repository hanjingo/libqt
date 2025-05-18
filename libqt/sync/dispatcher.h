#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QEventLoop>
#include <QCoreApplication>

class Dispatcher
{
public:
    explicit Dispatcher()
    {}
    ~Dispatcher()
    {}

    static Dispatcher* instance()
    {
        static Dispatcher inst{};
        return &inst;
    }

    template <typename Fn, typename... Args>
    static auto post(const Fn& fn, Args&& ... args)
    {
        return QtConcurrent::run(std::move(fn), std::forward<Args>(args)...);
    }

    template <typename Ret>
    static auto wait(QFuture<Ret>& ctx, int ms)
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() != QThread::currentThread(),
                   "WARNING", "Not recommand use this function in main thread");

        ms = (ms < 1 || ms > 60000) ? 1 : ms;
        QFutureWatcher<Ret> watcher;
        QEventLoop loop;
        QTimer timer;

        timer.setSingleShot(true);
        QObject::connect(&watcher, &QFutureWatcher<Ret>::finished, &loop, &QEventLoop::quit);
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

        watcher.setFuture(ctx);
        timer.start(ms);
        loop.exec(QEventLoop::ExcludeUserInputEvents);
    }

private:
    Q_DISABLE_COPY(Dispatcher)
};

#endif
