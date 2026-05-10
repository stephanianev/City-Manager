#pragma once

#include "Building.h"

class IndustrialBuilding : public Building {
public:
    IndustrialBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    bool canAcceptCitizen(
        const Citizen& citizen
    ) const override;

    string getType() const override;
};