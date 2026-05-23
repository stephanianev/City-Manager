#pragma once

#include <string>
#include <vector>
#include <memory>
#include "BuildingType.h"

class Citizen;
class CityManager;

class Building {
    friend class CityManager;

    // Occupant management is exclusively handled by CityManager to ensure consistency and event generation
    void addOccupant(std::shared_ptr<Citizen> c);
    void removeOccupant(int citizenId);

protected:
    int id;
    std::string name;
    size_t capacity;

    std::vector<std::weak_ptr<Citizen>> occupants;

public:
    Building(int id,
             const std::string& name,
             size_t capacity);

    virtual ~Building() = default;

    int getId() const;
    std::string getName() const;
    size_t getCapacity() const;

    bool hasCapacity() const;
    size_t getOccupantCount() const;

    std::vector<std::shared_ptr<Citizen>> getOccupants() const;

    void setName(const std::string& name);

    virtual bool canAcceptCitizen(const Citizen& citizen) const = 0;

    virtual std::string getType() const = 0;           // keep for serialization display
    virtual BuildingType getBuildingType() const = 0;  // for internal logic and type checking 
};