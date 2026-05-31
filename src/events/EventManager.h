#pragma once

#include <vector>
#include <memory>

#include "Event.h"

// Maintains an ordered, bounded log of city events.
//
// Events are appended in chronological order. When the log reaches maxEvents,
// the oldest entry is dropped to make room (sliding window / FIFO eviction).
// This prevents unbounded memory growth in long-running sessions.
//
// CityManager owns one EventManager and is the only caller of addEvent.
// Consumers (UI, tests, serialization) access the log read-only via getEvents().
class EventManager {
private:
    std::vector<std::shared_ptr<Event>> events;
    size_t maxEvents = 10000; // cap to prevent unbounded memory growth

public:
    // Appends an event to the log, evicting the oldest if at capacity.
    // Throws if event is null.
    void addEvent(std::shared_ptr<Event> event);

    // Returns the full log in insertion (chronological) order.
    const std::vector<std::shared_ptr<Event>>&
    getEvents() const;

    size_t getEventCount() const;

    // Clears all events (used in tests).
    void clear();
};
