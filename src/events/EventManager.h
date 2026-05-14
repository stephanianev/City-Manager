#pragma once

#include <vector>
#include <memory>

#include "Event.h"

using namespace std;

class EventManager {
private:
    vector<shared_ptr<Event>> events;

public:
    void addEvent(shared_ptr<Event> event);

    const vector<shared_ptr<Event>>&
    getEvents() const;

    size_t getEventCount() const;

    void clear();
};
