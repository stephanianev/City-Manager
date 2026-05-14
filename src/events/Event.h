#pragma once

#include <string>
#include <ctime>

using namespace std;

class Event {
protected:
    time_t timestamp;

public:
    Event();

    virtual ~Event() = default;

    time_t getTimestamp() const;

    virtual string getType() const = 0;

    virtual string getDescription() const = 0;
};
