#pragma once

#include <string>
#include <vector>
#include <memory>
#include "BuildingType.h"

class Citizen;
class CityManager;

// Abstract base class for all building types.
//
// Ownership model:
//   - CityManager holds a shared_ptr<Building> for each building in the city.
//   - Citizens hold weak_ptr<Building> back-references (home, workplace, location),
//     so they never keep a building alive on their own.
//   - The occupants list here is also weak_ptr<Citizen> for the same reason:
//     CityManager owns the citizens; buildings only observe them.
//
// Physical occupancy vs. assignment:
//   - "Occupants" (this class) = citizens currently physically present,
//     as set by CityManager::moveCitizen. This is what getOccupantCount()
//     and hasCapacity() operate on.
//   - Home/workplace assignments on Citizen are separate logical relationships
//     and do NOT add to the occupant list.
//
// addOccupant / removeOccupant are private and friend-accessible only to
// CityManager, so that all physical movement stays centralised there.
class Building {
    friend class CityManager;

    // Only CityManager may move citizens in/out — it ensures capacity checks,
    // old-location cleanup, and event emission all happen atomically.
    void addOccupant(std::shared_ptr<Citizen> c);
    void removeOccupant(int citizenId);

protected:
    int id;
    std::string name;
    size_t capacity; // maximum number of citizens physically present at once

    // Weak references so that removing a citizen from CityManager automatically
    // causes these entries to expire without requiring explicit cleanup here.
    std::vector<std::weak_ptr<Citizen>> occupants;

public:
    Building(int id,
             const std::string& name,
             size_t capacity);

    virtual ~Building() = default;

    // --- Getters ---
    int getId() const;
    std::string getName() const;
    size_t getCapacity() const;

    // Returns true if the number of non-expired occupants is below capacity.
    bool hasCapacity() const;

    // Counts only non-expired weak_ptrs (expired = citizen was removed from city).
    size_t getOccupantCount() const;

    // Locks all non-expired weak_ptrs and returns a live shared_ptr list.
    std::vector<std::shared_ptr<Citizen>> getOccupants() const;

    void setName(const std::string& name);

    // Subclass-specific admission logic (e.g. age/profession restrictions in
    // the future). Currently all subtypes accept every citizen, but the hook
    // exists so that behaviour can be extended without touching CityManager.
    virtual bool canAcceptCitizen(const Citizen& citizen) const = 0;

    virtual std::string getType() const = 0;           // string label used in serialization and display
    virtual BuildingType getBuildingType() const = 0;  // enum used for internal type-checking logic
};