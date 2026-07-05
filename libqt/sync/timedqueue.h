#ifndef TIMED_QUEUE_H
#define TIMED_QUEUE_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <functional>

class TimedQueue : public QObject
{
    Q_OBJECT
  public:
    explicit TimedQueue(int      intervalMs,
                        int      maxQueueSize = 1024,
                        QObject *parent       = nullptr)
        : QObject(parent)
        , m_maxQueueSize(maxQueueSize)
        , m_interval(intervalMs)
    {
        m_timer.setInterval(m_interval);
        connect(&m_timer, &QTimer::timeout, this, &TimedQueue::consume);
    }

    static TimedQueue &Instance()
    {
        static TimedQueue instance(100, 1024);
        return instance;
    }

    void enqueue(std::function<void()> task)
    {
        int dropped = 0;
        while(m_queue.size() >= m_maxQueueSize)
        {
            m_queue.dequeue();
            dropped++;
        }
        if(dropped > 0)
            emit signalQueueOverflow(dropped);

        m_queue.enqueue(task);
    }
    void setInterval(int ms)
    {
        m_interval = ms;
        m_timer.setInterval(ms);
    }
    void start(int ms)
    {
        if(!m_timer.isActive())
        {
            setInterval(ms);
            m_timer.start();
        }
    }
    void stop() { m_timer.stop(); }
    void clear()
    {
        m_queue.clear();
        m_timer.stop();
    }
    int queueSize() const { return m_queue.size(); }

  signals:
    void signalQueueOverflow(int droppedCount);

  private slots:
    void consume()
    {
        if(m_queue.isEmpty())
            return;

        auto task = m_queue.dequeue();
        task();
    }

  private:
    QQueue<std::function<void()>> m_queue;
    QTimer                        m_timer;
    int                           m_maxQueueSize;
    int                           m_interval;
};

#endif // TIMED_QUEUE_H