#pragma once // header guard

#include <string>
#include <memory>

class Building;

class Citizen {
    friend class CityManager;

private:
    int id;
    std::string name;
    int age;
    std::string profession;

    std::weak_ptr<Building> home;
    std::weak_ptr<Building> workplace;
    std::weak_ptr<Building> currentLocation;

    void setHome(std::shared_ptr<Building>); 
    void setWorkplace(std::shared_ptr<Building>);
    void setLocation(std::shared_ptr<Building>);

public:
    Citizen(int id,
            const std::string& name,
            int age,
            const std::string& profession);

    int getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getProfession() const;

    std::shared_ptr<Building> getHome() const;
    std::shared_ptr<Building> getWorkplace() const;
    std::shared_ptr<Building> getLocation() const;

    void setAge(int age);
    void setProfession(const std::string& profession);
};