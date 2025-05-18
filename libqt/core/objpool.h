#ifndef OBJ_POOL_H
#define OBJ_POOL_H

#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QSemaphore>

template <typename Obj>
class ObjPool
{
public:
    ObjPool(const int sz) : m_container{}, m_sem{sz}, m_capa{sz}
    {
        m_container.reserve(sz);
        for (auto i = 0; i < sz; ++i)
            m_sem.acquire(1);
    }
    ~ObjPool()
    {}

    inline ObjPool& operator>>(Obj& obj) { obj = acquire(); return *this; }
    inline ObjPool& operator<<(const Obj&& obj) { giveback(std::move(obj)); return *this; }
    inline int size() { return m_sem.available(); }
    inline int capa() { return m_capa; }

    template<typename... Args>
    void construct(Args&& ... args)
    {
        Obj obj(std::forward<Args>(args)...);
        giveback(std::move(obj));
    }

    Obj acquire()
    {
        m_sem.acquire(1);
        return m_container.dequeue();
    }

    bool tryAcquire(Obj& obj)
    {
        if (!m_sem.tryAcquire(1))
            return false;

        obj = m_container.dequeue();
        return true;
    }

    bool tryAcquire(Obj& obj, const int ms)
    {
        if (!m_sem.tryAcquire(1, ms))
            return false;

        obj = m_container.dequeue();
        return true;
    }

    void giveback(const Obj&& obj)
    {
        m_sem.release(1);
        m_container.enqueue(std::move(obj));
    }

private:
    QQueue<Obj> m_container;
    QSemaphore  m_sem;
    int         m_capa;
};

#endif
