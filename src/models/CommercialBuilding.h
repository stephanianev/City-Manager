#pragma once

#include "Building.h"

class CommercialBuilding : public Building {
public:
    CommercialBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    bool canAcceptCitizen(
        const Citizen& citizen
    ) const override;

    string getType() const override;
};