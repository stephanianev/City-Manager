#include "IndustrialBuilding.h"
#include "Citizen.h"

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