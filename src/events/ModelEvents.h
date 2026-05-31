#pragma once

#include "Event.h"

#include <string>

// This file contains all concrete Event subclasses that correspond to
// changes in the city's data model. Each event captures only the IDs/names
// needed to produce a readable description — it does not hold pointers to
// live objects (those might be destroyed by the time the log is read).

///////////////////////////////////////////////////////////
// CitizenCreatedEvent
// Emitted by CityManager::createCitizen.
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
// Emitted by CityManager::removeCitizen.
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
// Emitted by CityManager::createXxxBuilding.
///////////////////////////////////////////////////////////

class BuildingCreatedEvent : public Event {
private:
    int buildingId;
    std::string buildingName;
    std::string buildingType; // e.g. "Residential"

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
// Emitted by CityManager::removeBuilding.
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
// Emitted by CityManager::assignHome.
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
// Emitted by CityManager::assignWorkplace.
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
// Emitted by CityManager::moveCitizen.
// Tracks physical movement of a citizen to a new building
// (used by the future simulation layer).
///////////////////////////////////////////////////////////

class CitizenMovedEvent : public Event {
private:
    int citizenId;
    int buildingId; // the destination building

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
// Emitted by CityManager::updateCitizenAge / updateCitizenProfession.
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
// Emitted by CityManager::renameBuilding.
///////////////////////////////////////////////////////////

class BuildingUpdatedEvent : public Event {
private:
    int buildingId;
    std::string buildingName; // the new name after the rename

public:
    BuildingUpdatedEvent(int id, const std::string& name);

    std::string getType() const override;
    std::string getDescription() const override;
};