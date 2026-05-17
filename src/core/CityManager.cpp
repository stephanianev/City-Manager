#include "CityManager.h"
#include "../events/ModelEvents.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

const unordered_map<int, shared_ptr<Citizen>>&
CityManager::getCitizens() const {
    return citizens;
}

const unordered_map<int, shared_ptr<Building>>&
CityManager::getBuildings() const {
    return buildings;
}

bool CityManager::isBlank(const string& str) const {
    return all_of(str.begin(), str.end(),
        [](unsigned char c) {
            return isspace(c);
        });
}

shared_ptr<Citizen>
CityManager::getCitizenOrThrow(
    int id
) const {

    auto it = citizens.find(id);

    if (it == citizens.end()) {
        throw invalid_argument(
            "Citizen does not exist"
        );
    }

    return it->second;
}

shared_ptr<Building>
CityManager::getBuildingOrThrow(
    int id
) const {

    auto it = buildings.find(id);

    if (it == buildings.end()) {
        throw invalid_argument(
            "Building does not exist"
        );
    }

    return it->second;
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

    eventManager.addEvent(
        make_shared<CitizenCreatedEvent>(
            citizen->getId(),
            citizen->getName()
        )
    );

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

    eventManager.addEvent(
        make_shared<BuildingCreatedEvent>(
            building->getId(),
            building->getName(),
            building->getType()
        )
    );

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

    eventManager.addEvent(
        make_shared<BuildingCreatedEvent>(
            building->getId(),
            building->getName(),
            building->getType()
        )
    );

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

    eventManager.addEvent(
        make_shared<BuildingCreatedEvent>(
            building->getId(),
            building->getName(),
            building->getType()
        )
    );

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

    eventManager.addEvent(
        make_shared<BuildingCreatedEvent>(
            building->getId(),
            building->getName(),
            building->getType()
        )
    );

    return building;
}

shared_ptr<Citizen>
CityManager::restoreCitizen(
    int id,
    const string& name,
    int age,
    const string& profession
) {

    //----------------------------------
    // Prevent duplicate IDs
    //----------------------------------

    if (citizens.count(id)) {
        throw invalid_argument(
            "Citizen ID already exists"
        );
    }

    //----------------------------------
    // Create citizen
    //----------------------------------

    auto citizen =
        make_shared<Citizen>(
            id,
            name,
            age,
            profession
        );

    citizens[id] = citizen;

    //----------------------------------
    // Sync ID counter
    //----------------------------------

    nextCitizenId =
        max(nextCitizenId, id + 1);

    return citizen;
}

shared_ptr<Building> 
CityManager::restoreResidentialBuilding(
        int id,
        const string& name,
        size_t capacity
) {

    //----------------------------------
    // Prevent duplicate IDs
    //----------------------------------

    if (buildings.count(id)) {
        throw invalid_argument(
            "Building ID already exists"
        );
    }

    //----------------------------------
    // Create building
    //----------------------------------

    auto building =
        make_shared<ResidentialBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    //----------------------------------
    // Sync ID counter
    //----------------------------------

    nextBuildingId =
        max(nextBuildingId, id + 1);

    return building;
}

shared_ptr<Building>
CityManager::restoreCommercialBuilding(
    int id,
    const string& name,
    size_t capacity
) {

    //----------------------------------
    // Prevent duplicate IDs
    //----------------------------------

    if (buildings.count(id)) {
        throw invalid_argument(
            "Building ID already exists"
        );
    }

    //----------------------------------
    // Create building
    //----------------------------------

    auto building =
        make_shared<CommercialBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    //----------------------------------
    // Sync ID counter
    //----------------------------------

    nextBuildingId =
        max(nextBuildingId, id + 1);

    return building;
}

shared_ptr<Building>
CityManager::restoreIndustrialBuilding(
    int id,
    const string& name,
    size_t capacity
) {

    //----------------------------------
    // Prevent duplicate IDs
    //----------------------------------

    if (buildings.count(id)) {
        throw invalid_argument(
            "Building ID already exists"
        );
    }

    //----------------------------------
    // Create building
    //----------------------------------

    auto building =
        make_shared<IndustrialBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    //----------------------------------
    // Sync ID counter
    //----------------------------------

    nextBuildingId =
        max(nextBuildingId, id + 1);

    return building;
}


