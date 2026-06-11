#include "ConsoleUI.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

#include "../serialization/CitySerializer.h"

using namespace std;

namespace {
    vector<shared_ptr<Citizen>> citizensFromMap(const unordered_map<int, shared_ptr<Citizen>>& src) {
        vector<shared_ptr<Citizen>> result;
        result.reserve(src.size());
        for (const auto& [id, citizen] : src) {
            result.push_back(citizen);
        }
        return result;
    }

    vector<shared_ptr<Building>> buildingsFromMap(const unordered_map<int, shared_ptr<Building>>& src) {
        vector<shared_ptr<Building>> result;
        result.reserve(src.size());
        for (const auto& [id, building] : src) {
            result.push_back(building);
        }
        return result;
    }
}

ConsoleUI::ConsoleUI()
    : city(make_unique<CityManager>()),
      running(true) {}

void ConsoleUI::run() {
    cout << "Smart City Manager UI\n";

    while (running) {
        try {
            printMainMenu();
            int choice = promptChoice("Select an option: ", 0, 10);

            switch (choice) {
                case 1: citizenMenu(); break;
                case 2: buildingMenu(); break;
                case 3: relationshipMenu(); break;
                case 4: movementMenu(); break;
                case 5: queryMenu(); break;
                case 6: statsMenu(); break;
                case 7: eventMenu(); break;
                case 8: persistenceMenu(); break;
                case 9: demoScenario(); break;
                case 10: printAllState(); break;
                case 0: running = false; break;
                default: break;
            }
        } catch (const exception& e) {
            cout << "\nError: " << e.what() << "\n";
            pause();
        }
    }

    cout << "Goodbye.\n";
}

void ConsoleUI::printMainMenu() const {
    cout << "\n========================================\n";
    cout << "SMART CITY MANAGER\n";
    cout << "========================================\n";
    cout << "1. Citizen Management\n";
    cout << "2. Building Management\n";
    cout << "3. Assignments\n";
    cout << "4. Movement\n";
    cout << "5. Search and Queries\n";
    cout << "6. Statistics and Reports\n";
    cout << "7. Event Log\n";
    cout << "8. Save and Load\n";
    cout << "9. Demo Scenario\n";
    cout << "10. Print Full State\n";
    cout << "0. Exit\n";
}

void ConsoleUI::resetCity() {
    city = make_unique<CityManager>();
}

void ConsoleUI::ensureCity() {
    if (!city) {
        city = make_unique<CityManager>();
    }
}

string ConsoleUI::trim(const string& value) {
    size_t first = 0;
    while (first < value.size() && isspace(static_cast<unsigned char>(value[first]))) {
        ++first;
    }

    size_t last = value.size();
    while (last > first && isspace(static_cast<unsigned char>(value[last - 1]))) {
        --last;
    }

    return value.substr(first, last - first);
}

string ConsoleUI::formatPercent(double value) {
    ostringstream oss;
    oss << fixed << setprecision(2) << (value * 100.0) << "%";
    return oss.str();
}

string ConsoleUI::buildingRef(const shared_ptr<Building>& building) {
    if (!building) {
        return "-";
    }

    return to_string(building->getId()) + " (" + building->getName() + ")";
}

string ConsoleUI::professionLabel(const string& profession) {
    return profession.empty() ? "Jobless" : profession;
}

string ConsoleUI::promptLine(const string& prompt, bool allowEmpty) const {
    string value;

    while (true) {
        cout << prompt;
        getline(cin, value);

        if (allowEmpty) {
            return value;
        }

        if (!trim(value).empty()) {
            return value;
        }

        cout << "Value cannot be empty.\n";
    }
}

int ConsoleUI::promptInt(const string& prompt) const {
    while (true) {
        string raw = trim(promptLine(prompt, true));
        try {
            size_t pos = 0;
            int value = stoi(raw, &pos);
            if (pos != raw.size()) {
                throw invalid_argument("Trailing characters");
            }
            return value;
        } catch (...) {
            cout << "Please enter a valid integer.\n";
        }
    }
}

