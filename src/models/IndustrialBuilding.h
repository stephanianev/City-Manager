#pragma once

#include "Building.h"

// An industrial building (factories, warehouses, etc.). Can be assigned as a workplace.
// Cannot be assigned as a home (CityManager enforces this).
class IndustrialBuilding : public Building {
public:
    IndustrialBuilding(
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