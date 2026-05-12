#include "CommercialBuilding.h"
#include "Citizen.h"

CommercialBuilding::CommercialBuilding(
    int id,
    const string& name,
    size_t capacity
) : Building(id, name, capacity) {}

bool CommercialBuilding::canAcceptCitizen(
    const Citizen&
) const {
    return true;
}

string CommercialBuilding::getType() const {
    return "Commercial";
}