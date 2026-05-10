#pragma once

#include "Building.h"

class ResidentialBuilding : public Building {
public:
    ResidentialBuilding(int id,
                        const string& name,
                        size_t capacity);

    string getType() const override;
};