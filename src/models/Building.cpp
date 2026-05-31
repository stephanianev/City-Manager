#include "Building.h"
#include "Citizen.h"

#include <algorithm>
#include <stdexcept>

using namespace std;

Building::Building(
    int id,
    const string& name,
    size_t capacity
)
    : id(id), name(name), capacity(capacity) {

    //----------------------------------
    // Validation
    // Minimal invariant checks for direct construction.
    // Full serialization-safe validation happens in Validation:: and
    // CityManager::restoreBuildingImpl before this constructor is used in load paths.
    //----------------------------------

    if (name.empty()) {
        throw invalid_argument(
            "Building name cannot be empty"
        );
    }

    if (capacity == 0) {
        throw invalid_argument(
            "Building capacity must be positive"
        );
    }
}

// --- Simple value getters ---
int Building::getId() const { return id; }
string Building::getName() const { return name; }
size_t Building::getCapacity() const { return capacity; }

bool Building::hasCapacity() const {
    return getOccupantCount() < capacity;
}

size_t Building::getOccupantCount() const {
    // We cannot use occupants.size() directly because the vector may contain
    // expired weak_ptrs from citizens who were removed from the city without
    // going through this building (e.g. removeBuilding skips the occupant list).
    // Counting only non-expired entries gives the true physical headcount.
    size_t count = 0;
    for (const auto& w : occupants) {
        if (!w.expired()) count++;
    }
    return count;
}

vector<shared_ptr<Citizen>> Building::getOccupants() const {
    vector<shared_ptr<Citizen>> result;

    for (auto& w : occupants) {
        // w.lock() attempts to promote the weak_ptr to a shared_ptr.
        // It returns nullptr if the citizen was already destroyed, so we
        // only add valid (live) citizens to the result.
        if (auto c = w.lock()) {
            result.push_back(c);
        }
    }

    return result;
}

void Building::setName(const string& name) {
    if (name.empty()) {
        throw invalid_argument("Building name cannot be empty");
    }
    this->name = name;
}

// Called by CityManager::moveCitizen only.
// Guards against accidentally adding the same citizen twice (idempotent).
void Building::addOccupant(shared_ptr<Citizen> c) {

    for (const auto& w : occupants) {

        auto existing = w.lock();

        // If we already have a live reference to this citizen, do nothing.
        if (existing &&
            existing->getId() == c->getId()) {
            return;
        }
    }

    occupants.push_back(c);
}

// Called by CityManager when a citizen moves away or is removed from the city.
// Also removes any stale (expired) entries for the same citizen ID while iterating,
// keeping the occupants vector clean.
void Building::removeOccupant(int citizenId) {
    occupants.erase(
        remove_if(occupants.begin(), occupants.end(),
            [citizenId](const weak_ptr<Citizen>& w) {
                auto c = w.lock();
                // Remove if expired (citizen destroyed) OR if it matches the target ID.
                return !c || c->getId() == citizenId;
            }),
        occupants.end()
    );
}
