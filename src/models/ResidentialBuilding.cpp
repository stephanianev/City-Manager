#include "ResidentialBuilding.h"
#include "Citizen.h"

ResidentialBuilding::ResidentialBuilding(
    int id,
    const string& name,
    size_t capacity
) {
    this->id = id;
    this->name = name;
    this->capacity = capacity;
}

bool ResidentialBuilding::canAcceptCitizen(const Citizen&) const {
    return true;
}

string ResidentialBuilding::getType() const {
    return "Residential";
}