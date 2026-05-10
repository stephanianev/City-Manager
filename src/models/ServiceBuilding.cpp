#include "ServiceBuilding.h"
#include "Citizen.h"

ServiceBuilding::ServiceBuilding(
    int id,
    const string& name,
    size_t capacity
) {
    this->id = id;
    this->name = name;
    this->capacity = capacity;
}

bool ServiceBuilding::canAcceptCitizen(
    const Citizen&
) const {
    return true;
}

string ServiceBuilding::getType() const {
    return "Service";
}