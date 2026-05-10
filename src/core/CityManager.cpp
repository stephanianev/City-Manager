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
    // Synchronize state
    //--------------------------------------

    citizen->setWorkplace(building);
}

void CityManager::moveCitizen(
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
    // Remove old location
    //--------------------------------------

    if (auto oldLocation =
        citizen->getLocation()) {

        oldLocation->removeOccupant(
            citizenId
        );
    }

    //--------------------------------------
    // Add new location
    //--------------------------------------

    building->addOccupant(citizen);

    citizen->setLocation(building);
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
    // Synchronize state
    //--------------------------------------

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

vector<shared_ptr<Citizen>>
CityManager::findCitizensByProfession(
    const string& profession
) const {

    vector<shared_ptr<Citizen>> result;

    for (const auto& [id, citizen] : citizens) {

        if (citizen->getProfession() == profession) {
            result.push_back(citizen);
        }
    }

    return result;
}

vector<shared_ptr<Citizen>>
CityManager::listCitizensInBuilding(
    int buildingId
) const {

    auto it = buildings.find(buildingId);

    if (it == buildings.end()) {
        throw invalid_argument(
            "Building does not exist"
        );
    }

    return it->second->getOccupants();
}

vector<shared_ptr<Building>>
CityManager::findBuildingsWithCapacity() const {

    vector<shared_ptr<Building>> result;

    for (const auto& [id, building] : buildings) {

        if (building->hasCapacity()) {
            result.push_back(building);
        }
    }

    return result;
}

size_t CityManager::getTotalCitizens() const {
    return citizens.size();
}

size_t CityManager::getTotalBuildings() const {
    return buildings.size();
}

size_t CityManager::getHomelessCount() const {

    size_t count = 0;

    for (const auto& [id, citizen] : citizens) {

        if (!citizen->getHome()) {
            count++;
        }
    }

    return count;
}

size_t CityManager::getUnemployedCount() const {

    size_t count = 0;

    for (const auto& [id, citizen] : citizens) {

        if (!citizen->getWorkplace()) {
            count++;
        }
    }

    return count;
}

double CityManager::getOccupancyRate(int buildingId) const {

    auto it = buildings.find(buildingId);

    if (it == buildings.end()) {
        throw invalid_argument(
            "Building does not exist"
        );
    }

    auto building = it->second;

    if (building->capacity == 0) {
        return 0.0;
    }

    return static_cast<double>(building->getOccupantCount()) /
           building->capacity;
}