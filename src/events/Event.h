#pragma once

#include <string>
#include <ctime>

// Abstract base for all city events.
//
// Every action that changes city state (creating/removing citizens or buildings,
// assigning homes/workplaces, moving citizens) emits a concrete subclass of
// Event through CityManager -> EventManager. Events are immutable once created.
//
// Subclasses must implement:
//   getType()        — a short string identifier (e.g. "CitizenCreated")
//   getDescription() — a human-readable log line for display and debugging
class Event {
protected:
    time_t timestamp; // wall-clock time at construction; set once in Event()

public:
    Event();

    virtual ~Event() = default;

    time_t getTimestamp() const;

    virtual std::string getType() const = 0;

    virtual std::string getDescription() const = 0;
};
