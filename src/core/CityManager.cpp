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

shared_ptr<CommercialBuilding>
CityManager::createCommercialBuilding(
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
        make_shared<CommercialBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    return building;
}

shared_ptr<IndustrialBuilding>
CityManager::createIndustrialBuilding(
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
        make_shared<IndustrialBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    return building;
}

shared_ptr<ServiceBuilding>
CityManager::createServiceBuilding(
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
        make_shared<ServiceBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    return building;
}

void CityManager::assignWorkplace(
    int citizenId,
    int buildingId
) {

    //--------------------------------------
    // Validate citizen
    //--------------------------------------

    auto citizenIt = citizens.find(citizenId);

    if (citizenIt == citizens.end()) {
        throw invalid_argument(
            "Citizen does not exist"
        );
    }

    //--------------------------------------
    // Validate building
    //--------------------------------------

    auto buildingIt = buildings.find(buildingId);

    if (buildingIt == buildings.end()) {
        throw invalid_argument(
            "Building does not exist"
        );
    }

    auto citizen = citizenIt->second;
    auto building = buildingIt->second;

    //--------------------------------------
    // Residential buildings forbidden
    //--------------------------------------

    if (building->getType() == "Residential") {
        throw invalid_argument(
            "Residential buildings cannot be workplaces"
        );
    }

    //--------------------------------------
    // Capacity validation
    //--------------------------------------

    if (!building->hasCapacity()) {
        throw runtime_error(
            "Building is full"
        );
    }

    //--------------------------------------
    // Validation hook
    //--------------------------------------

    if (!building->canAcceptCitizen(*citizen)) {
        throw runtime_error(
            "Citizen rejected by building"
        );
    }

    //--------------------------------------
    // Remove previous workplace
    //--------------------------------------

    if (auto oldWorkplace =
        citizen->getWorkplace()) {

        oldWorkplace->removeOccupant(
            citizenId
        );
    }

    //--------------------------------------
    // Synchronize state
    //--------------------------------------

    building->addOccupant(citizen);

    citizen->setWorkplace(building);
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

void CityManager::assignHome(
    int citizenId,
    int buildingId
) {

    //--------------------------------------
    // Validate citizen
    //--------------------------------------

    auto citizenIt = citizens.find(citizenId);

    if (citizenIt == citizens.end()) {
        throw invalid_argument(
            "Citizen does not exist"
        );
    }

    //--------------------------------------
    // Validate building
    //--------------------------------------

    auto buildingIt = buildings.find(buildingId);

    if (buildingIt == buildings.end()) {
        throw invalid_argument(
            "Building does not exist"
        );
    }

    auto citizen = citizenIt->second;
    auto building = buildingIt->second;

    //--------------------------------------
    // Building type validation
    //--------------------------------------

    if (building->getType() != "Residential") {
        throw invalid_argument(
            "Home must be residential"
        );
    }

    //--------------------------------------
    // Capacity validation
    //--------------------------------------

    if (!building->hasCapacity()) {
        throw runtime_error(
            "Building is full"
        );
    }

    //--------------------------------------
    // Validation hook
    //--------------------------------------

    if (!building->canAcceptCitizen(*citizen)) {
        throw runtime_error(
            "Citizen rejected by building"
        );
    }

    //--------------------------------------
    // Remove old home
    //--------------------------------------

    if (auto oldHome = citizen->getHome()) {
        oldHome->removeOccupant(citizenId);
    }

    //--------------------------------------
    // Synchronize state
    //--------------------------------------

    building->addOccupant(citizen);

    citizen->setHome(building);
}

void CityManager::removeBuilding(int buildingId) {

    //--------------------------------------
    // Validate building
    //--------------------------------------

    auto buildingIt = buildings.find(buildingId);

    if (buildingIt == buildings.end()) {
        throw invalid_argument(
            "Building does not exist"
        );
    }

    auto building = buildingIt->second;

    //--------------------------------------
    // Cleanup citizen references
    //--------------------------------------

    auto occupants = building->getOccupants();

    for (auto& citizen : occupants) {

        //----------------------------------
        // Home cleanup
        //----------------------------------

        auto home = citizen->getHome();

        if (home &&
            home->getId() == buildingId) {

            citizen->setHome(nullptr);
        }

        //----------------------------------
        // Workplace cleanup
        //----------------------------------

        auto workplace =
            citizen->getWorkplace();

        if (workplace &&
            workplace->getId() == buildingId) {

            citizen->setWorkplace(nullptr);
        }

        //----------------------------------
        // Location cleanup
        //----------------------------------

        auto location =
            citizen->getLocation();

        if (location &&
            location->getId() == buildingId) {

            citizen->setLocation(nullptr);
        }
    }

    //--------------------------------------
    // Remove building from system
    //--------------------------------------

    buildings.erase(buildingIt);
}

int CityManager::getTotalCitizens() const {
    return static_cast<int>(citizens.size());
}

int CityManager::getTotalBuildings() const {
    return static_cast<int>(buildings.size());
}