
#ifndef Event_hpp
#define Event_hpp

#include <iostream>
#include <vector>
#include <queue>
#include <memory>

class Event {
protected:
    const double event_time;
public:
    Event();
    Event(double time);
    virtual ~Event() = default;
    double getEventTime() const;
    bool operator<(const Event& event);
    virtual void process() const = 0;
};

struct EventComparator {
    bool operator() (const std::shared_ptr<Event>& left, const std::shared_ptr<Event>& right) const;
};
#endif /* Event_hpp */