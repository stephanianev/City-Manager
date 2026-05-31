#pragma once

#include "Building.h"

// A commercial building (shops, offices, etc.). Can be assigned as a workplace.
// Cannot be assigned as a home (CityManager enforces this).
class CommercialBuilding : public Building {
public:
    CommercialBuilding(
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