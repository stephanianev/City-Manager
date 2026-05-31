#include "Event.h"

using namespace std;

// Capture the current wall-clock time at construction so the event log
// carries an accurate timestamp regardless of when it is later read.
Event::Event()
    : timestamp(time(nullptr)) {}

time_t Event::getTimestamp() const {
    return timestamp;
}