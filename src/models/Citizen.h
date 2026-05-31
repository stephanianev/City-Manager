#pragma once

#include <string>
#include <memory>

class Building;

// Represents a single resident of the city.
//
// A citizen has three distinct building relationships:
//   - home:            the residential building they are registered to live in
//   - workplace:       the building they are registered to work at
//   - currentLocation: the building they are physically present in right now
//                      (tracked by moveCitizen; used for the future simulation system)
//
// All three use weak_ptr<Building> to avoid circular ownership cycles — the
// city's CityManager owns the shared_ptrs; citizens only observe them.
// Calling getHome/getWorkplace/getLocation locks the weak_ptr and returns a
// shared_ptr (null if the building has since been removed).
//
// The setHome/setWorkplace/setLocation setters are private and only accessible
// to CityManager (via friend). This keeps all assignment logic — validation,
// capacity checks, and event emission — centralised there.
class Citizen {
    friend class CityManager;

private:
    int id;
    std::string name;
    int age;
    std::string profession;

    // Weak references: citizens observe buildings but do not own them.
    // If a building is destroyed, these automatically become expired.
    std::weak_ptr<Building> home;
    std::weak_ptr<Building> workplace;
    std::weak_ptr<Building> currentLocation; // physical whereabouts for the simulation layer

    // Private — only CityManager may call these, ensuring that every
    // assignment goes through validation, capacity checks, and event emission.
    void setHome(std::shared_ptr<Building>);
    void setWorkplace(std::shared_ptr<Building>);
    void setLocation(std::shared_ptr<Building>);

public:
    Citizen(int id,
            const std::string& name,
            int age,
            const std::string& profession);

    // --- Getters ---
    int getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getProfession() const;

    // Lock the weak_ptr and return the building, or nullptr if it was removed.
    std::shared_ptr<Building> getHome() const;
    std::shared_ptr<Building> getWorkplace() const;
    std::shared_ptr<Building> getLocation() const;

    // --- Public setters (data fields only, not building relationships) ---
    void setAge(int age);
    void setProfession(const std::string& profession);
};