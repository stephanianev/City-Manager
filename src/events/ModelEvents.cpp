#include "ModelEvents.h"

using namespace std;

///////////////////////////////////////////////////////////
// CitizenCreatedEvent
///////////////////////////////////////////////////////////

CitizenCreatedEvent::CitizenCreatedEvent(
    int id,
    const string& name
)
    : citizenId(id),
      citizenName(name) {}

string CitizenCreatedEvent::getType() const {
    return "CitizenCreated";
}

string CitizenCreatedEvent::getDescription() const {

    return "Citizen created: ID=" +
           to_string(citizenId) +
           ", Name=" +
           citizenName;
}

///////////////////////////////////////////////////////////
// CitizenRemovedEvent
///////////////////////////////////////////////////////////

CitizenRemovedEvent::CitizenRemovedEvent(
    int id
)
    : citizenId(id) {}

string CitizenRemovedEvent::getType() const {
    return "CitizenRemoved";
}

string CitizenRemovedEvent::getDescription() const {

    return "Citizen removed: ID=" +
           to_string(citizenId);
}

///////////////////////////////////////////////////////////
// BuildingCreatedEvent
///////////////////////////////////////////////////////////

BuildingCreatedEvent::BuildingCreatedEvent(
    int id,
    const string& name,
    const string& type
)
    : buildingId(id),
      buildingName(name),
      buildingType(type) {}

string BuildingCreatedEvent::getType() const {
    return "BuildingCreated";
}

string BuildingCreatedEvent::getDescription() const {

    return "Building created: ID=" +
           to_string(buildingId) +
           ", Name=" +
           buildingName +
           ", Type=" +
           buildingType;
}

///////////////////////////////////////////////////////////
// BuildingRemovedEvent
///////////////////////////////////////////////////////////

BuildingRemovedEvent::BuildingRemovedEvent(
    int id
)
    : buildingId(id) {}

string BuildingRemovedEvent::getType() const {
    return "BuildingRemoved";
}

string BuildingRemovedEvent::getDescription() const {

    return "Building removed: ID=" +
           to_string(buildingId);
}

///////////////////////////////////////////////////////////
// HomeAssignedEvent
///////////////////////////////////////////////////////////

HomeAssignedEvent::HomeAssignedEvent(
    int citizen,
    int building
)
    : citizenId(citizen),
      buildingId(building) {}

string HomeAssignedEvent::getType() const {
    return "HomeAssigned";
}

string HomeAssignedEvent::getDescription() const {

    return "Home assigned: Citizen ID=" +
           to_string(citizenId) +
           ", Building ID=" +
           to_string(buildingId);
}

///////////////////////////////////////////////////////////
// WorkplaceAssignedEvent
///////////////////////////////////////////////////////////

WorkplaceAssignedEvent::WorkplaceAssignedEvent(
    int citizen,
    int building
)
    : citizenId(citizen),
      buildingId(building) {}

string WorkplaceAssignedEvent::getType() const {
    return "WorkplaceAssigned";
}

string WorkplaceAssignedEvent::getDescription() const {

    return "Workplace assigned: Citizen ID=" +
           to_string(citizenId) +
           ", Building ID=" +
           to_string(buildingId);
}

///////////////////////////////////////////////////////////
// CitizenMovedEvent
///////////////////////////////////////////////////////////

CitizenMovedEvent::CitizenMovedEvent(
    int citizen,
    int building
)
    : citizenId(citizen),
      buildingId(building) {}

string CitizenMovedEvent::getType() const {
    return "CitizenMoved";
}

string CitizenMovedEvent::getDescription() const {

    return "Citizen moved: Citizen ID=" +
           to_string(citizenId) +
           ", Building ID=" +
           to_string(buildingId);
}

///////////////////////////////////////////////////////////
// CitizenUpdatedEvent
///////////////////////////////////////////////////////////

CitizenUpdatedEvent::CitizenUpdatedEvent(
    int id,
    const string& name
)
    : citizenId(id),
      citizenName(name) {}

string CitizenUpdatedEvent::getType() const {
    return "CitizenUpdated";
}

string CitizenUpdatedEvent::getDescription() const {
    return "Citizen updated: ID=" +
           to_string(citizenId) +
           ", Name=" +
           citizenName;
}

///////////////////////////////////////////////////////////
// BuildingUpdatedEvent
///////////////////////////////////////////////////////////

BuildingUpdatedEvent::BuildingUpdatedEvent(
    int id,
    const string& name
)
    : buildingId(id),
      buildingName(name) {}

string BuildingUpdatedEvent::getType() const {
    return "BuildingUpdated";
}

string BuildingUpdatedEvent::getDescription() const {
    return "Building updated: ID=" +
           to_string(buildingId) +
           ", Name=" +
           buildingName;
}