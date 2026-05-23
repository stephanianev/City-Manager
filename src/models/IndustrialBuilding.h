#pragma once

#include "Building.h"

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