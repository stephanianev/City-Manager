#include "IndustrialBuilding.h"
#include "Citizen.h"

using namespace std;

IndustrialBuilding::IndustrialBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

// No citizen-type restrictions currently.
// Hook exists for future rules (e.g. minimum age for industrial work).
bool IndustrialBuilding::canAcceptCitizen(
    const Citizen&
) const {
    return true;
}

string IndustrialBuilding::getType() const {
    return "Industrial";
}

BuildingType IndustrialBuilding::getBuildingType() const {
    return BuildingType::Industrial;
}