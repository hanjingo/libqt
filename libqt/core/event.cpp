#include "event.h"

const QEvent::Type Event::UserEventType = (QEvent::Type)QEvent::registerEventType(QEvent::User + 101);

Event::Event()
    : QEvent(UserEventType)
{
}

Event::Event(const Event &other)
    : QEvent(UserEventType)
{
}
