#pragma once

#include <string>
#include <memory>

using namespace std;

class Building;

class Citizen {
private:
    int id;
    string name;
    int age;
    string profession;

    weak_ptr<Building> home;
    weak_ptr<Building> workplace;
    weak_ptr<Building> currentLocation;

public:
    Citizen(int id,
            const string& name,
            int age,
            const string& profession);

    int getId() const;
    string getName() const;
    string getProfession() const;

    shared_ptr<Building> getHome() const;
    shared_ptr<Building> getWorkplace() const;
    shared_ptr<Building> getLocation() const;

    void setHome(shared_ptr<Building>);
    void setWorkplace(shared_ptr<Building>);
    void setLocation(shared_ptr<Building>);
};