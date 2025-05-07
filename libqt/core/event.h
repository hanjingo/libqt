#ifndef EVENT_H
#define EVENT_H

#include <QEvent>
#include <QMetaType>
#include <QMap>

class Event : public QEvent
{
public:
    Event();
    Event(const Event& other);
	~Event();

    bool operator==(const Event &other) const;

private:
    static const Type UserEventType;
    QMap<QString, QString> mParam;
};

Q_DECLARE_METATYPE(Event)

#endif // EVENT_H
