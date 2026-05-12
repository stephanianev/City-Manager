#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "../models/Citizen.h"
#include "../models/Building.h"
#include "../models/ResidentialBuilding.h"
#include "../models/CommercialBuilding.h"
#include "../models/IndustrialBuilding.h"
#include "../models/ServiceBuilding.h"

using namespace std;

class CityManager {
private:
    unordered_map<int, shared_ptr<Citizen>> citizens;
    unordered_map<int, shared_ptr<Building>> buildings;

    int nextCitizenId = 1;
    int nextBuildingId = 1;

    bool isBlank(const string& str) const;

    shared_ptr<Citizen>
    getCitizenOrThrow(int id) const;

    shared_ptr<Building>
    getBuildingOrThrow(int id) const;

public:
    //--------------------------------------------------
    // Runtime Creation Operations
    //--------------------------------------------------
    shared_ptr<Citizen> createCitizen(
        const string& name,
        int age,
        const string& profession
    );

    shared_ptr<ResidentialBuilding> createResidentialBuilding(
        const string& name,
        size_t capacity
    );

    shared_ptr<CommercialBuilding> createCommercialBuilding(
        const string& name,
        size_t capacity
    );

    shared_ptr<IndustrialBuilding> createIndustrialBuilding(
        const string& name,
        size_t capacity
    );

    shared_ptr<ServiceBuilding> createServiceBuilding(
        const string& name,
        size_t capacity
    );

    //--------------------------------------------------
    // Restoration Operations
    //--------------------------------------------------
    shared_ptr<Citizen> restoreCitizen(
    int id,
    const string& name,
    int age,
    const string& profession
    );

    shared_ptr<Building> restoreResidentialBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    shared_ptr<Building> restoreCommercialBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    shared_ptr<Building> restoreIndustrialBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    shared_ptr<Building> restoreServiceBuilding(
        int id,
        const string& name,
        size_t capacity
    );

    void assignWorkplace(int citizenId, int buildingId);
    void moveCitizen(int citizenId, int buildingId);

    void assignHome(int citizenId, int buildingId);

    //--------------------------------------------------
    // Removal Operations
    //--------------------------------------------------
    // Note: we do not manually null every weak_ptr, because they will automatically become expired when the shared_ptr is destroyed.

    void removeCitizen(int citizenId);

    void removeBuilding(int buildingId);

    //--------------------------------------------------
    // Query Operations
    //--------------------------------------------------

    vector<shared_ptr<Citizen>> findCitizensByProfession(const string& profession) const;

    vector<shared_ptr<Citizen>> listCitizensInBuilding(int buildingId) const;

    vector<shared_ptr<Building>> findBuildingsWithCapacity() const;

    //--------------------------------------------------
    // Statistics
    //--------------------------------------------------

    size_t getTotalCitizens() const;

    size_t getTotalBuildings() const;

    size_t getHomelessCount() const;

    size_t getUnemployedCount() const;

    double getOccupancyRate(int buildingId) const;
};