size_t ConsoleUI::promptSize(const string& prompt) const {
    while (true) {
        string raw = trim(promptLine(prompt, true));
        try {
            size_t pos = 0;
            unsigned long long value = stoull(raw, &pos);
            if (pos != raw.size()) {
                throw invalid_argument("Trailing characters");
            }
            return static_cast<size_t>(value);
        } catch (...) {
            cout << "Please enter a valid non-negative integer.\n";
        }
    }
}

double ConsoleUI::promptDouble(const string& prompt) const {
    while (true) {
        string raw = trim(promptLine(prompt, true));
        try {
            size_t pos = 0;
            double value = stod(raw, &pos);
            if (pos != raw.size()) {
                throw invalid_argument("Trailing characters");
            }
            return value;
        } catch (...) {
            cout << "Please enter a valid number.\n";
        }
    }
}

int ConsoleUI::promptChoice(const string& prompt, int minChoice, int maxChoice) const {
    while (true) {
        int choice = promptInt(prompt);
        if (choice >= minChoice && choice <= maxChoice) {
            return choice;
        }
        cout << "Choice must be between " << minChoice << " and " << maxChoice << ".\n";
    }
}

void ConsoleUI::pause() const {
    cout << "Press Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

void ConsoleUI::printCitizen(const shared_ptr<Citizen>& citizen) const {
    if (!citizen) {
        return;
    }

    cout << "ID: " << citizen->getId()
         << " | Name: " << citizen->getName()
         << " | Age: " << citizen->getAge()
         << " | Profession: " << professionLabel(citizen->getProfession())
         << " | Home: " << buildingRef(citizen->getHome())
         << " | Workplace: " << buildingRef(citizen->getWorkplace())
         << " | Location: " << buildingRef(citizen->getLocation())
         << "\n";
}

void ConsoleUI::printBuilding(const shared_ptr<Building>& building) const {
    if (!building) {
        return;
    }

    cout << "ID: " << building->getId()
         << " | Type: " << building->getType()
         << " | Name: " << building->getName()
         << " | Capacity: " << building->getCapacity()
         << " | Occupants: " << building->getOccupantCount()
         << " | Free: " << (building->hasCapacity() ? "Yes" : "No")
         << " | Occupancy: " << formatPercent(
                building->getCapacity() == 0 ? 0.0
                                              : static_cast<double>(building->getOccupantCount()) / building->getCapacity()
            )
         << "\n";
}

void ConsoleUI::printCitizenList(const vector<shared_ptr<Citizen>>& citizens) const {
    if (citizens.empty()) {
        cout << "No citizens found.\n";
        return;
    }

    for (const auto& citizen : citizens) {
        printCitizen(citizen);
    }
}

void ConsoleUI::printBuildingList(const vector<shared_ptr<Building>>& buildings) const {
    if (buildings.empty()) {
        cout << "No buildings found.\n";
        return;
    }

    for (const auto& building : buildings) {
        printBuilding(building);
    }
}

void ConsoleUI::printDistribution(
    const unordered_map<string, int>& data,
    const string& title
) const {
    cout << title << "\n";
    if (data.empty()) {
        cout << "  (empty)\n";
        return;
    }

    vector<pair<string, int>> items(data.begin(), data.end());
    sort(items.begin(), items.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& [key, value] : items) {
        cout << "  " << (key.empty() ? "Jobless" : key) << ": " << value << "\n";
    }
}

void ConsoleUI::printDistribution(
    const unordered_map<string, double>& data,
    const string& title
) const {
    cout << title << "\n";
    if (data.empty()) {
        cout << "  (empty)\n";
        return;
    }

    vector<pair<string, double>> items(data.begin(), data.end());
    sort(items.begin(), items.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& [key, value] : items) {
        cout << "  " << key << ": " << formatPercent(value) << "\n";
    }
}

void ConsoleUI::printOccupancyReport() const {
    const auto report = city->getBuildingOccupancyReport();
    cout << "Occupancy report by building\n";

    if (report.empty()) {
        cout << "  (empty)\n";
        return;
    }

    for (const auto& [name, ratio] : report) {
        cout << "  " << name << ": " << formatPercent(ratio) << "\n";
    }
}

void ConsoleUI::printGeneralStats() const {
    cout << "General statistics\n";
    cout << "  Total citizens: " << city->getTotalCitizens() << "\n";
    cout << "  Total buildings: " << city->getTotalBuildings() << "\n";
    cout << "  Average citizen age: " << fixed << setprecision(2) << city->getAverageCitizenAge() << "\n";
    cout << "  Employment rate: " << formatPercent(city->getEmploymentRate()) << "\n";
    cout << "  Homeless citizens: " << city->getHomelessCount() << "\n";
    cout << "  Unemployed citizens: " << city->getUnemployedCount() << "\n";
    cout.unsetf(ios::floatfield);
}

void ConsoleUI::printAllState() const {
    cout << "\n=== FULL CITY STATE ===\n";

    cout << "\nCitizens:\n";
    auto citizens = citizensFromMap(city->getCitizens());
    sort(citizens.begin(), citizens.end(),
        [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
    printCitizenList(citizens);

    cout << "\nBuildings:\n";
    auto buildings = buildingsFromMap(city->getBuildings());
    sort(buildings.begin(), buildings.end(),
        [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
    printBuildingList(buildings);

    cout << "\n";
    printGeneralStats();
    cout << "\n";
    printDistribution(city->getProfessionDistribution(), "Profession distribution");
    cout << "\n";
    printDistribution(city->getBuildingTypeDistribution(), "Building type distribution");
    cout << "\n";
    printDistribution(city->getAverageOccupancyByType(), "Average occupancy by building type");
    cout << "\n";
    printOccupancyReport();
    cout << "\n";
    printEvents(20);
    pause();
}

void ConsoleUI::printEvents(size_t limit) const {
    const auto& events = city->getEventManager().getEvents();

    cout << "Event log (" << events.size() << " total)\n";
    if (events.empty()) {
        cout << "  (no events)\n";
        return;
    }

    size_t shown = min(limit, events.size());
    for (size_t i = 0; i < shown; ++i) {
        cout << "  " << events[i]->getType() << " -> " << events[i]->getDescription() << "\n";
    }

    if (events.size() > shown) {
        cout << "  ... " << (events.size() - shown) << " more\n";
    }
}

void ConsoleUI::citizenMenu() {
    while (true) {
        cout << "\n--- Citizen Management ---\n";
        cout << "1. Create citizen\n";
        cout << "2. Update citizen age\n";
        cout << "3. Update citizen profession\n";
        cout << "4. Remove citizen\n";
        cout << "5. List all citizens\n";
        cout << "6. Search citizens by name\n";
        cout << "7. Search citizens by profession\n";
        cout << "8. Sort citizens\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 8);
        if (choice == 0) return;

        try {
            switch (choice) {
                case 1: {
                    string name = promptLine("Name: ");
                    int age = promptInt("Age: ");
                    string profession = promptLine("Profession (leave empty for Jobless): ", true);
                    auto citizen = city->createCitizen(name, age, profession);
                    cout << "Created citizen with ID " << citizen->getId() << ".\n";
                    break;
                }
                case 2: {
                    int id = promptInt("Citizen ID: ");
                    int age = promptInt("New age: ");
                    city->updateCitizenAge(id, age);
                    cout << "Citizen updated.\n";
                    break;
                }
                case 3: {
                    int id = promptInt("Citizen ID: ");
                    string profession = promptLine("New profession (leave empty for Jobless): ", true);
                    city->updateCitizenProfession(id, profession);
                    cout << "Citizen updated.\n";
                    break;
                }
                case 4: {
                    int id = promptInt("Citizen ID: ");
                    city->removeCitizen(id);
                    cout << "Citizen removed.\n";
                    break;
                }
                case 5: {
                    auto citizens = citizensFromMap(city->getCitizens());
                    sort(citizens.begin(), citizens.end(),
                        [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
                    printCitizenList(citizens);
                    break;
                }
                case 6: {
                    string name = promptLine("Exact name: ");
                    auto result = city->findCitizensByName(name);
                    printCitizenList(result);
                    break;
                }
                case 7: {
                    string profession = promptLine("Exact profession (empty = Jobless): ", true);
                    auto result = city->findCitizensByProfession(profession);
                    printCitizenList(result);
                    break;
                }
                case 8: {
                    auto citizens = citizensFromMap(city->getCitizens());
                    cout << "Sort by:\n";
                    cout << "1. ID\n";
                    cout << "2. Name\n";
                    cout << "3. Age\n";
                    cout << "4. Profession\n";
                    int sortChoice = promptChoice("Select: ", 1, 4);

                    if (sortChoice == 1) {
                        citizens = city->sortCitizens(std::move(citizens),
                            [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
                    } else if (sortChoice == 2) {
                        citizens = city->sortCitizens(std::move(citizens),
                            [](const auto& a, const auto& b) { return a->getName() < b->getName(); });
                    } else if (sortChoice == 3) {
                        citizens = city->sortCitizens(std::move(citizens),
                            [](const auto& a, const auto& b) { return a->getAge() < b->getAge(); });
                    } else {
                        citizens = city->sortCitizens(std::move(citizens),
                            [](const auto& a, const auto& b) { return a->getProfession() < b->getProfession(); });
                    }

                    printCitizenList(citizens);
                    break;
                }
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::buildingMenu() {
    while (true) {
        cout << "\n--- Building Management ---\n";
        cout << "1. Create residential building\n";
        cout << "2. Create commercial building\n";
        cout << "3. Create industrial building\n";
        cout << "4. Create service building\n";
        cout << "5. Rename building\n";
        cout << "6. Remove building\n";
        cout << "7. List all buildings\n";
        cout << "8. Search buildings by name\n";
        cout << "9. List buildings with free capacity\n";
        cout << "10. Sort buildings\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 10);
        if (choice == 0) return;

        try {
            switch (choice) {
                case 1:
                case 2:
                case 3:
                case 4: {
                    string name = promptLine("Name: ");
                    size_t capacity = promptSize("Capacity: ");
                    shared_ptr<Building> building;

                    if (choice == 1) building = city->createResidentialBuilding(name, capacity);
                    if (choice == 2) building = city->createCommercialBuilding(name, capacity);
                    if (choice == 3) building = city->createIndustrialBuilding(name, capacity);
                    if (choice == 4) building = city->createServiceBuilding(name, capacity);

                    cout << "Created building with ID " << building->getId() << ".\n";
                    break;
                }
                case 5: {
                    int id = promptInt("Building ID: ");
                    string name = promptLine("New name: ");
                    city->renameBuilding(id, name);
                    cout << "Building renamed.\n";
                    break;
                }
                case 6: {
                    int id = promptInt("Building ID: ");
                    city->removeBuilding(id);
                    cout << "Building removed.\n";
                    break;
                }
                case 7: {
                    auto buildings = buildingsFromMap(city->getBuildings());
                    sort(buildings.begin(), buildings.end(),
                        [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
                    printBuildingList(buildings);
                    break;
                }
                case 8: {
                    string name = promptLine("Exact name: ");
                    auto result = city->findBuildingsByName(name);
                    printBuildingList(result);
                    break;
                }
                case 9: {
                    auto result = city->findBuildingsWithCapacity();
                    printBuildingList(result);
                    break;
                }
                case 10: {
                    auto buildings = buildingsFromMap(city->getBuildings());
                    cout << "Sort by:\n";
                    cout << "1. ID\n";
                    cout << "2. Name\n";
                    cout << "3. Capacity\n";
                    cout << "4. Occupant count\n";
                    int sortChoice = promptChoice("Select: ", 1, 4);

                    if (sortChoice == 1) {
                        buildings = city->sortBuildings(std::move(buildings),
                            [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
                    } else if (sortChoice == 2) {
                        buildings = city->sortBuildings(std::move(buildings),
                            [](const auto& a, const auto& b) { return a->getName() < b->getName(); });
                    } else if (sortChoice == 3) {
                        buildings = city->sortBuildings(std::move(buildings),
                            [](const auto& a, const auto& b) { return a->getCapacity() < b->getCapacity(); });
                    } else {
                        buildings = city->sortBuildings(std::move(buildings),
                            [](const auto& a, const auto& b) { return a->getOccupantCount() < b->getOccupantCount(); });
                    }

                    printBuildingList(buildings);
                    break;
                }
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::relationshipMenu() {
    while (true) {
        cout << "\n--- Assignments ---\n";
        cout << "1. Assign home\n";
        cout << "2. Assign workplace\n";
        cout << "3. View citizen details\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 3);
        if (choice == 0) return;

        try {
            if (choice == 1 || choice == 2) {
                int citizenId = promptInt("Citizen ID: ");
                int buildingId = promptInt("Building ID: ");
                if (choice == 1) {
                    city->assignHome(citizenId, buildingId);
                    cout << "Home assigned.\n";
                } else {
                    city->assignWorkplace(citizenId, buildingId);
                    cout << "Workplace assigned.\n";
                }
            } else if (choice == 3) {
                int citizenId = promptInt("Citizen ID: ");
                const auto& citizens = city->getCitizens();
                auto it = citizens.find(citizenId);
                if (it == citizens.end()) {
                    throw invalid_argument("Citizen not found");
                }
                printCitizen(it->second);
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::movementMenu() {
    while (true) {
        cout << "\n--- Movement ---\n";
        cout << "1. Move citizen\n";
        cout << "2. List occupants in building\n";
        cout << "3. Show building occupancy rate\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 3);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                int citizenId = promptInt("Citizen ID: ");
                int buildingId = promptInt("Destination building ID: ");
                city->moveCitizen(citizenId, buildingId);
                cout << "Citizen moved.\n";
            } else if (choice == 2) {
                int buildingId = promptInt("Building ID: ");
                auto occupants = city->listCitizensInBuilding(buildingId);
                printCitizenList(occupants);
            } else if (choice == 3) {
                int buildingId = promptInt("Building ID: ");
                cout << "Occupancy rate: " << formatPercent(city->getOccupancyRate(buildingId)) << "\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::queryMenu() {
    while (true) {
        cout << "\n--- Search and Queries ---\n";
        cout << "1. Citizens by exact name\n";
        cout << "2. Citizens by profession\n";
        cout << "3. Buildings by exact name\n";
        cout << "4. Buildings with free capacity\n";
        cout << "5. Citizens older than age\n";
        cout << "6. Citizens who are homeless\n";
        cout << "7. Citizens who are unemployed\n";
        cout << "8. Buildings by type\n";
        cout << "9. Buildings above occupancy threshold\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 9);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                string name = promptLine("Exact name: ");
                printCitizenList(city->findCitizensByName(name));
            } else if (choice == 2) {
                string profession = promptLine("Exact profession (leave empty for Jobless): ", true);
                printCitizenList(city->findCitizensByProfession(profession));
            } else if (choice == 3) {
                string name = promptLine("Exact name: ");
                printBuildingList(city->findBuildingsByName(name));
            } else if (choice == 4) {
                printBuildingList(city->findBuildingsWithCapacity());
            } else if (choice == 5) {
                int minAge = promptInt("Minimum age: ");
                auto result = city->queryCitizens([minAge](const shared_ptr<Citizen>& c) {
                    return c->getAge() > minAge;
                });
                printCitizenList(result);
            } else if (choice == 6) {
                auto result = city->queryCitizens([](const shared_ptr<Citizen>& c) {
                    return !c->getHome();
                });
                printCitizenList(result);
            } else if (choice == 7) {
                auto result = city->queryCitizens([](const shared_ptr<Citizen>& c) {
                    return !c->getWorkplace();
                });
                printCitizenList(result);
            } else if (choice == 8) {
                cout << "Type options: Residential, Commercial, Industrial, Service\n";
                string type = promptLine("Type: ");
                auto result = city->queryBuildings([&type](const shared_ptr<Building>& b) {
                    return b->getType() == type;
                });
                printBuildingList(result);
            } else if (choice == 9) {
                double threshold = promptDouble("Minimum occupancy ratio as percent (e.g. 50 for 50%): ");
                auto result = city->queryBuildings([threshold](const shared_ptr<Building>& b) {
                    double ratio = b->getCapacity() == 0 ? 0.0
                        : static_cast<double>(b->getOccupantCount()) / b->getCapacity();
                    return ratio >= (threshold / 100.0);
                });
                printBuildingList(result);
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::statsMenu() {
    while (true) {
        cout << "\n--- Statistics and Reports ---\n";
        cout << "1. General statistics\n";
        cout << "2. Profession distribution\n";
        cout << "3. Building type distribution\n";
        cout << "4. Average occupancy by building type\n";
        cout << "5. Occupancy report by building\n";
        cout << "6. Occupancy rate for building\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 6);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                printGeneralStats();
            } else if (choice == 2) {
                printDistribution(city->getProfessionDistribution(), "Profession distribution");
            } else if (choice == 3) {
                printDistribution(city->getBuildingTypeDistribution(), "Building type distribution");
            } else if (choice == 4) {
                printDistribution(city->getAverageOccupancyByType(), "Average occupancy by building type");
            } else if (choice == 5) {
                printOccupancyReport();
            } else if (choice == 6) {
                int buildingId = promptInt("Building ID: ");
                cout << "Occupancy rate: " << formatPercent(city->getOccupancyRate(buildingId)) << "\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::eventMenu() {
    while (true) {
        cout << "\n--- Event Log ---\n";
        cout << "1. Show last 10 events\n";
        cout << "2. Show last N events\n";
        cout << "3. Show event count\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 3);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                printEvents(10);
            } else if (choice == 2) {
                size_t n = promptSize("How many events to show: ");
                printEvents(n);
            } else if (choice == 3) {
                cout << "Event count: " << city->getEventManager().getEventCount() << "\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::persistenceMenu() {
    while (true) {
        cout << "\n--- Save and Load ---\n";
        cout << "1. Save city\n";
        cout << "2. Load city\n";
        cout << "3. Reset current city\n";
        cout << "0. Back\n";

        int choice = promptChoice("Select an option: ", 0, 3);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                string filename = promptLine("File name: ");
                CitySerializer::saveToFile(*city, filename);
                cout << "City saved.\n";
            } else if (choice == 2) {
                string filename = promptLine("File name: ");
                auto loadedCity = make_unique<CityManager>();
                CitySerializer::loadFromFile(*loadedCity, filename);
                city = std::move(loadedCity);
                cout << "City loaded.\n";
            } else if (choice == 3) {
                resetCity();
                cout << "City reset.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

void ConsoleUI::demoScenario() {
    resetCity();

    cout << "\nCreating demo city...\n";

    auto home1 = city->createResidentialBuilding("Oak Residences", 4);
    auto home2 = city->createResidentialBuilding("River Apartments", 3);
    auto office = city->createCommercialBuilding("Central Office", 5);
    auto factory = city->createIndustrialBuilding("North Factory", 4);
    auto clinic = city->createServiceBuilding("City Clinic", 3);

    auto anna = city->createCitizen("Anna Petrova", 28, "Architect");
    auto boris = city->createCitizen("Boris Ivanov", 41, "Engineer");
    auto celine = city->createCitizen("Celine Dimitrova", 22, "");
    auto dimitar = city->createCitizen("Dimitar Kolev", 35, "Doctor");
    auto elena = city->createCitizen("Elena Stoyanova", 30, "Technician");

    city->assignHome(anna->getId(), home1->getId());
    city->assignHome(boris->getId(), home1->getId());
    city->assignHome(celine->getId(), home2->getId());
    city->assignHome(dimitar->getId(), home2->getId());
    city->assignHome(elena->getId(), home2->getId());

    city->assignWorkplace(anna->getId(), office->getId());
    city->assignWorkplace(boris->getId(), factory->getId());
    city->assignWorkplace(celine->getId(), office->getId());
    city->assignWorkplace(dimitar->getId(), clinic->getId());
    city->assignWorkplace(elena->getId(), factory->getId());

    city->moveCitizen(anna->getId(), office->getId());
    city->moveCitizen(boris->getId(), factory->getId());
    city->moveCitizen(celine->getId(), office->getId());
    city->moveCitizen(dimitar->getId(), clinic->getId());
    city->moveCitizen(elena->getId(), factory->getId());

    cout << "Demo scenario created.\n";
    printAllState();
}
