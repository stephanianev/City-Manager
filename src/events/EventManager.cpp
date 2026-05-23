#include "EventManager.h"

#include <stdexcept>

using namespace std;

void EventManager::addEvent(
    shared_ptr<Event> event
) {
    if (!event) {
        throw invalid_argument("Event cannot be null");
    }

    // If at capacity, drop the oldest event (front) to make room
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