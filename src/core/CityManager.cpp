#include "CityManager.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

bool CityManager::isBlank(const string& str) const {
    return all_of(str.begin(), str.end(),
        [](unsigned char c) {
            return isspace(c);
        });
}

shared_ptr<Citizen> CityManager::createCitizen(
    const string& name,
    int age,
    const string& profession
) {
    if (name.empty() || isBlank(name)) {
        throw invalid_argument(
            "Citizen name cannot be empty"
        );
    }

    if (age < 0 || age > 120) {
        throw invalid_argument(
            "Age must be between 0 and 120"
        );
    }

    int id = nextCitizenId++;

    auto citizen = make_shared<Citizen>(
        id,
        name,
        age,
        profession
    );

    citizens[id] = citizen;

    return citizen;
}

shared_ptr<ResidentialBuilding>
CityManager::createResidentialBuilding(
    const string& name,
    size_t capacity
) {
    if (name.empty() || isBlank(name)) {
        throw invalid_argument(
            "Building name cannot be empty"
        );
    }

    if (capacity == 0) {
        throw invalid_argument(
            "Building capacity cannot be zero"
        );
    }

    int id = nextBuildingId++;

    auto building =
        make_shared<ResidentialBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    return building;
}

void CityManager::removeCitizen(int citizenId) {
    auto it = citizens.find(citizenId);

    if (it == citizens.end()) {
        throw invalid_argument(
            "Citizen does not exist"
        );
    }

    auto citizen = it->second;

    // Remove from home
    if (auto home = citizen->getHome()) {
        home->removeOccupant(citizenId);
    }

    // Remove from workplace
    if (auto workplace = citizen->getWorkplace()) {
        workplace->removeOccupant(citizenId);
    }

    // Remove from current location
    if (auto location = citizen->getLocation()) {
        location->removeOccupant(citizenId);
    }

    citizen->setHome(nullptr);
    citizen->setWorkplace(nullptr);
    citizen->setLocation(nullptr);

    citizens.erase(it);
}

int CityManager::getTotalCitizens() const {
    return static_cast<int>(citizens.size());
}

int CityManager::getTotalBuildings() const {
    return static_cast<int>(buildings.size());
}