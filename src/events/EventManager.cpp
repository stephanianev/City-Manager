#include "EventManager.h"

#include <stdexcept>

using namespace std;

void EventManager::addEvent(
    shared_ptr<Event> event
) {
    if (!event) {
        throw invalid_argument("Event cannot be null");
    }

    // Sliding-window eviction: drop the oldest event (front) when full.
    // Erasing from the front of a vector is O(n), but with a cap of 10 000
    // entries this is fast enough in practice. A deque would be O(1) here
    // if the log ever needs to be much larger.
    if (events.size() >= maxEvents) {
        events.erase(events.begin());
    }

    events.push_back(event);
}

const vector<shared_ptr<Event>>&
EventManager::getEvents() const {
    return events;
}

size_t EventManager::getEventCount() const {
    return events.size();
}

void EventManager::clear() {
    events.clear();
}