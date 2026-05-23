#pragma once

#include <string>
#include <ctime>

class Event {
protected:
    time_t timestamp;

public:
    Event();

    virtual ~Event() = default;

    time_t getTimestamp() const;

    virtual std::string getType() const = 0;

    virtual std::string getDescription() const = 0;
};
