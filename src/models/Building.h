#pragma once

#include <string>
#include <vector>
#include <memory>

using namespace std;

class Citizen;
class CityManager;

class Building {
    friend class CityManager;

protected:
    int id;
    string name;
    size_t capacity;

    vector<weak_ptr<Citizen>> occupants;

protected:
    void addOccupant(shared_ptr<Citizen> c);
    void removeOccupant(int citizenId);

public:
    Building(int id,
             const string& name,
             size_t capacity
    );
    
    virtual ~Building() = default;

    int getId() const;
    string getName() const;
    size_t getCapacity() const;

    bool hasCapacity() const;
    size_t getOccupantCount() const;

    vector<shared_ptr<Citizen>> getOccupants() const;

    virtual bool canAcceptCitizen(const Citizen& citizen) const = 0;

    virtual string getType() const = 0;
};