#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "../models/Citizen.h"
#include "../models/Building.h"
#include "../models/ResidentialBuilding.h"

using namespace std;

class CityManager {
private:
    unordered_map<int, shared_ptr<Citizen>> citizens;
    unordered_map<int, shared_ptr<Building>> buildings;

    int nextCitizenId = 1;
    int nextBuildingId = 1;

    bool isBlank(const string& str) const;

public:
    shared_ptr<Citizen> createCitizen(
        const string& name,
        int age,
        const string& profession
    );

    shared_ptr<ResidentialBuilding> createResidentialBuilding(
        const string& name,
        size_t capacity
    );

    void removeCitizen(int citizenId);
    void assignHome(int citizenId, int buildingId);
    void removeBuilding(int buildingId); // does not remove manually assigned citizens, but clears their references

    int getTotalCitizens() const;
    int getTotalBuildings() const;
};