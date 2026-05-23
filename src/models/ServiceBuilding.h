#pragma once

#include "Building.h"

class ServiceBuilding : public Building {
public:
    ServiceBuilding(
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