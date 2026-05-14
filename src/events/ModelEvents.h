#pragma once

#include "Event.h"

#include <string>

using namespace std;

///////////////////////////////////////////////////////////
// CitizenCreatedEvent
///////////////////////////////////////////////////////////

class CitizenCreatedEvent : public Event {
private:
    int citizenId;
    string citizenName;

public:
    CitizenCreatedEvent(
        int id,
        const string& name
    );

    string getType() const override;

    string getDescription() const override;
};

///////////////////////////////////////////////////////////
// CitizenRemovedEvent
///////////////////////////////////////////////////////////

class CitizenRemovedEvent : public Event {
private:
    int citizenId;

public:
    CitizenRemovedEvent(int id);

    string getType() const override;

    string getDescription() const override;
};

///////////////////////////////////////////////////////////
// BuildingCreatedEvent
///////////////////////////////////////////////////////////

class BuildingCreatedEvent : public Event {
private:
    int buildingId;
    string buildingName;
    string buildingType;

public:
    BuildingCreatedEvent(
        int id,
        const string& name,
        const string& type
    );

    string getType() const override;

    string getDescription() const override;
};

///////////////////////////////////////////////////////////
// BuildingRemovedEvent
///////////////////////////////////////////////////////////

class BuildingRemovedEvent : public Event {
private:
    int buildingId;

public:
    BuildingRemovedEvent(int id);

    string getType() const override;

    string getDescription() const override;
};

///////////////////////////////////////////////////////////
// HomeAssignedEvent
///////////////////////////////////////////////////////////

class HomeAssignedEvent : public Event {
private:
    int citizenId;
    int buildingId;

public:
    HomeAssignedEvent(
        int citizenId,
        int buildingId
    );

    string getType() const override;

    string getDescription() const override;
};

///////////////////////////////////////////////////////////
// WorkplaceAssignedEvent
///////////////////////////////////////////////////////////

class WorkplaceAssignedEvent : public Event {
private:
    int citizenId;
    int buildingId;

public:
    WorkplaceAssignedEvent(
        int citizenId,
        int buildingId
    );

    string getType() const override;

    string getDescription() const override;
};

///////////////////////////////////////////////////////////
// CitizenMovedEvent
///////////////////////////////////////////////////////////

class CitizenMovedEvent : public Event {
private:
    int citizenId;
    int buildingId;

public:
    CitizenMovedEvent(
        int citizenId,
        int buildingId
    );

    string getType() const override;

    string getDescription() const override;
};