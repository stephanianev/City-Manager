#include "CityManager.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

bool CityManager::isBlank(const string& str) const {
    return all_of(str.begin(), str.end(),
        [](unsigned char c) { return isspace(c); });
}

shared_ptr<Citizen> CityManager::createCitizen(
    const string& name,
    int age,
    const string& profession
) {
    if (name.empty() || isBlank(name)) {
        throw invalid_argument("Citizen name cannot be empty or blank");
    }

    if (age < 0 || age > 120) {
        throw invalid_argument("Age must be between 0 and 120");
    }

    int id = nextCitizenId++;

    auto citizen = make_shared<Citizen>(id, name, age, profession);

    citizens[id] = citizen;

    return citizen;
}

int CityManager::getTotalCitizens() const {
    return static_cast<int>(citizens.size());
}