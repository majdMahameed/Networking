

#include "Event.h"

Event::Event(double time) : event_time(time) {};

double Event::getEventTime() const {
    return event_time;
}

bool Event::operator<(const Event& event) {
    return event_time < event.event_time;
}

bool EventComparator::operator()(const std::shared_ptr<Event>& left, const std::shared_ptr<Event>& right) const
{
    return left->getEventTime() > right->getEventTime();
}
