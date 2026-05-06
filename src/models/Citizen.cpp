#include "Citizen.h"

Citizen::Citizen(int id,
                 const string& name,
                 int age,
                 const string& profession)
    : id(id), name(name), age(age), profession(profession) {}

int Citizen::getId() const { return id; }
string Citizen::getName() const { return name; }
string Citizen::getProfession() const { return profession; }

shared_ptr<Building> Citizen::getHome() const { return home.lock(); }
shared_ptr<Building> Citizen::getWorkplace() const { return workplace.lock(); }
shared_ptr<Building> Citizen::getLocation() const { return currentLocation.lock(); }

void Citizen::setHome(shared_ptr<Building> b) { home = b; }
void Citizen::setWorkplace(shared_ptr<Building> b) { workplace = b; }
void Citizen::setLocation(shared_ptr<Building> b) { currentLocation = b; }