#ifndef EVENT_H
#define EVENT_H

#include <QEvent>
#include <QMetaType>

#include <QVariant>

#ifndef EVENT_PARAM_N
#define EVENT_PARAM_N 20
#endif

class Event : public QEvent
{
public:
    Event();
    Event(const Event& other);

    bool operator==(const Event &other) const;

    inline QVariant& get(const int idx) { return m_params[idx]; }
    inline void set(const int idx, const QVariant& value) { m_params[idx] = value; }

private:
    static const Type UserEventType;

    QVariant m_params[EVENT_PARAM_N];
};

Q_DECLARE_METATYPE(Event)

#endif
