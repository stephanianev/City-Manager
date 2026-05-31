#include "CityManager.h"
#include "../events/ModelEvents.h"
#include "../utils/Validation.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace std;

// --- Map accessors (used by CitySerializer) ---

const unordered_map<int, shared_ptr<Citizen>>&
CityManager::getCitizens() const {
    return citizens;
}

const unordered_map<int, shared_ptr<Building>>&
CityManager::getBuildings() const {
    return buildings;
}

// Returns true if str is empty or contains only whitespace characters.
// Used to reject search inputs that would match nothing meaningful.
bool CityManager::isBlank(const string& str) const {
    return all_of(str.begin(), str.end(),
        [](unsigned char c) {
            return isspace(c);
        });
}

// Internal helper: look up a citizen or throw.
// Centralises the find + "does not exist" check so callers don't repeat it.
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

// Internal helper: look up a building or throw.
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

//--------------------------------------------------
// createBuildingImpl<T>
//
// Single template that handles all four building types.
// Type T must be a concrete subclass of Building with a
// (int, string, size_t) constructor.
//
// Steps:
//   1. Validate name and capacity (throws on bad input).
//   2. Consume the next available ID.
//   3. Construct and store the building.
//   4. Emit a BuildingCreatedEvent.
//--------------------------------------------------
template<typename T>
std::shared_ptr<T> CityManager::createBuildingImpl(
    const std::string& name,
    size_t capacity
) {
    Validation::validateBuildingName(name);
    Validation::validateBuildingCapacity(capacity);

    int id = nextBuildingId++;

    auto building = make_shared<T>(id, name, capacity);

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

//--------------------------------------------------
// restoreBuildingImpl<T>
//
// Silent counterpart to createBuildingImpl used during file loading.
// Key differences vs. createBuildingImpl:
//   - Accepts an explicit ID (from the save file) instead of auto-incrementing.
//   - Advances nextBuildingId so post-load creation never reuses a saved ID.
//   - Does NOT emit a BuildingCreatedEvent (restore is not a live creation).
//--------------------------------------------------
template<typename T>
std::shared_ptr<Building> CityManager::restoreBuildingImpl(
    int id,
    const std::string& name,
    size_t capacity
) {
    if (buildings.count(id)) {
        throw invalid_argument("Building ID already exists");
    }

    Validation::validateBuildingName(name);
    Validation::validateBuildingCapacity(capacity);

    auto building = make_shared<T>(id, name, capacity);

    buildings[id] = building;

    // Ensure the next auto-assigned ID is always higher than any restored ID.
    nextBuildingId = max(nextBuildingId, id + 1);

    return building;
}

//--------------------------------------------------
// Citizen creation
//--------------------------------------------------

shared_ptr<Citizen> CityManager::createCitizen(
    const string& name,
    int age,
    const string& profession
) {
    // Validate before touching any state so that a failure leaves the city unchanged.
    Validation::validateCitizenName(name);
    Validation::validateCitizenAge(age);

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

// Thin wrappers — each delegates to the appropriate template instantiation.
shared_ptr<ResidentialBuilding> CityManager::createResidentialBuilding(
    const string& name, size_t capacity
) {
    return createBuildingImpl<ResidentialBuilding>(name, capacity);
}

shared_ptr<CommercialBuilding> CityManager::createCommercialBuilding(
    const string& name, size_t capacity
) {
    return createBuildingImpl<CommercialBuilding>(name, capacity);
}

shared_ptr<IndustrialBuilding> CityManager::createIndustrialBuilding(
    const string& name, size_t capacity
) {
    return createBuildingImpl<IndustrialBuilding>(name, capacity);
}

shared_ptr<ServiceBuilding> CityManager::createServiceBuilding(
    const string& name, size_t capacity
) {
    return createBuildingImpl<ServiceBuilding>(name, capacity);
}

//--------------------------------------------------
// Update operations
//--------------------------------------------------

void CityManager::updateCitizenAge(int citizenId, int age) {
    auto citizen = getCitizenOrThrow(citizenId);
    citizen->setAge(age); // setAge validates the range internally

    eventManager.addEvent(
        make_shared<CitizenUpdatedEvent>(citizenId, citizen->getName())
    );
}

void CityManager::updateCitizenProfession(
    int citizenId,
    const string& profession
) {
    auto citizen = getCitizenOrThrow(citizenId);
    citizen->setProfession(profession);

    eventManager.addEvent(
        make_shared<CitizenUpdatedEvent>(citizenId, citizen->getName())
    );
}

void CityManager::renameBuilding(
    int buildingId,
    const string& name
) {
    Validation::validateBuildingName(name);
    auto building = getBuildingOrThrow(buildingId);
    building->setName(name);

    eventManager.addEvent(
        make_shared<BuildingUpdatedEvent>(buildingId, name)
    );
}

//--------------------------------------------------
// Restoration operations (called by CitySerializer)
//--------------------------------------------------

shared_ptr<Citizen>
CityManager::restoreCitizen(
    int id,
    const string& name,
    int age,
    const string& profession
) {
    // Guard against duplicate IDs — the save file should never produce them,
    // but we fail loudly rather than silently overwriting existing data.
    if (citizens.count(id)) {
        throw invalid_argument("Citizen ID already exists");
    }

    Validation::validateCitizenName(name);
    Validation::validateCitizenAge(age);

    auto citizen =
        make_shared<Citizen>(
            id,
            name,
            age,
            profession
        );

    citizens[id] = citizen;

    // Advance the counter so that post-load createCitizen() won't reuse this ID.
    nextCitizenId = max(nextCitizenId, id + 1);

    return citizen;
}

// Thin wrappers for each building type — see restoreBuildingImpl for shared logic.
shared_ptr<Building> CityManager::restoreResidentialBuilding(
    int id, const string& name, size_t capacity
) {
    return restoreBuildingImpl<ResidentialBuilding>(id, name, capacity);
}

shared_ptr<Building> CityManager::restoreCommercialBuilding(
    int id, const string& name, size_t capacity
) {
    return restoreBuildingImpl<CommercialBuilding>(id, name, capacity);
}

shared_ptr<Building> CityManager::restoreIndustrialBuilding(
    int id, const string& name, size_t capacity
) {
    return restoreBuildingImpl<IndustrialBuilding>(id, name, capacity);
}

shared_ptr<Building> CityManager::restoreServiceBuilding(
    int id, const string& name, size_t capacity
) {
    return restoreBuildingImpl<ServiceBuilding>(id, name, capacity);
}

//--------------------------------------------------
// assignWorkplace
//
// Registers a building as the citizen's employer (logical relationship only).
// This updates only the citizen's weak_ptr and does not change occupants.
// (physically move the citizen by calling moveCitizen)
//
// Restrictions:
//   - Residential buildings cannot be workplaces.
//   - The building must have remaining capacity (based on physical occupants).
//   - The building's canAcceptCitizen() hook must return true.
//--------------------------------------------------
void CityManager::assignWorkplace(
    int citizenId,
    int buildingId
) {
    auto citizen  = getCitizenOrThrow(citizenId);
    auto building = getBuildingOrThrow(buildingId);

    // People live in residential buildings; they work in everything else.
    if (building->getBuildingType() == BuildingType::Residential) {
        throw invalid_argument("Residential buildings cannot be workplaces");
    }

    auto currentWorkspace = citizen->getWorkplace();
    if (currentWorkspace && currentWorkspace->getId() == buildingId) {
        throw invalid_argument("Can not assign current workplace");
    }

    if (!building->hasCapacity()) {
        throw runtime_error("Building is full");
    }

    // canAcceptCitizen is a subclass hook for future admission rules.
    if (!building->canAcceptCitizen(*citizen)) {
        throw runtime_error("Citizen rejected by building");
    }

    // Workplace is a logical relationship only:
    // it updates the citizen's reference, but it does not modify occupants.
    citizen->setWorkplace(building);

    eventManager.addEvent(
        make_shared<WorkplaceAssignedEvent>(
            citizenId,
            buildingId
        )
    );
}

//--------------------------------------------------
// moveCitizen
//
// Physically relocates a citizen to a building (simulation layer).
// This is the only operation that changes Building::occupants.
//
// Steps:
//   1. Validate both IDs.
//   2. Check capacity and acceptance hook.
//   3. Remove citizen from their previous physical location (if any).
//   4. Add citizen to the new building's occupant list.
//   5. Update citizen.currentLocation.
//   6. Emit CitizenMovedEvent.
//--------------------------------------------------
void CityManager::moveCitizen(
    int citizenId,
    int buildingId
) {
    auto citizen  = getCitizenOrThrow(citizenId);
    auto building = getBuildingOrThrow(buildingId);

    // Moving to the same physical location is a no-op.
    if (auto currentLocation = citizen->getLocation()) {
        if (currentLocation->getId() == buildingId) {
            return;
        }
    }

    if (!building->hasCapacity()) {
        throw runtime_error("Building is full");
    }

    if (!building->canAcceptCitizen(*citizen)) {
        throw runtime_error("Citizen rejected by building");
    }

    // Remove from old physical location before adding to the new one.
    if (auto oldLocation = citizen->getLocation()) {
        oldLocation->removeOccupant(citizenId);
    }

    // addOccupant is private to Building; CityManager has friend access.
    // It is idempotent — moving a citizen to the building they're already in
    // is a no-op on the occupants list (handled by the early return above).
    building->addOccupant(citizen);

    citizen->setLocation(building);

    eventManager.addEvent(
        make_shared<CitizenMovedEvent>(
            citizenId,
            buildingId
        )
    );
}

//--------------------------------------------------
// assignHome
//
// Registers a residential building as the citizen's home (logical only).
// This updates only the citizen's weak_ptr and does not change occupants.
//--------------------------------------------------
void CityManager::assignHome(
    int citizenId,
    int buildingId
) {
    auto citizen  = getCitizenOrThrow(citizenId);
    auto building = getBuildingOrThrow(buildingId);

    // Only residential buildings can be homes.
    if (building->getBuildingType() != BuildingType::Residential) {
        throw invalid_argument("Home must be residential");
    }

    auto currentHome = citizen->getHome();
    if (currentHome && currentHome->getId() == buildingId) {
        throw invalid_argument("Can not assign citizen to current home");
    }

    if (!building->hasCapacity()) {
        throw runtime_error("Building is full");
    }

    if (!building->canAcceptCitizen(*citizen)) {
        throw runtime_error("Citizen rejected by building");
    }

    // Home is a logical relationship only:
    // it updates the citizen's reference, but it does not modify occupants.
    citizen->setHome(building);

    eventManager.addEvent(
        make_shared<HomeAssignedEvent>(
            citizenId,
            buildingId
        )
    );
}

//--------------------------------------------------
// removeCitizen
//
// Destroys the city's ownership of a citizen.
// Before erasing the map entry, we clean up the citizen's physical location:
// they are removed from whichever building they are currently in.
// Home and workplace weak_ptrs on the citizen expire automatically once the
// shared_ptr is destroyed — no explicit nulling is needed for those.
//--------------------------------------------------
void CityManager::removeCitizen(
    int citizenId
) {

    auto citizen = getCitizenOrThrow(citizenId);

    // Remove the citizen from whichever building they are physically in.
    if (auto location = citizen->getLocation()) {
        location->removeOccupant(citizenId);
    }

    eventManager.addEvent(
        make_shared<CitizenRemovedEvent>(citizenId)
    );

    citizens.erase(citizenId);
    // At this point the shared_ptr ref count drops; if no other pointer
    // holds the citizen, the Citizen object is destroyed and all weak_ptrs
    // pointing to it automatically become expired.
}

//--------------------------------------------------
// removeBuilding
//
// Destroys the city's ownership of a building.
// Before erasing, we iterate all citizens and explicitly null any home,
// workplace, or location reference that pointed to this building.
// We cannot rely on weak_ptr expiry here because citizens hold weak_ptrs
// to buildings (not the other way around); the weak_ptr would only expire
// when the shared_ptr is destroyed, which happens after we erase it — but
// a citizen could still briefly observe a building ID that no longer exists
// if we didn't clear the references first.
//--------------------------------------------------
void CityManager::removeBuilding(
    int buildingId
) {

    auto building = getBuildingOrThrow(buildingId);

    // Walk every citizen and clear any reference pointing at this building.
    for (const auto& [id, citizen] : citizens) {

        if (auto home = citizen->getHome()) {
            if (home->getId() == buildingId) {
                citizen->setHome(nullptr);
            }
        }

        if (auto workplace = citizen->getWorkplace()) {
            if (workplace->getId() == buildingId) {
                citizen->setWorkplace(nullptr);
            }
        }

        if (auto location = citizen->getLocation()) {
            if (location->getId() == buildingId) {
                citizen->setLocation(nullptr);
            }
        }
    }

    eventManager.addEvent(
        make_shared<BuildingRemovedEvent>(buildingId)
    );

    buildings.erase(buildingId);
}

//--------------------------------------------------
// Query operations
//--------------------------------------------------

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

// Returns physical occupants of a building (set by moveCitizen).
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

// Finds all citizens whose name exactly matches the given string.
// Results are sorted by ID for deterministic ordering (the map iterates
// in hash order, which is not guaranteed to be stable).
vector<shared_ptr<Citizen>> CityManager::findCitizensByName(const string& name) const {
    if (name.empty() || isBlank(name)) {
        throw invalid_argument("Search name cannot be empty or blank");
    }

    vector<shared_ptr<Citizen>> result;

    for (const auto& [id, citizen] : citizens) {
        if (citizen->getName() == name) {
            result.push_back(citizen);
        }
    }

    sort(result.begin(), result.end(),
        [](const shared_ptr<Citizen>& a, const shared_ptr<Citizen>& b) {
            return a->getId() < b->getId();
        });

    return result;
}

// Same as findCitizensByName but for buildings.
vector<shared_ptr<Building>> CityManager::findBuildingsByName(const string& name) const {
    if (name.empty() || isBlank(name)) {
        throw invalid_argument("Search name cannot be empty or blank");
    }

    vector<shared_ptr<Building>> result;

    for (const auto& [id, building] : buildings) {
        if (building->getName() == name) {
            result.push_back(building);
        }
    }

    sort(result.begin(), result.end(),
        [](const shared_ptr<Building>& a, const shared_ptr<Building>& b) {
            return a->getId() < b->getId();
        });

    return result;
}

// Generic citizen filter. Caller supplies a predicate; we return all citizens
// for which predicate returns true. Useful for ad-hoc queries without adding
// new named methods (e.g. "all citizens over 30 who are Engineers").
vector<shared_ptr<Citizen>>
CityManager::queryCitizens(
    function<bool(
        const shared_ptr<Citizen>&
    )> predicate
) const {

    if (!predicate) throw invalid_argument("Predicate cannot be null");

    vector<shared_ptr<Citizen>> result;

    for (const auto& [id, citizen] : citizens) {
        if (predicate(citizen)) {
            result.push_back(citizen);
        }
    }

    return result;
}

// Generic building filter — same pattern as queryCitizens.
vector<shared_ptr<Building>>
CityManager::queryBuildings(
    function<bool(
        const shared_ptr<Building>&
    )> predicate
) const {

    if (!predicate) throw invalid_argument("Predicate cannot be null");

    vector<shared_ptr<Building>> result;

    for (const auto& [id, building] : buildings) {
        if (predicate(building)) {
            result.push_back(building);
        }
    }

    return result;
}

//--------------------------------------------------
// Statistics
//--------------------------------------------------

size_t CityManager::getTotalCitizens() const {
    return citizens.size();
}

size_t CityManager::getTotalBuildings() const {
    return buildings.size();
}

size_t CityManager::getHomelessCount() const {

    size_t count = 0;

    for (const auto& [id, citizen] : citizens) {
        // getHome() returns nullptr if no home is assigned.
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

// Returns the physical occupancy ratio for a single building in [0.0, 1.0].
// The capacity == 0 guard is defensive — Building's constructor already
// rejects zero capacity, but a future code path might bypass that.
double CityManager::getOccupancyRate(int buildingId) const {

    auto building = getBuildingOrThrow(buildingId);

    if (building->getCapacity() == 0) {
        return 0.0;
    }

    return static_cast<double>(building->getOccupantCount()) /
           building->getCapacity();
}

double CityManager::getAverageCitizenAge() const {

    if (citizens.empty()) {
        return 0.0;
    }

    unsigned long totalAge = 0;

    for (const auto& [id, citizen] : citizens) {
        totalAge += citizen->getAge();
    }

    return static_cast<double>(totalAge) / citizens.size();
}

// Returns the fraction of citizens who have a workplace assigned [0.0, 1.0].
double CityManager::getEmploymentRate() const {

    if (citizens.empty()) {
        return 0.0;
    }

    unsigned long employed = 0;

    for (const auto& [id, citizen] : citizens) {
        if (citizen->getWorkplace()) {
            employed++;
        }
    }

    return static_cast<double>(employed) / citizens.size();
}

// Computes the average physical occupancy ratio per building type.
// Uses two parallel maps (total occupants and total capacity per type) so
// we can compute the aggregate ratio in a single pass over all buildings,
// then divide at the end.
unordered_map<string, double>
CityManager::getAverageOccupancyByType() const {

    unordered_map<string, int> totalOccupants;
    unordered_map<string, int> totalCapacity;

    for (const auto& [id, building] : buildings) {
        totalOccupants[building->getType()] += building->getOccupantCount();
        totalCapacity [building->getType()] += building->getCapacity();
    }

    unordered_map<string, double> result;

    for (const auto& [type, capacity] : totalCapacity) {
        if (capacity == 0) {
            result[type] = 0.0;
        }
        else {
            result[type] =
                static_cast<double>(totalOccupants[type]) / capacity;
        }
    }

    return result;
}

const EventManager& CityManager::getEventManager() const { return eventManager; }

//--------------------------------------------------
// Sorting
//
// Both sort functions take their input vector by value (copy) so that the
// caller's original collection is not modified. The copy is sorted in-place
// and returned — this is an intentional "sort a snapshot" pattern.
//--------------------------------------------------

vector<shared_ptr<Citizen>>
CityManager::sortCitizens(
    vector<shared_ptr<Citizen>> citizensToSort,
    function<bool(
        const shared_ptr<Citizen>&,
        const shared_ptr<Citizen>&
    )> comparator
) const {

    sort(
        citizensToSort.begin(),
        citizensToSort.end(),
        comparator
    );

    return citizensToSort;
}

vector<shared_ptr<Building>>
CityManager::sortBuildings(
    vector<shared_ptr<Building>> buildingsToSort,
    function<bool(
        const shared_ptr<Building>&,
        const shared_ptr<Building>&
    )> comparator
) const {

    sort(
        buildingsToSort.begin(),
        buildingsToSort.end(),
        comparator
    );

    return buildingsToSort;
}

//--------------------------------------------------
// Reporting
//--------------------------------------------------

// Counts citizens per profession string. Citizens with no profession
// (empty string) appear under "" — the UI displays them as "Jobless".
unordered_map<string, int>
CityManager::getProfessionDistribution() const {

    unordered_map<string, int> result;

    for (const auto& [id, citizen] : citizens) {
        result[citizen->getProfession()]++;
    }

    return result;
}

unordered_map<string, int>
CityManager::getBuildingTypeDistribution() const {

    unordered_map<string, int> result;

    for (const auto& [id, building] : buildings) {
        result[building->getType()]++;
    }

    return result;
}

// Returns (name, occupancy ratio) for every building.
// Useful for showing a per-building breakdown in the UI.
vector<pair<string, double>>
CityManager::getBuildingOccupancyReport() const {

    vector<pair<string, double>> result;

    for (const auto& [id, building] : buildings) {

        double rate = 0.0;

        if (building->getCapacity() > 0) {
            rate = static_cast<double>(building->getOccupantCount())
                   / building->getCapacity();
        }

        result.push_back(std::make_pair(
            building->getName(),
            rate
        ));
    }

    return result;
}