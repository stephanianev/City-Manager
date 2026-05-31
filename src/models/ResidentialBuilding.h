#pragma once

#include "Building.h"

// A building intended for citizens to live in.
// This is the only type that can be assigned as a citizen's home (CityManager
// enforces this rule). It cannot be used as a workplace.
class ResidentialBuilding : public Building {
public:
    ResidentialBuilding(
        int id,
        const std::string& name,
        size_t capacity
    );

    bool canAcceptCitizen(
        const Citizen& citizen
    ) const override;

    std::string getType() const override;
    BuildingType getBuildingType() const override;
};