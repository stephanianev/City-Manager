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

int Building::getId() const { return id; }
string Building::getName() const { return name; }
size_t Building::getCapacity() const { return capacity; }

bool Building::hasCapacity() const {
    return getOccupantCount() < capacity;
}

size_t Building::getOccupantCount() const { // occupants.size() still counts expired weak_ptrs, so we need to count only the valid ones
    size_t count = 0;
    for (const auto& w : occupants) {
        if (!w.expired()) count++;
    }
    return count;
}

vector<shared_ptr<Citizen>> Building::getOccupants() const {
    vector<shared_ptr<Citizen>> result;

    for (auto& w : occupants) { // auto& to avoid copying weak_ptr (auto deduces a type from the initializer)
        if (auto c = w.lock()) { // lock the weak_ptr to get a shared_ptr (w.lock() attempts to create a shared_ptr<Citizen>)
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

void Building::addOccupant(shared_ptr<Citizen> c) {

    for (const auto& w : occupants) {

        auto existing = w.lock();

        if (existing &&
            existing->getId() == c->getId()) {
            return;
        }
    }

    occupants.push_back(c);
}

void Building::removeOccupant(int citizenId) {
    occupants.erase(
        remove_if(occupants.begin(), occupants.end(),
            [citizenId](const weak_ptr<Citizen>& w) {
                auto c = w.lock();
                return !c || c->getId() == citizenId;
            }),
        occupants.end()
    );
}
