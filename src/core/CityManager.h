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

// Avoid "using namespace std;" in headers — it would pollute every translation
// unit that includes this file with the entire std namespace.

// The central controller for all city state.
//
// Owns all citizens and buildings via shared_ptr maps keyed by their integer IDs.
// All creation, removal, assignment, movement, querying, and reporting
// operations go through this class.
//
// ID assignment:
//   - createXxx  functions auto-increment nextCitizenId / nextBuildingId.
//   - restoreXxx functions accept an explicit ID (used during file loading)
//     and advance the counter if the given ID is higher than the current one,
//     so that post-load creation never reuses an ID from the save file.
//
// Physical vs. logical relationships:
//   - Logical:  assignHome / assignWorkplace — sets a weak reference on the
//     citizen; does NOT add the citizen to the building's occupant list.
//   - Physical: moveCitizen — adds the citizen to the building's occupant
//     list and updates citizen.currentLocation. This is the basis of the
//     future simulation layer.
class CityManager {
private:
    std::unordered_map<int, std::shared_ptr<Citizen>>  citizens;
    std::unordered_map<int, std::shared_ptr<Building>> buildings;

    int nextCitizenId  = 1; // always 1 higher than the largest citizen ID in use
    int nextBuildingId = 1; // always 1 higher than the largest building ID in use

    // Returns true if str is empty or consists entirely of whitespace.
    bool isBlank(const std::string& str) const;

    // Looks up a citizen by ID; throws invalid_argument if not found.
    // Used internally to avoid repeating the same find+check pattern.
    std::shared_ptr<Citizen>
    getCitizenOrThrow(int id) const;

    // Looks up a building by ID; throws invalid_argument if not found.
    std::shared_ptr<Building>
    getBuildingOrThrow(int id) const;

    EventManager eventManager;

    // Template helper used by all four createXxxBuilding methods.
    // Validates input, assigns the next ID, constructs a T, stores it,
    // and emits a BuildingCreatedEvent — all in one place.
    template<typename T>
    std::shared_ptr<T> createBuildingImpl(
        const std::string& name,
        size_t capacity
    );

    // Template helper used by all four restoreXxxBuilding methods.
    // Constructs a T with the given explicit ID (from a save file),
    // stores it, and advances nextBuildingId if necessary.
    // Does NOT emit an event — restore operations are silent replays.
    template<typename T>
    std::shared_ptr<Building> restoreBuildingImpl(
        int id,
        const std::string& name,
        size_t capacity
    );

public:
    //--------------------------------------------------
    // Runtime Creation Operations
    // These auto-assign IDs and emit creation events.
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

    // Update individual fields on existing citizens / buildings.
    // Each emits an Updated event.
    void updateCitizenAge(int citizenId, int age);
    void updateCitizenProfession(int citizenId, const std::string& profession);
    void renameBuilding(int buildingId, const std::string& name);

    //--------------------------------------------------
    // Restoration Operations
    // Used exclusively by CitySerializer::loadFromFile to reconstruct
    // the city from a save file with the original IDs. These do NOT
    // emit events, but they DO validate restored values so malformed
    // save data fails fast.
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

    // Read-only access to the internal maps (used by CitySerializer).
    const std::unordered_map<int, std::shared_ptr<Citizen>>&  getCitizens()  const;
    const std::unordered_map<int, std::shared_ptr<Building>>& getBuildings() const;

    // Assigns a non-residential building as the citizen's workplace (logical only).
    void assignWorkplace(int citizenId, int buildingId);

    // Physically moves a citizen to a building: removes them from their current
    // location's occupant list, adds them to the new one, and updates currentLocation.
    void moveCitizen(int citizenId, int buildingId);

    // Assigns a residential building as the citizen's home (logical only).
    void assignHome(int citizenId, int buildingId);

    //--------------------------------------------------
    // Removal Operations
    //--------------------------------------------------

    // Removes a citizen from the city. Also removes them from their current
    // physical location's occupant list.
    // Note: home/workplace weak_ptrs on the citizen expire automatically when
    // the shared_ptr is destroyed — no manual nulling required.
    void removeCitizen(int citizenId);

    // Removes a building from the city. Iterates all citizens and clears any
    // home/workplace/location references pointing to the removed building.
    void removeBuilding(int buildingId);

    //--------------------------------------------------
    // Query Operations
    //--------------------------------------------------

    std::vector<std::shared_ptr<Citizen>> findCitizensByProfession(const std::string& profession) const;

    // Returns the current physical occupants of a building (i.e. citizens
    // placed there by moveCitizen, not just assigned residents/workers).
    std::vector<std::shared_ptr<Citizen>> listCitizensInBuilding(int buildingId) const;

    std::vector<std::shared_ptr<Building>> findBuildingsWithCapacity() const;

    std::vector<std::shared_ptr<Citizen>> findCitizensByName(const std::string& name) const;

    std::vector<std::shared_ptr<Building>> findBuildingsByName(const std::string& name) const;

    // Generic predicate-based citizen filter. Accepts any callable that takes a
    // shared_ptr<Citizen> and returns bool. Useful for ad-hoc queries in main/tests.
    std::vector<std::shared_ptr<Citizen>>
    queryCitizens(
        std::function<bool(
            const std::shared_ptr<Citizen>&
        )> predicate
    ) const;

    // Generic predicate-based building filter.
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

    // Citizens with no home assigned.
    size_t getHomelessCount() const;

    // Citizens with no workplace assigned.
    size_t getUnemployedCount() const;

    // Returns occupant count / capacity for a single building in [0.0, 1.0].
    double getOccupancyRate(int buildingId) const;

    double getAverageCitizenAge() const;

    // Returns employed / total citizens in [0.0, 1.0].
    double getEmploymentRate() const;

    // Returns a map of building-type string -> average occupancy ratio [0.0, 1.0]
    // aggregated across all buildings of that type.
    std::unordered_map<std::string, double>
    getAverageOccupancyByType() const;

    //--------------------------------------------------
    // Events
    //--------------------------------------------------

    const EventManager& getEventManager() const;

    //--------------------------------------------------
    // Sorting Operations
    // These take a copy of a vector (intentionally by value) and return
    // it sorted according to the caller-supplied comparator, leaving the
    // original collection unchanged.
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

    // Returns a map of profession string -> citizen count.
    // Citizens with an empty profession string appear under the "" key
    // (displayed as "Jobless" in the UI).
    std::unordered_map<std::string, int>
    getProfessionDistribution() const;

    // Returns a map of building-type string -> building count.
    std::unordered_map<std::string, int>
    getBuildingTypeDistribution() const;

    // Returns a list of (building name, occupancy ratio) pairs for every building.
    std::vector<std::pair<std::string, double>>
    getBuildingOccupancyReport() const;
};