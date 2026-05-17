#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cmath>
#include <cstdio>

#include "core/CityManager.h"
#include "serialization/CitySerializer.h"

using namespace std;

struct TestResult {
    string name;
    bool passed;
    string message;
};

class TestRunner {
private:
    vector<TestResult> results;

public:
    void run(const string& name, function<void()> test) {
        try {
            test();
            results.push_back({name, true, "OK"});
        }
        catch (const exception& e) {
            results.push_back({name, false, e.what()});
        }
        catch (...) {
            results.push_back({name, false, "Unknown error"});
        }
    }

    void report() const {
        int passed = 0;

        cout << "\n========== TEST REPORT ==========\n";

        for (const auto& r : results) {
            cout << (r.passed ? "[PASS] " : "[FAIL] ") << r.name;
            if (!r.passed) {
                cout << " -> " << r.message;
            }
            cout << endl;
            if (r.passed) {
                passed++;
            }
        }

        cout << "\nSummary: "
             << passed << "/"
             << results.size()
             << " tests passed\n";

        cout << "================================\n";
    }
};

#define EXPECT_TRUE(cond) \
    if (!(cond)) throw runtime_error("Condition failed: " #cond);

#define EXPECT_FALSE(cond) \
    if ((cond)) throw runtime_error("Condition failed (expected false): " #cond);

#define EXPECT_EQ(a, b) \
    if (!((a) == (b))) throw runtime_error("Expected equality failed");

#define EXPECT_NEAR(a, b, eps) \
    if (fabs((a) - (b)) > (eps)) throw runtime_error("Expected near-equality failed");

#define EXPECT_THROW(stmt) \
    { bool thrown = false; \
      try { stmt; } catch (...) { thrown = true; } \
      if (!thrown) throw runtime_error("Expected exception not thrown"); }

namespace {

string formatTimestamp(time_t ts) {
    tm localTm{};

#if defined(_WIN32)
    localtime_s(&localTm, &ts);
#else
    localtime_r(&ts, &localTm);
#endif

    ostringstream oss;
    oss << put_time(&localTm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

string buildingRef(const shared_ptr<Building>& building) {
    if (!building) {
        return "-";
    }

    return to_string(building->getId()) + ":" + building->getName();
}

void printSection(const string& title) {
    cout << "\n============================================================\n";
    cout << title << "\n";
    cout << "============================================================\n";
}

template <typename K, typename V>
void printDistribution(const unordered_map<K, V>& data, const string& title) {
    vector<pair<K, V>> items(data.begin(), data.end());

    sort(items.begin(), items.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

    cout << title << "\n";
    if (items.empty()) {
        cout << "  (empty)\n";
        return;
    }

    for (const auto& [key, value] : items) {
        cout << "  " << key << ": " << value << "\n";
    }
}

void printCitizens(const vector<shared_ptr<Citizen>>& citizens, const string& title) {
    vector<shared_ptr<Citizen>> sorted = citizens;

    sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) {
            return a->getId() < b->getId();
        });

    cout << title << "\n";
    if (sorted.empty()) {
        cout << "  (none)\n";
        return;
    }

    for (const auto& c : sorted) {
        cout << "  Citizen #" << c->getId()
             << " | " << c->getName()
             << " | age=" << c->getAge()
             << " | profession=" << (c->getProfession().empty() ? "(empty)" : c->getProfession())
             << " | home=" << buildingRef(c->getHome())
             << " | workplace=" << buildingRef(c->getWorkplace())
             << " | location=" << buildingRef(c->getLocation())
             << "\n";
    }
}

void printBuildings(const vector<shared_ptr<Building>>& buildings, const string& title) {
    vector<shared_ptr<Building>> sorted = buildings;

    sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) {
            if (a->getType() == b->getType()) {
                return a->getName() < b->getName();
            }
            return a->getType() < b->getType();
        });

    cout << title << "\n";
    if (sorted.empty()) {
        cout << "  (none)\n";
        return;
    }

    for (const auto& b : sorted) {
        double occupancy = 0.0;
        if (b->getCapacity() > 0) {
            occupancy = static_cast<double>(b->getOccupantCount()) / b->getCapacity();
        }

        cout << "  Building #" << b->getId()
             << " | " << b->getType()
             << " | " << b->getName()
             << " | cap=" << b->getCapacity()
             << " | current occupants=" << b->getOccupantCount()
             << " | occupancy=" << fixed << setprecision(2) << (occupancy * 100.0) << "%\n";
    }
}

void printEventLog(const CityManager& city, size_t maxEventsToShow = 50) {
    const auto& events = city.getEventManager().getEvents();

    cout << "Event log (" << events.size() << " total)\n";
    if (events.empty()) {
        cout << "  (no events)\n";
        return;
    }

    size_t shown = min(maxEventsToShow, events.size());
    for (size_t i = 0; i < shown; ++i) {
        const auto& e = events[i];
        cout << "  [" << formatTimestamp(e->getTimestamp()) << "] "
             << e->getType() << " -> "
             << e->getDescription() << "\n";
    }

    if (events.size() > shown) {
        cout << "  ... " << (events.size() - shown) << " more\n";
    }
}

void printCitySnapshot(const CityManager& city, const string& title) {
    printSection(title);

    cout << "Total citizens: " << city.getTotalCitizens() << "\n";
    cout << "Total buildings: " << city.getTotalBuildings() << "\n";
    cout << "Homeless citizens: " << city.getHomelessCount() << "\n";
    cout << "Unemployed citizens: " << city.getUnemployedCount() << "\n";
    cout << fixed << setprecision(2);
    cout << "Average citizen age: " << city.getAverageCitizenAge() << "\n";
    cout << "Employment rate: " << (city.getEmploymentRate() * 100.0) << "%\n";

    printDistribution(city.getProfessionDistribution(), "\nProfession distribution:");
    printDistribution(city.getBuildingTypeDistribution(), "\nBuilding type distribution:");

    auto avgOccupancy = city.getAverageOccupancyByType();
    vector<pair<string, double>> avgOcc(avgOccupancy.begin(), avgOccupancy.end());
    sort(avgOcc.begin(), avgOcc.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    cout << "\nAverage occupancy by building type:\n";
    if (avgOcc.empty()) {
        cout << "  (empty)\n";
    } else {
        for (const auto& [type, rate] : avgOcc) {
            cout << "  " << type << ": " << fixed << setprecision(2) << (rate * 100.0) << "%\n";
        }
    }

    auto allCitizens = city.queryCitizens([](const shared_ptr<Citizen>&) { return true; });
    auto allBuildings = city.queryBuildings([](const shared_ptr<Building>&) { return true; });

    cout << "\nCitizens:\n";
    printCitizens(allCitizens, "");
    cout << "\nBuildings:\n";
    printBuildings(allBuildings, "");

    cout << "\nCurrent event count: " << city.getEventManager().getEventCount() << "\n";
    cout << "\n";
}

void runShowcaseDemo() {
    printSection("SMART CITY MANAGER - SHOWCASE DEMO");

    CityManager city;

    auto ivan = city.createCitizen("Ivan", 25, "Engineer");
    auto maria = city.createCitizen("Maria", 30, "Doctor");
    auto peter = city.createCitizen("Peter", 40, "Teacher");
    auto anna = city.createCitizen("Anna", 22, "Student");
    auto george = city.createCitizen("George", 35, "Engineer");
    auto noJob = city.createCitizen("NoJob", 28, "");

    auto homeA = city.createResidentialBuilding("Block A", 3);
    auto homeB = city.createResidentialBuilding("Block B", 2);
    auto office = city.createCommercialBuilding("Central Office", 3);
    auto industrial = city.createIndustrialBuilding("Industrial Park", 2);
    auto service = city.createServiceBuilding("City Hospital", 2);

    city.assignHome(ivan->getId(), homeA->getId());
    city.assignHome(anna->getId(), homeA->getId());
    city.assignHome(george->getId(), homeB->getId());
    city.assignHome(maria->getId(), homeB->getId());

    city.assignWorkplace(ivan->getId(), office->getId());
    city.assignWorkplace(george->getId(), office->getId());
    city.assignWorkplace(maria->getId(), service->getId());
    city.assignWorkplace(peter->getId(), industrial->getId());

    city.moveCitizen(ivan->getId(), office->getId());
    city.moveCitizen(george->getId(), office->getId());
    city.moveCitizen(maria->getId(), service->getId());
    city.moveCitizen(peter->getId(), industrial->getId());
    city.moveCitizen(anna->getId(), homeA->getId());
    city.moveCitizen(noJob->getId(), service->getId());

    printCitySnapshot(city, "CITY SNAPSHOT AFTER CREATION, ASSIGNMENTS, AND MOVEMENT");

    printSection("QUERY EXAMPLES");
    auto engineers = city.findCitizensByProfession("Engineer");
    printCitizens(engineers, "Engineers found by specialized query:");

    auto adults = city.queryCitizens([](const shared_ptr<Citizen>& c) {
        return c->getAge() >= 30;
    });
    printCitizens(adults, "\nCitizens age 30+ found by generic predicate query:");

    auto buildingsWithCapacity = city.findBuildingsWithCapacity();
    printBuildings(buildingsWithCapacity, "\nBuildings with available capacity:");

    auto locationQuery = city.queryBuildings([](const shared_ptr<Building>& b) {
        return b->getOccupantCount() > 0;
    });
    printBuildings(locationQuery, "\nBuildings with current occupants (generic building query):");

    printSection("SORTING EXAMPLES");
    auto sortedCitizens = city.sortCitizens(
        city.queryCitizens([](const shared_ptr<Citizen>&) { return true; }),
        [](const shared_ptr<Citizen>& a, const shared_ptr<Citizen>& b) {
            if (a->getAge() == b->getAge()) {
                return a->getName() < b->getName();
            }
            return a->getAge() < b->getAge();
        }
    );
    printCitizens(sortedCitizens, "Citizens sorted by age/name:");

    auto sortedBuildings = city.sortBuildings(
        city.queryBuildings([](const shared_ptr<Building>&) { return true; }),
        [](const shared_ptr<Building>& a, const shared_ptr<Building>& b) {
            if (a->getCapacity() == b->getCapacity()) {
                return a->getName() < b->getName();
            }
            return a->getCapacity() > b->getCapacity();
        }
    );
    printBuildings(sortedBuildings, "\nBuildings sorted by capacity descending:");

    printSection("EVENT LOG");
    printEventLog(city);

    printSection("PERSISTENCE ROUNDTRIP DEMO");
    const string fileName = "smart_city_showcase_save.txt";

    CitySerializer::saveToFile(city, fileName);

    CityManager loaded;
    CitySerializer::loadFromFile(loaded, fileName);

    cout << "Loaded citizens: " << loaded.getTotalCitizens() << "\n";
    cout << "Loaded buildings: " << loaded.getTotalBuildings() << "\n";
    cout << "Loaded average age: " << fixed << setprecision(2) << loaded.getAverageCitizenAge() << "\n";
    cout << "Loaded employment rate: " << fixed << setprecision(2) << (loaded.getEmploymentRate() * 100.0) << "%\n";

    auto loadedCitizens = loaded.queryCitizens([](const shared_ptr<Citizen>&) { return true; });
    printCitizens(loadedCitizens, "\nLoaded citizens:");

    auto loadedBuildings = loaded.queryBuildings([](const shared_ptr<Building>&) { return true; });
    printBuildings(loadedBuildings, "\nLoaded buildings:");

    auto postLoadCitizen = loaded.createCitizen("AfterLoad", 29, "Analyst");
    cout << "\nPost-load new citizen ID: " << postLoadCitizen->getId() << "\n";

    cout << "\nLoaded city event count (reconstruction may produce audit entries): "
         << loaded.getEventManager().getEventCount() << "\n";

    ::remove(fileName.c_str());

    printSection("SHOWCASE DEMO COMPLETE");
}

} // namespace

int main() {
    ios::sync_with_stdio(false);

    runShowcaseDemo();

    TestRunner runner;

    //--------------------------------------------------
    // 1. EMPTY CITY STATISTICS
    //--------------------------------------------------
    runner.run("Empty City Statistics", []() {
        CityManager city;

        EXPECT_EQ(city.getTotalCitizens(), 0);
        EXPECT_EQ(city.getTotalBuildings(), 0);
        EXPECT_EQ(city.getHomelessCount(), 0);
        EXPECT_EQ(city.getUnemployedCount(), 0);
        EXPECT_NEAR(city.getAverageCitizenAge(), 0.0, 1e-9);
        EXPECT_NEAR(city.getEmploymentRate(), 0.0, 1e-9);
    });

    //--------------------------------------------------
    // 2. VALIDATION RULES
    //--------------------------------------------------
    runner.run("Validation Rules", []() {
        CityManager city;

        EXPECT_THROW(city.createCitizen("", 20, "X"));
        EXPECT_THROW(city.createCitizen("   ", 20, "X"));
        EXPECT_THROW(city.createCitizen("\t\n", 20, "X"));
        EXPECT_THROW(city.createCitizen("Ivan", -1, "X"));
        EXPECT_THROW(city.createCitizen("Ivan", 121, "X"));

        EXPECT_THROW(city.createResidentialBuilding("", 10));
        EXPECT_THROW(city.createCommercialBuilding("   ", 10));
        EXPECT_THROW(city.createIndustrialBuilding("Factory", 0));
        EXPECT_THROW(city.createServiceBuilding("\t", 0));

        EXPECT_EQ(city.getTotalCitizens(), 0);
        EXPECT_EQ(city.getTotalBuildings(), 0);
    });

    //--------------------------------------------------
    // 3. BUILDING CREATION TYPES
    //--------------------------------------------------
    runner.run("Building Creation Types", []() {
        CityManager city;

        auto residential = city.createResidentialBuilding("Block A", 10);
        auto commercial = city.createCommercialBuilding("Mall", 20);
        auto industrial = city.createIndustrialBuilding("Factory", 30);
        auto service = city.createServiceBuilding("Hospital", 40);

        EXPECT_EQ(residential->getType(), "Residential");
        EXPECT_EQ(commercial->getType(), "Commercial");
        EXPECT_EQ(industrial->getType(), "Industrial");
        EXPECT_EQ(service->getType(), "Service");
        EXPECT_EQ(city.getTotalBuildings(), 4);
    });

    //--------------------------------------------------
    // 4. CREATION EVENTS
    //--------------------------------------------------
    runner.run("Creation Events", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createResidentialBuilding("Block", 10);
        city.createCommercialBuilding("Mall", 20);

        EXPECT_EQ(city.getEventManager().getEventCount(), 3);

        const auto& events = city.getEventManager().getEvents();
        EXPECT_EQ(events[0]->getType(), "CitizenCreated");
        EXPECT_EQ(events[1]->getType(), "BuildingCreated");
        EXPECT_EQ(events[2]->getType(), "BuildingCreated");
    });

    //--------------------------------------------------
    // 5. HOME AND WORKPLACE RELATIONSHIPS
    //--------------------------------------------------
    runner.run("Home And Workplace Relationships", []() {
        CityManager city;

        auto citizen = city.createCitizen("Ivan", 25, "Engineer");
        auto home = city.createResidentialBuilding("Block", 10);
        auto office = city.createCommercialBuilding("Office", 10);

        city.assignHome(citizen->getId(), home->getId());
        city.assignWorkplace(citizen->getId(), office->getId());

        EXPECT_TRUE(citizen->getHome() != nullptr);
        EXPECT_TRUE(citizen->getWorkplace() != nullptr);
        EXPECT_EQ(citizen->getHome()->getId(), home->getId());
        EXPECT_EQ(citizen->getWorkplace()->getId(), office->getId());
    });

    //--------------------------------------------------
    // 6. PHYSICAL OCCUPANCY IS CURRENT LOCATION
    //--------------------------------------------------
    runner.run("Physical Occupancy Uses Current Location", []() {
        CityManager city;

        auto citizen = city.createCitizen("Anna", 22, "Student");
        auto home = city.createResidentialBuilding("Block", 10);
        auto office = city.createCommercialBuilding("Office", 10);

        city.assignHome(citizen->getId(), home->getId());
        city.assignWorkplace(citizen->getId(), office->getId());

        EXPECT_EQ(home->getOccupantCount(), 0);
        EXPECT_EQ(office->getOccupantCount(), 0);

        city.moveCitizen(citizen->getId(), home->getId());

        EXPECT_EQ(home->getOccupantCount(), 1);
        EXPECT_EQ(citizen->getLocation()->getId(), home->getId());
    });

    //--------------------------------------------------
    // 7. MOVE AND REASSIGNMENT
    //--------------------------------------------------
    runner.run("Move And Reassignment", []() {
        CityManager city;

        auto citizen = city.createCitizen("Peter", 40, "Teacher");
        auto b1 = city.createCommercialBuilding("Mall A", 10);
        auto b2 = city.createCommercialBuilding("Mall B", 10);

        city.moveCitizen(citizen->getId(), b1->getId());
        EXPECT_EQ(b1->getOccupantCount(), 1);
        EXPECT_EQ(citizen->getLocation()->getId(), b1->getId());

        city.moveCitizen(citizen->getId(), b2->getId());
        EXPECT_EQ(b1->getOccupantCount(), 0);
        EXPECT_EQ(b2->getOccupantCount(), 1);
        EXPECT_EQ(citizen->getLocation()->getId(), b2->getId());
    });

    //--------------------------------------------------
    // 8. DUPLICATE MOVE IS IDEMPOTENT
    //--------------------------------------------------
    runner.run("Duplicate Move Is Idempotent", []() {
        CityManager city;

        auto citizen = city.createCitizen("George", 35, "Engineer");
        auto building = city.createCommercialBuilding("Central Office", 5);

        city.moveCitizen(citizen->getId(), building->getId());
        city.moveCitizen(citizen->getId(), building->getId());

        EXPECT_EQ(building->getOccupantCount(), 1);
        EXPECT_EQ(citizen->getLocation()->getId(), building->getId());
    });

    //--------------------------------------------------
    // 9. QUERY ENGINE
    //--------------------------------------------------
    runner.run("Query Engine", []() {
        CityManager city;

        city.createCitizen("A", 20, "Engineer");
        city.createCitizen("B", 30, "Doctor");
        city.createCitizen("C", 40, "Engineer");
        city.createCitizen("D", 50, "");

        auto engineers = city.findCitizensByProfession("Engineer");
        EXPECT_EQ(engineers.size(), 2);

        auto age30Plus = city.queryCitizens([](const shared_ptr<Citizen>& c) {
            return c->getAge() >= 30;
        });
        EXPECT_EQ(age30Plus.size(), 3);

        auto noProfession = city.queryCitizens([](const shared_ptr<Citizen>& c) {
            return c->getProfession().empty();
        });
        EXPECT_EQ(noProfession.size(), 1);

        auto allBuildings = city.queryBuildings([](const shared_ptr<Building>&) {
            return true;
        });
        EXPECT_TRUE(allBuildings.empty());
    });

    //--------------------------------------------------
    // 10. SORTING LAYER
    //--------------------------------------------------
    runner.run("Sorting Layer", []() {
        CityManager city;

        city.createCitizen("Z", 45, "X");
        city.createCitizen("A", 20, "X");
        city.createCitizen("M", 30, "X");

        auto people = city.queryCitizens([](const shared_ptr<Citizen>&) { return true; });

        auto byAge = city.sortCitizens(
            people,
            [](const shared_ptr<Citizen>& a, const shared_ptr<Citizen>& b) {
                return a->getAge() < b->getAge();
            }
        );

        EXPECT_EQ(byAge[0]->getAge(), 20);
        EXPECT_EQ(byAge[1]->getAge(), 30);
        EXPECT_EQ(byAge[2]->getAge(), 45);

        auto allBuildings = city.queryBuildings([](const shared_ptr<Building>&) { return true; });
        EXPECT_TRUE(allBuildings.empty());
    });

    //--------------------------------------------------
    // 11. STATISTICS LAYER
    //--------------------------------------------------
    runner.run("Statistics Layer", []() {
        CityManager city;

        auto c1 = city.createCitizen("A", 20, "Engineer");
        auto c2 = city.createCitizen("B", 40, "Engineer");
        auto c3 = city.createCitizen("C", 30, "Teacher");

        auto home = city.createResidentialBuilding("Block", 3);
        auto office = city.createCommercialBuilding("Office", 4);

        city.assignHome(c1->getId(), home->getId());
        city.assignHome(c2->getId(), home->getId());
        city.assignWorkplace(c1->getId(), office->getId());
        city.assignWorkplace(c3->getId(), office->getId());

        city.moveCitizen(c1->getId(), home->getId());
        city.moveCitizen(c2->getId(), home->getId());
        city.moveCitizen(c3->getId(), office->getId());

        EXPECT_NEAR(city.getAverageCitizenAge(), 30.0, 1e-9);
        EXPECT_NEAR(city.getEmploymentRate(), 2.0 / 3.0, 1e-9);

        auto professions = city.getProfessionDistribution();
        EXPECT_EQ(professions["Engineer"], 2);
        EXPECT_EQ(professions["Teacher"], 1);

        auto buildingTypes = city.getBuildingTypeDistribution();
        EXPECT_EQ(buildingTypes["Residential"], 1);
        EXPECT_EQ(buildingTypes["Commercial"], 1);

        auto avgOcc = city.getAverageOccupancyByType();
        EXPECT_TRUE(avgOcc["Residential"] > 0.0);
        EXPECT_TRUE(avgOcc["Commercial"] > 0.0);
    });

    //--------------------------------------------------
    // 12. EVENT SYSTEM AND FAILED OPS
    //--------------------------------------------------
    runner.run("Event System And Failed Operations", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createResidentialBuilding("Block", 10);

        size_t before = city.getEventManager().getEventCount();

        EXPECT_THROW(city.assignHome(999, 999));

        EXPECT_EQ(city.getEventManager().getEventCount(), before);

        auto c = city.createCitizen("Maria", 30, "Doctor");
        auto b = city.createCommercialBuilding("Office", 10);
        city.assignWorkplace(c->getId(), b->getId());

        EXPECT_TRUE(city.getEventManager().getEventCount() > before);
    });

    //--------------------------------------------------
    // 13. PERSISTENCE ROUNDTRIP
    //--------------------------------------------------
    runner.run("Persistence Roundtrip", []() {
        const string fileName = "city_roundtrip_test.txt";

        {
            CityManager city;

            auto c1 = city.createCitizen("Ivan", 25, "Engineer");
            auto c2 = city.createCitizen("Maria", 30, "Doctor");

            auto home = city.createResidentialBuilding("HomeA", 10);
            auto office = city.createCommercialBuilding("OfficeA", 10);
            auto hospital = city.createServiceBuilding("Hospital", 10);

            city.assignHome(c1->getId(), home->getId());
            city.assignWorkplace(c1->getId(), office->getId());
            city.moveCitizen(c1->getId(), office->getId());

            city.assignWorkplace(c2->getId(), hospital->getId());
            city.moveCitizen(c2->getId(), hospital->getId());

            CitySerializer::saveToFile(city, fileName);
        }

        CityManager loaded;
        CitySerializer::loadFromFile(loaded, fileName);

        EXPECT_EQ(loaded.getTotalCitizens(), 2);
        EXPECT_EQ(loaded.getTotalBuildings(), 3);

        auto loadedIvan = loaded.getCitizens().at(1);
        EXPECT_TRUE(loadedIvan->getHome() != nullptr);
        EXPECT_TRUE(loadedIvan->getWorkplace() != nullptr);
        EXPECT_TRUE(loadedIvan->getLocation() != nullptr);
        EXPECT_EQ(loadedIvan->getHome()->getName(), "HomeA");
        EXPECT_EQ(loadedIvan->getWorkplace()->getName(), "OfficeA");
        EXPECT_EQ(loadedIvan->getLocation()->getName(), "OfficeA");

        auto next = loaded.createCitizen("AfterLoad", 29, "Analyst");
        EXPECT_EQ(next->getId(), 3);

        ::remove(fileName.c_str());
    });

    //--------------------------------------------------
    // 14. REMOVAL CLEANUP
    //--------------------------------------------------
    runner.run("Removal Cleanup", []() {
        CityManager city;

        auto citizen = city.createCitizen("Ivan", 20, "X");
        auto home = city.createResidentialBuilding("Block", 10);
        auto office = city.createCommercialBuilding("Office", 10);

        city.assignHome(citizen->getId(), home->getId());
        city.assignWorkplace(citizen->getId(), office->getId());
        city.moveCitizen(citizen->getId(), office->getId());

        EXPECT_EQ(office->getOccupantCount(), 1);

        city.removeCitizen(citizen->getId());
        EXPECT_EQ(city.getTotalCitizens(), 0);
        EXPECT_EQ(office->getOccupantCount(), 0);

        auto c2 = city.createCitizen("Anna", 30, "Y");
        auto building = city.createCommercialBuilding("Mall", 20);
        city.moveCitizen(c2->getId(), building->getId());
        city.assignWorkplace(c2->getId(), building->getId()); // allowed as workplace, occupation remains current location based

        EXPECT_TRUE(c2->getLocation() != nullptr);

        city.removeBuilding(building->getId());
        EXPECT_TRUE(c2->getLocation() == nullptr);
        EXPECT_TRUE(c2->getWorkplace() == nullptr);
    });

    //--------------------------------------------------
    // 15. MISSING FILE LOAD
    //--------------------------------------------------
    runner.run("Missing File Load", []() {
        CityManager city;
        EXPECT_THROW(CitySerializer::loadFromFile(city, "__definitely_missing_city_file__.txt"));
    });

    //--------------------------------------------------
    // 16. STRESS TEST
    //--------------------------------------------------
    runner.run("Stress Test", []() {
        CityManager city;

        auto home = city.createResidentialBuilding("MassHome", 5000);
        auto office = city.createCommercialBuilding("MassOffice", 5000);
        auto hospital = city.createServiceBuilding("MassHospital", 5000);

        for (int i = 0; i < 2000; ++i) {
            auto c = city.createCitizen("Citizen" + to_string(i), 18 + (i % 60), (i % 2 == 0) ? "Engineer" : "Teacher");

            if (i % 3 == 0) {
                city.assignHome(c->getId(), home->getId());
                city.moveCitizen(c->getId(), home->getId());
            }
            else if (i % 3 == 1) {
                city.assignWorkplace(c->getId(), office->getId());
                city.moveCitizen(c->getId(), office->getId());
            }
            else {
                city.assignWorkplace(c->getId(), hospital->getId());
                city.moveCitizen(c->getId(), hospital->getId());
            }
        }

        EXPECT_EQ(city.getTotalCitizens(), 2000);
        EXPECT_TRUE(city.getEventManager().getEventCount() >= 2003);

        auto engineers = city.findCitizensByProfession("Engineer");
        EXPECT_TRUE(!engineers.empty());

        auto all = city.queryCitizens([](const shared_ptr<Citizen>&) { return true; });
        auto sorted = city.sortCitizens(
            all,
            [](const shared_ptr<Citizen>& a, const shared_ptr<Citizen>& b) {
                return a->getAge() < b->getAge();
            }
        );

        EXPECT_EQ(sorted.size(), 2000);
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}