shared_ptr<Building>
CityManager::restoreServiceBuilding(
    int id,
    const string& name,
    size_t capacity
) {

    //----------------------------------
    // Prevent duplicate IDs
    //----------------------------------

    if (buildings.count(id)) {
        throw invalid_argument(
            "Building ID already exists"
        );
    }

    //----------------------------------
    // Create building
    //----------------------------------

    auto building =
        make_shared<ServiceBuilding>(
            id,
            name,
            capacity
        );

    buildings[id] = building;

    //----------------------------------
    // Sync ID counter
    //----------------------------------

    nextBuildingId =
        max(nextBuildingId, id + 1);

    return building;
}

void CityManager::assignWorkplace(
    int citizenId,
    int buildingId
) {

    //--------------------------------------
    // Validate citizen and building
    //--------------------------------------

    auto citizen = getCitizenOrThrow(citizenId);

    auto building = getBuildingOrThrow(buildingId);

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

    eventManager.addEvent(
        make_shared<WorkplaceAssignedEvent>(
            citizenId,
            buildingId
        )
    );
}

void CityManager::moveCitizen(
    int citizenId,
    int buildingId
) {

    //--------------------------------------
    // Validate citizen and building
    //--------------------------------------

    auto citizen = getCitizenOrThrow(citizenId);

    auto building = getBuildingOrThrow(buildingId);

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

    eventManager.addEvent(
        make_shared<CitizenMovedEvent>(
            citizenId,
            buildingId
        )
    );
}

void CityManager::assignHome(
    int citizenId,
    int buildingId
) {

    //--------------------------------------
    // Validate citizen and building
    //--------------------------------------

    auto citizen = getCitizenOrThrow(citizenId);

    auto building = getBuildingOrThrow(buildingId);

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

    eventManager.addEvent(
        make_shared<HomeAssignedEvent>(
            citizenId,
            buildingId
        )
    );
}

void CityManager::removeCitizen(
    int citizenId
) {

    //--------------------------------------
    // Validate citizen
    //--------------------------------------

    auto citizen = getCitizenOrThrow(citizenId);

    //--------------------------------------
    // Remove from current location
    //--------------------------------------

    if (auto location =
        citizen->getLocation()) {

        location->removeOccupant(
            citizenId
        );
    }

    //--------------------------------------
    // Remove citizen from storage
    //--------------------------------------

    eventManager.addEvent(
        make_shared<CitizenRemovedEvent>(citizenId)
    );

    citizens.erase(citizenId);
}

void CityManager::removeBuilding(
    int buildingId
) {

    //--------------------------------------
    // Validate building
    //--------------------------------------

    auto building = getBuildingOrThrow(buildingId);   

    //--------------------------------------
    // Clear citizen references
    //--------------------------------------

    for (const auto& [id, citizen] : citizens) {

        //----------------------------------
        // Home
        //----------------------------------

        if (auto home =
            citizen->getHome()) {

            if (home->getId() == buildingId) {
                citizen->setHome(nullptr);
            }
        }

        //----------------------------------
        // Workplace
        //----------------------------------

        if (auto workplace =
            citizen->getWorkplace()) {

            if (workplace->getId() == buildingId) {
                citizen->setWorkplace(nullptr);
            }
        }

        //----------------------------------
        // Current location
        //----------------------------------

        if (auto location =
            citizen->getLocation()) {

            if (location->getId() == buildingId) {
                citizen->setLocation(nullptr);
            }
        }
    }

    //--------------------------------------
    // Remove building
    //--------------------------------------

    eventManager.addEvent(
        make_shared<BuildingRemovedEvent>(buildingId)
    );

    buildings.erase(buildingId);
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

    auto building = getBuildingOrThrow(buildingId);

    return building->getOccupants();
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

    auto building = getBuildingOrThrow(buildingId);

    if (building->capacity == 0) {
        return 0.0;
    }

    return static_cast<double>(building->getOccupantCount()) /
           building->capacity;
}

const EventManager& CityManager::getEventManager() const {return eventManager;}