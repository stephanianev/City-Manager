#include "Event.h"

Event::Event()
    : timestamp(time(nullptr)) {}

time_t Event::getTimestamp() const {
    return timestamp;
}