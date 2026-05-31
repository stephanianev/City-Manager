#pragma once

#include "Building.h"

// A service building (hospitals, schools, etc.). Can be assigned as a workplace.
// Cannot be assigned as a home (CityManager enforces this).
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