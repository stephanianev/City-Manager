#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <functional>
#include <vector>

#include "../models/Citizen.h"
#include "../models/Building.h"
#include "../models/ResidentialBuilding.h"
#include "../models/CommercialBuilding.h"
#include "../models/IndustrialBuilding.h"
#include "../models/ServiceBuilding.h"
#include "../events/EventManager.h"

// using namespace std; Avoiding this in headers to prevent namespace pollution, but we have to use it in the implementation file.
class CityManager {
private:
    std::unordered_map<int, std::shared_ptr<Citizen>> citizens;
    std::unordered_map<int, std::shared_ptr<Building>> buildings;

    int nextCitizenId = 1;
    int nextBuildingId = 1;

    bool isBlank(const std::string& str) const;

    std::shared_ptr<Citizen>
    getCitizenOrThrow(int id) const;

    std::shared_ptr<Building>
    getBuildingOrThrow(int id) const;

    EventManager eventManager;

    template<typename T>
    std::shared_ptr<T> createBuildingImpl(
        const std::string& name,
        size_t capacity
    );

    template<typename T>
    std::shared_ptr<Building> restoreBuildingImpl(
        int id,
        const std::string& name,
        size_t capacity
    );

public:
    //--------------------------------------------------
    // Runtime Creation Operations
    //--------------------------------------------------
    std::shared_ptr<Citizen> createCitizen(
        const std::string& name,
        int age,
        const std::string& profession
    );

    std::shared_ptr<ResidentialBuilding> createResidentialBuilding(
        const std::string& name,
        size_t capacity
    );

    std::shared_ptr<CommercialBuilding> createCommercialBuilding(
        const std::string& name,
        size_t capacity
    );

    std::shared_ptr<IndustrialBuilding> createIndustrialBuilding(
        const std::string& name,
        size_t capacity
    );

    std::shared_ptr<ServiceBuilding> createServiceBuilding(
        const std::string& name,
        size_t capacity
    );

    void updateCitizenAge(int citizenId, int age);
    void updateCitizenProfession(int citizenId, const std::string& profession);
    void renameBuilding(int buildingId, const std::string& name);

    //--------------------------------------------------
    // Restoration Operations
    //--------------------------------------------------
    std::shared_ptr<Citizen> restoreCitizen(
    int id,
    const std::string& name,
    int age,
    const std::string& profession
    );

    std::shared_ptr<Building> restoreResidentialBuilding(
        int id,
        const std::string& name,
        size_t capacity
    );

    std::shared_ptr<Building> restoreCommercialBuilding(
        int id,
        const std::string& name,
        size_t capacity
    );

    std::shared_ptr<Building> restoreIndustrialBuilding(
        int id,
        const std::string& name,
        size_t capacity
    );

    std::shared_ptr<Building> restoreServiceBuilding(
        int id,
        const std::string& name,
        size_t capacity
    );

    const std::unordered_map<int, std::shared_ptr<Citizen>>&getCitizens() const;
    const std::unordered_map<int, std::shared_ptr<Building>>&getBuildings() const;

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

    std::vector<std::shared_ptr<Citizen>> findCitizensByProfession(const std::string& profession) const;

    std::vector<std::shared_ptr<Citizen>> listCitizensInBuilding(int buildingId) const;

    std::vector<std::shared_ptr<Building>> findBuildingsWithCapacity() const;

    std::vector<std::shared_ptr<Citizen>> findCitizensByName(const std::string& name) const;
    
    std::vector<std::shared_ptr<Building>> findBuildingsByName(const std::string& name) const;

    std::vector<std::shared_ptr<Citizen>>
    queryCitizens(
        std::function<bool(
            const std::shared_ptr<Citizen>&
        )> predicate
    ) const;

    std::vector<std::shared_ptr<Building>>
    queryBuildings(
        std::function<bool(
            const std::shared_ptr<Building>&
        )> predicate
    ) const;

    //--------------------------------------------------
    // Statistics
    //--------------------------------------------------

    size_t getTotalCitizens() const;

    size_t getTotalBuildings() const;

    size_t getHomelessCount() const;

    size_t getUnemployedCount() const;

    double getOccupancyRate(int buildingId) const;

    double getAverageCitizenAge() const;

    double getEmploymentRate() const;

    std::unordered_map<std::string, double>
    getAverageOccupancyByType() const;

    //--------------------------------------------------
    // Events
    //--------------------------------------------------

    const EventManager& getEventManager() const;

    //--------------------------------------------------
    // Sorting Operations
    //--------------------------------------------------

    std::vector<std::shared_ptr<Citizen>>
    sortCitizens(
        std::vector<std::shared_ptr<Citizen>> citizensToSort,
        std::function<bool(
            const std::shared_ptr<Citizen>&,
            const std::shared_ptr<Citizen>&
        )> comparator
    ) const;

    std::vector<std::shared_ptr<Building>>
    sortBuildings(
        std::vector<std::shared_ptr<Building>> buildingsToSort,
        std::function<bool(
            const std::shared_ptr<Building>&,
            const std::shared_ptr<Building>&
        )> comparator
    ) const;

    //--------------------------------------------------
    // Reporting Operations
    //--------------------------------------------------

    std::unordered_map<std::string, int>
    getProfessionDistribution() const;

    std::unordered_map<std::string, int>
    getBuildingTypeDistribution() const;

    std::vector<std::pair<std::string, double>>
    getBuildingOccupancyReport() const;
};