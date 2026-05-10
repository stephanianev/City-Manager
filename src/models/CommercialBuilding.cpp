#include "CommercialBuilding.h"
#include "Citizen.h"

CommercialBuilding::CommercialBuilding(
    int id,
    const string& name,
    size_t capacity
) {
    this->id = id;
    this->name = name;
    this->capacity = capacity;
}

bool CommercialBuilding::canAcceptCitizen(
    const Citizen&
) const {
    return true;
}

string CommercialBuilding::getType() const {
    return "Commercial";
}