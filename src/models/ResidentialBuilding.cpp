#include "ResidentialBuilding.h"
#include "Citizen.h"

using namespace std;

ResidentialBuilding::ResidentialBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

bool ResidentialBuilding::canAcceptCitizen(const Citizen&) const {
    return true;
}

string ResidentialBuilding::getType() const {
    return "Residential";
}

BuildingType ResidentialBuilding::getBuildingType() const {
    return BuildingType::Residential;
}