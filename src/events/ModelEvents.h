#pragma once

#include "Event.h"

#include <string>

///////////////////////////////////////////////////////////
// CitizenCreatedEvent
///////////////////////////////////////////////////////////

class CitizenCreatedEvent : public Event {
private:
    int citizenId;
    std::string citizenName;

public:
    CitizenCreatedEvent(
        int id,
        const std::string& name
    );

    std::string getType() const override;

    std::string getDescription() const override;
};

///////////////////////////////////////////////////////////
// CitizenRemovedEvent
///////////////////////////////////////////////////////////

class CitizenRemovedEvent : public Event {
private:
    int citizenId;

public:
    CitizenRemovedEvent(int id);

    std::string getType() const override;

    std::string getDescription() const override;
};

///////////////////////////////////////////////////////////
// BuildingCreatedEvent
///////////////////////////////////////////////////////////

class BuildingCreatedEvent : public Event {
private:
    int buildingId;
    std::string buildingName;
    std::string buildingType;

public:
    BuildingCreatedEvent(
        int id,
        const std::string& name,
        const std::string& type
    );

    std::string getType() const override;

    std::string getDescription() const override;
};

///////////////////////////////////////////////////////////
// BuildingRemovedEvent
///////////////////////////////////////////////////////////

class BuildingRemovedEvent : public Event {
private:
    int buildingId;

public:
    BuildingRemovedEvent(int id);

    std::string getType() const override;

    std::string getDescription() const override;
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

    std::string getType() const override;

    std::string getDescription() const override;
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

    std::string getType() const override;

    std::string getDescription() const override;
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

    std::string getType() const override;

    std::string getDescription() const override;
};

///////////////////////////////////////////////////////////
// CitizenUpdatedEvent
///////////////////////////////////////////////////////////

class CitizenUpdatedEvent : public Event {
private:
    int citizenId;
    std::string citizenName;

public:
    CitizenUpdatedEvent(int id, const std::string& name);

    std::string getType() const override;
    std::string getDescription() const override;
};

///////////////////////////////////////////////////////////
// BuildingUpdatedEvent
///////////////////////////////////////////////////////////

class BuildingUpdatedEvent : public Event {
private:
    int buildingId;
    std::string buildingName;

public:
    BuildingUpdatedEvent(int id, const std::string& name);

    std::string getType() const override;
    std::string getDescription() const override;
};