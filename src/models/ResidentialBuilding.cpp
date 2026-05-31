#include "ResidentialBuilding.h"
#include "Citizen.h"

using namespace std;

ResidentialBuilding::ResidentialBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

// No citizen-type restrictions for residential buildings — any citizen can live here.
// This override exists as a hook for future rules (e.g. age-restricted housing).
bool ResidentialBuilding::canAcceptCitizen(const Citizen&) const {
    return true;
}

string ResidentialBuilding::getType() const {
    return "Residential";
}

BuildingType ResidentialBuilding::getBuildingType() const {
    return BuildingType::Residential;
}