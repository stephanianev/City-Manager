#include "Citizen.h"
#include <stdexcept>

using namespace std;

Citizen::Citizen(int id,
                 const string& name,
                 int age,
                 const string& profession)
    : id(id), name(name), age(age), profession(profession) {

    //----------------------------------
    // Validation
    // Minimal invariant checks for direct construction.
    // Full serialization-safe validation happens in Validation:: and
    // CityManager::restoreCitizen before this constructor is used in load paths.
    //----------------------------------

    if (name.empty()) {
        throw invalid_argument(
            "Citizen name cannot be empty"
        );
    }

    if (age < 0 || age > 120) {
        throw invalid_argument(
            "Invalid citizen age"
        );
    }
}

// --- Simple value getters ---
int Citizen::getId() const { return id; }
int Citizen::getAge() const { return age; }
string Citizen::getName() const { return name; }
string Citizen::getProfession() const { return profession; }

// Lock the weak_ptrs when returning so callers get a usable shared_ptr (or
// nullptr if the referenced building has already been destroyed).
shared_ptr<Building> Citizen::getHome() const { return home.lock(); }
shared_ptr<Building> Citizen::getWorkplace() const { return workplace.lock(); }
shared_ptr<Building> Citizen::getLocation() const { return currentLocation.lock(); }

// These setters are private — CityManager calls them after completing all
// validation, capacity checks, and occupant list updates on the building side.
void Citizen::setHome(shared_ptr<Building> b) { home = b; }
void Citizen::setWorkplace(shared_ptr<Building> b) { workplace = b; }
void Citizen::setLocation(shared_ptr<Building> b) { currentLocation = b; }

void Citizen::setAge(int age) {
    if (age < 0 || age > 120) {
        throw invalid_argument("Invalid citizen age.");
    }
    this->age = age;
}

void Citizen::setProfession(const string& profession) {
    // Empty profession is intentionally allowed — it represents an unemployed
    // citizen with no registered trade (displayed as "Jobless" in reports).
    this->profession = profession;
}