#pragma once

#include "Building.h"

class ServiceBuilding : public Building {
public:
    ServiceBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    bool canAcceptCitizen(
        const Citizen& citizen
    ) const override;

    string getType() const override;
};