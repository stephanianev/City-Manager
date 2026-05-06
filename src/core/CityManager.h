#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "../models/Citizen.h"
#include "../models/Building.h"

using namespace std;

class CityManager {
private:
    unordered_map<int, shared_ptr<Citizen>> citizens;
    unordered_map<int, shared_ptr<Building>> buildings;

    int nextCitizenId = 1;

    bool isBlank(const string& str) const;

public:
    shared_ptr<Citizen> createCitizen(
        const string& name,
        int age,
        const string& profession
    );

    int getTotalCitizens() const;
};