#include "ServiceBuilding.h"
#include "Citizen.h"

using namespace std;

ServiceBuilding::ServiceBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

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