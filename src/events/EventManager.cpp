#include "EventManager.h"

#include <stdexcept>

void EventManager::addEvent(
    shared_ptr<Event> event
) {

    if (!event) {
        throw invalid_argument(
            "Event cannot be null"
        );
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