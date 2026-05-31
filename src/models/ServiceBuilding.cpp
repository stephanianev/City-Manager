#include "ServiceBuilding.h"
#include "Citizen.h"

using namespace std;

ServiceBuilding::ServiceBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

// No citizen-type restrictions currently.
// Hook exists for future rules (e.g. profession-restricted service buildings).
bool ServiceBuilding::canAcceptCitizen(
    const Citizen&
) const {
    return true;
}

string ServiceBuilding::getType() const {
    return "Service";
}

BuildingType ServiceBuilding::getBuildingType() const {
    return BuildingType::Service;
}