#include "IndustrialBuilding.h"
#include "Citizen.h"

using namespace std;

IndustrialBuilding::IndustrialBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

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