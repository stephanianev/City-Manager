#include "Event.h"

using namespace std;

Event::Event()
    : timestamp(time(nullptr)) {}

time_t Event::getTimestamp() const {
    return timestamp;
}