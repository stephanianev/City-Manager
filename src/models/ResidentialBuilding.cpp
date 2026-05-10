#include "ResidentialBuilding.h"

ResidentialBuilding::ResidentialBuilding(
    int id,
    const string& name,
    size_t capacity
) {
    this->id = id;
    this->name = name;
    this->capacity = capacity;
}

string ResidentialBuilding::getType() const {
    return "Residential";
}