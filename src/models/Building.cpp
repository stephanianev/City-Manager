#include "Building.h"
#include "Citizen.h"

#include <algorithm>

int Building::getId() const { return id; }
string Building::getName() const { return name; }
size_t Building::getCapacity() const { return capacity; }

bool Building::hasCapacity() const {
    return occupants.size() < capacity;
}

size_t Building::getOccupantCount() const {
    return occupants.size();
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
