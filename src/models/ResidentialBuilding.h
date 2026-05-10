#pragma once

#include "Building.h"

class ResidentialBuilding : public Building {
public:
    ResidentialBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    bool canAcceptCitizen(
        const Citizen& citizen
    ) const override;

    string getType() const override;
};