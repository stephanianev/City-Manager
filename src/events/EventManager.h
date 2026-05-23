#pragma once

#include <vector>
#include <memory>

#include "Event.h"

class EventManager {
private:
    std::vector<std::shared_ptr<Event>> events;
    size_t maxEvents = 10000; // default cap to prevent unbounded memory growth

public:
    void addEvent(std::shared_ptr<Event> event);

    const std::vector<std::shared_ptr<Event>>&
    getEvents() const;

    size_t getEventCount() const;

    void clear();
};
