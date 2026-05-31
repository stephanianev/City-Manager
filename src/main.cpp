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
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <fstream>

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
             << results.size() << " tests passed\n";

        cout << "================================\n";
    }
};

#define EXPECT_TRUE(cond) \
    if (!(cond)) throw runtime_error("Condition failed: " #cond);

#define EXPECT_FALSE(cond) \
    if ((cond)) throw runtime_error("Condition failed (expected false): " #cond);

#define EXPECT_EQ(a, b) \
    if (!((a) == (b))) { \
        ostringstream _oss; \
        _oss << "Expected " << (a) << " == " << (b); \
        throw runtime_error(_oss.str()); \
    }

#define EXPECT_NEAR(a, b, eps) \
    if (fabs((a) - (b)) > (eps)) { \
        ostringstream _oss; \
        _oss << "Expected near-equality failed: " << (a) << " vs " << (b) << " (epsilon=" << (eps) << ")"; \
        throw runtime_error(_oss.str()); \
    }

#define EXPECT_THROW(stmt) \
    { bool thrown = false; \
      try { stmt; } catch (...) { thrown = true; } \
      if (!thrown) throw runtime_error("Expected exception not thrown"); }

template <typename T, typename = void>
struct has_findCitizensByName : false_type {};

template <typename T>
struct has_findCitizensByName<T, void_t<
    decltype(declval<const T&>().findCitizensByName(declval<const string&>()))
>> : true_type {};

template <typename T, typename = void>
struct has_findBuildingsByName : false_type {};

template <typename T>
struct has_findBuildingsByName<T, void_t<
    decltype(declval<const T&>().findBuildingsByName(declval<const string&>()))
>> : true_type {};

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

void printSection(const string& title) {
    cout << "\n============================================================\n";
    cout << title << "\n";
    cout << "============================================================\n";
}

string buildingRef(const shared_ptr<Building>& b) {
    if (!b) return "-";
    return to_string(b->getId()) + ":" + b->getName();
}

string citizenLine(const shared_ptr<Citizen>& c) {
    ostringstream oss;
    oss << "  Citizen #" << c->getId()
        << " | " << c->getName()
        << " | age=" << c->getAge()
        << " | profession=" << (c->getProfession().empty() ? "(empty)" : c->getProfession())
        << " | home=" << buildingRef(c->getHome())
        << " | workplace=" << buildingRef(c->getWorkplace())
        << " | location=" << buildingRef(c->getLocation());
    return oss.str();
}

string buildingLine(const shared_ptr<Building>& b) {
    double occ = 0.0;
    if (b->getCapacity() > 0) {
        occ = static_cast<double>(b->getOccupantCount()) / b->getCapacity();
    }

    ostringstream oss;
    oss << "  Building #" << b->getId()
        << " | " << b->getType()
        << " | " << b->getName()
        << " | cap=" << b->getCapacity()
        << " | occupants=" << b->getOccupantCount()
        << " | occupancy=" << fixed << setprecision(2) << (occ * 100.0) << "%";
    return oss.str();
}

template <typename K, typename V>
void printDistribution(const unordered_map<K, V>& data, const string& label) {
    cout << label << "\n";
    if (data.empty()) {
        cout << "  (empty)\n";
        return;
    }

    vector<pair<K, V>> items(data.begin(), data.end());
    sort(items.begin(), items.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& [k, v] : items) {
        if (k == "") {
            cout << "  Jobless: " << v << "\n";
        } else {
            cout << "  " << k << ": " << v << "\n";
        }
    }
}

void printEventLog(const CityManager& city, size_t maxEventsToShow = 40) {
    const auto& events = city.getEventManager().getEvents();

    cout << "Event log (" << events.size() << " total)\n";
    if (events.empty()) {
        cout << "  (no events)\n";
        return;
    }

    size_t shown = min(maxEventsToShow, events.size());
    for (size_t i = 0; i < shown; ++i) {
        cout << "  [" << formatTimestamp(events[i]->getTimestamp()) << "] "
             << events[i]->getType() << " -> "
             << events[i]->getDescription() << "\n";
    }

    if (events.size() > shown) {
        cout << "  ... " << (events.size() - shown) << " more\n";
    }
}

double occupancyOf(
    const vector<pair<string, double>>& report,
    const string& buildingName
) {
    for (const auto& [name, value] : report) {
        if (name == buildingName) {
            return value;
        }
    }
    return -1.0;
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

    auto avgOcc = city.getAverageOccupancyByType();
    cout << "\nAverage occupancy by building type:\n";
    if (avgOcc.empty()) {
        cout << "  (empty)\n";
    } else {
        vector<pair<string, double>> items(avgOcc.begin(), avgOcc.end());
        sort(items.begin(), items.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });

        for (const auto& [type, rate] : items) {
            cout << "  " << type << ": " << fixed << setprecision(2) << (rate * 100.0) << "%\n";
        }
    }

    auto allCitizens = city.queryCitizens([](const shared_ptr<Citizen>&) { return true; });
    auto allBuildings = city.queryBuildings([](const shared_ptr<Building>&) { return true; });

    cout << "\nCitizens:\n";
    if (allCitizens.empty()) {
        cout << "  (none)\n";
    } else {
        sort(allCitizens.begin(), allCitizens.end(),
            [](const auto& a, const auto& b) { return a->getId() < b->getId(); });

        for (const auto& c : allCitizens) {
            cout << citizenLine(c) << "\n";
        }
    }

    cout << "\nBuildings:\n";
    if (allBuildings.empty()) {
        cout << "  (none)\n";
    } else {
        sort(allBuildings.begin(), allBuildings.end(),
            [](const auto& a, const auto& b) {
                if (a->getType() == b->getType()) return a->getName() < b->getName();
                return a->getType() < b->getType();
            });

        for (const auto& b : allBuildings) {
            cout << buildingLine(b) << "\n";
        }
    }

    cout << "\nEvent count: " << city.getEventManager().getEventCount() << "\n";
}

void runShowcaseDemo() {
    printSection("SMART CITY MANAGER SHOWCASE");

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

    printCitySnapshot(city, "STATE AFTER CREATION, ASSIGNMENT, AND MOVEMENT");
    printSection("EVENT LOG");
    printEventLog(city);

    printSection("QUERY EXAMPLES");
    auto engineers = city.findCitizensByProfession("Engineer");
    sort(engineers.begin(), engineers.end(),
        [](const auto& a, const auto& b) { return a->getId() < b->getId(); });

    cout << "Engineers:\n";
    for (const auto& c : engineers) {
        cout << citizenLine(c) << "\n";
    }

    auto adults = city.queryCitizens([](const shared_ptr<Citizen>& c) {
        return c->getAge() >= 30;
    });
    sort(adults.begin(), adults.end(),
        [](const auto& a, const auto& b) { return a->getAge() < b->getAge(); });

    cout << "\nCitizens age 30+:\n";
    for (const auto& c : adults) {
        cout << citizenLine(c) << "\n";
    }

    auto activeBuildings = city.queryBuildings([](const shared_ptr<Building>& b) {
        return b->getOccupantCount() > 0;
    });
    cout << "\nBuildings with occupants:\n";
    for (const auto& b : activeBuildings) {
        cout << buildingLine(b) << "\n";
    }

    if constexpr (has_findCitizensByName<CityManager>::value) {
        printSection("OPTIONAL NAME-LOOKUP DEMO");
        auto ivans = city.findCitizensByName("Ivan");
        cout << "Citizens named Ivan: " << ivans.size() << "\n";
        for (const auto& c : ivans) {
            cout << citizenLine(c) << "\n";
        }
    }

    if constexpr (has_findBuildingsByName<CityManager>::value) {
        auto matches = city.findBuildingsByName("Block A");
        cout << "\nBuildings named Block A: " << matches.size() << "\n";
        for (const auto& b : matches) {
            cout << buildingLine(b) << "\n";
        }
    }

    printSection("SORTING EXAMPLES");
    auto allCitizens = city.queryCitizens([](const shared_ptr<Citizen>&) { return true; });
    auto sortedCitizens = city.sortCitizens(
        allCitizens,
        [](const shared_ptr<Citizen>& a, const shared_ptr<Citizen>& b) {
            if (a->getAge() == b->getAge()) return a->getName() < b->getName();
            return a->getAge() < b->getAge();
        }
    );

    cout << "Citizens sorted by age/name:\n";
    for (const auto& c : sortedCitizens) {
        cout << citizenLine(c) << "\n";
    }

    auto allBuildings = city.queryBuildings([](const shared_ptr<Building>&) { return true; });
    auto sortedBuildings = city.sortBuildings(
        allBuildings,
        [](const shared_ptr<Building>& a, const shared_ptr<Building>& b) {
            if (a->getCapacity() == b->getCapacity()) return a->getName() < b->getName();
            return a->getCapacity() > b->getCapacity();
        }
    );

    cout << "\nBuildings sorted by capacity descending:\n";
    for (const auto& b : sortedBuildings) {
        cout << buildingLine(b) << "\n";
    }

    printSection("PERSISTENCE ROUNDTRIP");
    const string fileName = "smart_city_showcase_save.txt";
    CitySerializer::saveToFile(city, fileName);

    CityManager loaded;
    CitySerializer::loadFromFile(loaded, fileName);

    cout << "Loaded citizens: " << loaded.getTotalCitizens() << "\n";
    cout << "Loaded buildings: " << loaded.getTotalBuildings() << "\n";
    cout << fixed << setprecision(2);
    cout << "Loaded average age: " << loaded.getAverageCitizenAge() << "\n";
    cout << "Loaded employment rate: " << (loaded.getEmploymentRate() * 100.0) << "%\n";

    auto loadedCitizens = loaded.queryCitizens([](const shared_ptr<Citizen>&) { return true; });
    auto loadedBuildings = loaded.queryBuildings([](const shared_ptr<Building>&) { return true; });

    cout << "\nLoaded citizens:\n";
    for (const auto& c : loadedCitizens) {
        cout << citizenLine(c) << "\n";
    }

    cout << "\nLoaded buildings:\n";
    for (const auto& b : loadedBuildings) {
        cout << buildingLine(b) << "\n";
    }

    auto postLoadCitizen = loaded.createCitizen("AfterLoad", 29, "Analyst");
    cout << "\nPost-load new citizen ID: " << postLoadCitizen->getId() << "\n";

    ::remove(fileName.c_str());

    printSection("SHOWCASE COMPLETE");
}

} // namespace

int main() {
    ios::sync_with_stdio(false);

    try {
    runShowcaseDemo();

    TestRunner runner;

    //--------------------------------------------------
    // 1. EMPTY CITY INVARIANTS
    //--------------------------------------------------
    runner.run("Empty City Invariants", []() {
        CityManager city;

        EXPECT_EQ(city.getTotalCitizens(), 0);
        EXPECT_EQ(city.getTotalBuildings(), 0);
        EXPECT_EQ(city.getHomelessCount(), 0);
        EXPECT_EQ(city.getUnemployedCount(), 0);
        EXPECT_NEAR(city.getAverageCitizenAge(), 0.0, 1e-9);
        EXPECT_NEAR(city.getEmploymentRate(), 0.0, 1e-9);
        EXPECT_TRUE(city.getEventManager().getEventCount() == 0);
    });

    //--------------------------------------------------
    // 2. VALIDATION AND ID CONTINUITY
    //--------------------------------------------------
    runner.run("Validation And ID Continuity", []() {
        CityManager city;

        auto c1 = city.createCitizen("Ivan", 25, "Engineer");
        EXPECT_EQ(c1->getId(), 1);

        EXPECT_THROW(city.createCitizen("", 20, "X"));
        EXPECT_THROW(city.createCitizen("   ", 20, "X"));
        EXPECT_THROW(city.createCitizen("\t\n", 20, "X"));
        EXPECT_THROW(city.createCitizen("Ivan", -1, "X"));
        EXPECT_THROW(city.createCitizen("Ivan", 121, "X"));

        auto c2 = city.createCitizen("Maria", 30, "Doctor");
        EXPECT_EQ(c2->getId(), 2);

        EXPECT_THROW(city.createResidentialBuilding("", 10));
        EXPECT_THROW(city.createCommercialBuilding("   ", 10));
        EXPECT_THROW(city.createIndustrialBuilding("Factory", 0));
        EXPECT_THROW(city.createServiceBuilding("\t", 0));

        auto b1 = city.createResidentialBuilding("Block", 10);
        EXPECT_EQ(b1->getId(), 1);
        EXPECT_EQ(city.getTotalCitizens(), 2);
        EXPECT_EQ(city.getTotalBuildings(), 1);
    });

    //--------------------------------------------------
    // 3. BUILDING TYPES
    //--------------------------------------------------
    runner.run("Building Types", []() {
        CityManager city;

        auto r = city.createResidentialBuilding("R1", 10);
        auto c = city.createCommercialBuilding("C1", 20);
        auto i = city.createIndustrialBuilding("I1", 30);
        auto s = city.createServiceBuilding("S1", 40);

        EXPECT_EQ(r->getType(), "Residential");
        EXPECT_EQ(c->getType(), "Commercial");
        EXPECT_EQ(i->getType(), "Industrial");
        EXPECT_EQ(s->getType(), "Service");

        auto report = city.getBuildingTypeDistribution();
        EXPECT_EQ(report["Residential"], 1);
        EXPECT_EQ(report["Commercial"], 1);
        EXPECT_EQ(report["Industrial"], 1);
        EXPECT_EQ(report["Service"], 1);
    });

    //--------------------------------------------------
    // 4. HOME AND WORKPLACE ARE NOT PHYSICAL OCCUPANCY
    //--------------------------------------------------
    runner.run("Home And Workplace Are Not Physical Occupancy", []() {
        CityManager city;

        auto citizen = city.createCitizen("Anna", 22, "Student");
        auto home = city.createResidentialBuilding("Block", 10);
        auto office = city.createCommercialBuilding("Office", 10);

        city.assignHome(citizen->getId(), home->getId());
        city.assignWorkplace(citizen->getId(), office->getId());

        EXPECT_TRUE(citizen->getHome() != nullptr);
        EXPECT_TRUE(citizen->getWorkplace() != nullptr);
        EXPECT_EQ(citizen->getHome()->getId(), home->getId());
        EXPECT_EQ(citizen->getWorkplace()->getId(), office->getId());

        EXPECT_EQ(home->getOccupantCount(), 0);
        EXPECT_EQ(office->getOccupantCount(), 0);

        auto occupantsHome = city.listCitizensInBuilding(home->getId());
        auto occupantsOffice = city.listCitizensInBuilding(office->getId());
        EXPECT_TRUE(occupantsHome.empty());
        EXPECT_TRUE(occupantsOffice.empty());

        auto available = city.findBuildingsWithCapacity();
        EXPECT_EQ(available.size(), 2);
    });

    //--------------------------------------------------
    // 5. PHYSICAL OCCUPANCY COMES FROM MOVEMENT
    //--------------------------------------------------
    runner.run("Physical Occupancy Comes From Movement", []() {
        CityManager city;

        auto citizen = city.createCitizen("Ivan", 25, "Engineer");
        auto home = city.createResidentialBuilding("Block", 10);
        auto office = city.createCommercialBuilding("Office", 10);

        city.assignHome(citizen->getId(), home->getId());
        city.assignWorkplace(citizen->getId(), office->getId());

        EXPECT_EQ(home->getOccupantCount(), 0);
        EXPECT_EQ(office->getOccupantCount(), 0);
        EXPECT_TRUE(citizen->getLocation() == nullptr);

        city.moveCitizen(citizen->getId(), office->getId());

        EXPECT_TRUE(citizen->getLocation() != nullptr);
        EXPECT_EQ(citizen->getLocation()->getId(), office->getId());
        EXPECT_EQ(office->getOccupantCount(), 1);

        auto listed = city.listCitizensInBuilding(office->getId());
        EXPECT_EQ(listed.size(), 1);
        EXPECT_EQ(listed[0]->getId(), citizen->getId());

        auto report = city.getBuildingOccupancyReport();
        EXPECT_NEAR(occupancyOf(report, "Office"), 0.1, 1e-9);
    });

    //--------------------------------------------------
    // 6. MOVEMENT REASSIGNMENT AND IDEMPOTENCY
    //--------------------------------------------------
    runner.run("Movement Reassignment And Idempotency", []() {
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

        city.moveCitizen(citizen->getId(), b2->getId());
        EXPECT_EQ(b2->getOccupantCount(), 1);
    });

    //--------------------------------------------------
    // 7. CAPACITY ENFORCEMENT
    //--------------------------------------------------
    runner.run("Capacity Enforcement", []() {
        CityManager city;

        auto small = city.createCommercialBuilding("Tiny", 1);
        auto c1 = city.createCitizen("A", 20, "X");
        auto c2 = city.createCitizen("B", 20, "Y");

        city.moveCitizen(c1->getId(), small->getId());
        EXPECT_THROW(city.moveCitizen(c2->getId(), small->getId()));
        EXPECT_EQ(small->getOccupantCount(), 1);
    });

    //--------------------------------------------------
    // 8. REMOVAL CLEANUP
    //--------------------------------------------------
    runner.run("Removal Cleanup", []() {
        CityManager city;

        auto citizen = city.createCitizen("Ivan", 20, "X");
        auto home = city.createResidentialBuilding("Home", 10);
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
        EXPECT_TRUE(c2->getLocation() != nullptr);

        city.removeBuilding(building->getId());
        EXPECT_TRUE(c2->getLocation() == nullptr);
        EXPECT_TRUE(c2->getWorkplace() == nullptr);
        EXPECT_TRUE(c2->getHome() == nullptr);
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

        auto ageUnder30 = city.queryCitizens([](const shared_ptr<Citizen>& c) {
            return c->getAge() < 30 && c->getProfession() == "Engineer";
        });
        EXPECT_EQ(ageUnder30.size(), 1);
        EXPECT_EQ(ageUnder30[0]->getName(), "A");
    });

    //--------------------------------------------------
    // 10. OPTIONAL NAME LOOKUP API
    //--------------------------------------------------
    runner.run("Optional Name Lookup API", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createCitizen("Ivan", 35, "Doctor");
        city.createResidentialBuilding("Block A", 10);
        city.createResidentialBuilding("Block A", 20);

        if constexpr (has_findCitizensByName<CityManager>::value) {
            auto result = city.findCitizensByName("Ivan");
            EXPECT_EQ(result.size(), 2);
            EXPECT_EQ(result[0]->getName(), "Ivan");
            EXPECT_EQ(result[1]->getName(), "Ivan");
        } else {
            auto result = city.queryCitizens([](const shared_ptr<Citizen>& c) {
                return c->getName() == "Ivan";
            });
            EXPECT_EQ(result.size(), 2);
        }

        if constexpr (has_findBuildingsByName<CityManager>::value) {
            auto result = city.findBuildingsByName("Block A");
            EXPECT_EQ(result.size(), 2);
            EXPECT_EQ(result[0]->getName(), "Block A");
            EXPECT_EQ(result[1]->getName(), "Block A");
        } else {
            auto result = city.queryBuildings([](const shared_ptr<Building>& b) {
                return b->getName() == "Block A";
            });
            EXPECT_EQ(result.size(), 2);
        }
    });

    //--------------------------------------------------
    // 11. SORTING LAYER
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
                if (a->getAge() == b->getAge()) {
                    return a->getName() < b->getName();
                }
                return a->getAge() < b->getAge();
            }
        );

        EXPECT_EQ(byAge.size(), 3);
        EXPECT_EQ(byAge[0]->getAge(), 20);
        EXPECT_EQ(byAge[1]->getAge(), 30);
        EXPECT_EQ(byAge[2]->getAge(), 45);

        city.createResidentialBuilding("Block Z", 5);
        city.createResidentialBuilding("Block A", 10);
        city.createCommercialBuilding("Mall", 2);

        auto buildings = city.queryBuildings([](const shared_ptr<Building>&) { return true; });
        auto byCapacity = city.sortBuildings(
            buildings,
            [](const shared_ptr<Building>& a, const shared_ptr<Building>& b) {
                if (a->getCapacity() == b->getCapacity()) {
                    return a->getName() < b->getName();
                }
                return a->getCapacity() > b->getCapacity();
            }
        );

        EXPECT_EQ(byCapacity.size(), 3);
        EXPECT_EQ(byCapacity[0]->getCapacity(), 10);
        EXPECT_EQ(byCapacity[1]->getCapacity(), 5);
        EXPECT_EQ(byCapacity[2]->getCapacity(), 2);
    });

    //--------------------------------------------------
    // 12. STATISTICS AND REPORTING
    //--------------------------------------------------
    runner.run("Statistics And Reporting", []() {
        CityManager city;

        auto c1 = city.createCitizen("A", 20, "Engineer");
        auto c2 = city.createCitizen("B", 40, "Engineer");
        auto c3 = city.createCitizen("C", 30, "Teacher");

        auto home = city.createResidentialBuilding("Block", 4);
        auto office = city.createCommercialBuilding("Office", 5);

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
        EXPECT_NEAR(avgOcc["Residential"], 0.5, 1e-9);
        EXPECT_NEAR(avgOcc["Commercial"], 0.2, 1e-9);

        auto occupancyReport = city.getBuildingOccupancyReport();
        EXPECT_TRUE(!occupancyReport.empty());
        EXPECT_NEAR(occupancyOf(occupancyReport, "Block"), 0.5, 1e-9);
        EXPECT_NEAR(occupancyOf(occupancyReport, "Office"), 0.2, 1e-9);
    });

    //--------------------------------------------------
    // 13. EVENT SYSTEM
    //--------------------------------------------------
    runner.run("Event System", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createResidentialBuilding("Block", 10);

        EXPECT_EQ(city.getEventManager().getEventCount(), 2);

        const auto& events = city.getEventManager().getEvents();
        EXPECT_EQ(events[0]->getType(), "CitizenCreated");
        EXPECT_EQ(events[1]->getType(), "BuildingCreated");

        size_t before = city.getEventManager().getEventCount();
        EXPECT_THROW(city.assignHome(999, 999));
        EXPECT_EQ(city.getEventManager().getEventCount(), before);

        auto c = city.createCitizen("Maria", 30, "Doctor");
        auto b = city.createCommercialBuilding("Office", 10);
        city.assignWorkplace(c->getId(), b->getId());
        EXPECT_TRUE(city.getEventManager().getEventCount() > before);
    });

    //--------------------------------------------------
    // 14. PERSISTENCE ROUNDTRIP
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
    // 15. REPEATED PERSISTENCE ROUNDTRIP
    //--------------------------------------------------
    runner.run("Repeated Persistence Roundtrip", []() {
        const string file1 = "city_roundtrip_1.txt";
        const string file2 = "city_roundtrip_2.txt";

        {
            CityManager city;
            auto c1 = city.createCitizen("A", 20, "Engineer");
            auto b1 = city.createResidentialBuilding("Block A", 10);
            auto b2 = city.createCommercialBuilding("Office A", 10);
            city.assignHome(c1->getId(), b1->getId());
            city.moveCitizen(c1->getId(), b2->getId());

            CitySerializer::saveToFile(city, file1);
        }

        CityManager a;
        CitySerializer::loadFromFile(a, file1);
        CitySerializer::saveToFile(a, file2);

        CityManager b;
        CitySerializer::loadFromFile(b, file2);

        EXPECT_EQ(a.getTotalCitizens(), b.getTotalCitizens());
        EXPECT_EQ(a.getTotalBuildings(), b.getTotalBuildings());
        EXPECT_NEAR(a.getAverageCitizenAge(), b.getAverageCitizenAge(), 1e-9);
        EXPECT_NEAR(a.getEmploymentRate(), b.getEmploymentRate(), 1e-9);

        ::remove(file1.c_str());
        ::remove(file2.c_str());
    });

    //--------------------------------------------------
    // 16. MISSING FILE LOAD
    //--------------------------------------------------
    runner.run("Missing File Load", []() {
        CityManager city;
        EXPECT_THROW(CitySerializer::loadFromFile(city, "__definitely_missing_city_file__.txt"));
    });

    //--------------------------------------------------
    // 17. MALFORMED FILE LOAD
    //--------------------------------------------------
    runner.run("Malformed File Load", []() {
        const string badFile = "bad_city_file.txt";

        {
            ofstream out(badFile);
            out << "[BUILDINGS]\n";
            out << "1|Residential|Block A|10\n";
            out << "[CITIZENS]\n";
            out << "1|Ivan|25|Engineer|1|1\n";
            out << "BROKEN_LINE\n";
        }

        CityManager city;
        EXPECT_THROW(CitySerializer::loadFromFile(city, badFile));

        ::remove(badFile.c_str());
    });

    //--------------------------------------------------
    // 18. STATE INTEGRITY AFTER FAILED VALIDATION
    //--------------------------------------------------
    runner.run("State Integrity After Failed Validation", []() {
        CityManager city;

        auto c = city.createCitizen("Valid", 20, "X");
        auto b = city.createResidentialBuilding("ValidBlock", 10);

        size_t citizensBefore = city.getTotalCitizens();
        size_t buildingsBefore = city.getTotalBuildings();
        size_t eventsBefore = city.getEventManager().getEventCount();

        EXPECT_THROW(city.assignHome(999, b->getId()));
        EXPECT_THROW(city.assignWorkplace(c->getId(), 999));
        EXPECT_THROW(city.moveCitizen(999, b->getId()));

        EXPECT_EQ(city.getTotalCitizens(), citizensBefore);
        EXPECT_EQ(city.getTotalBuildings(), buildingsBefore);
        EXPECT_EQ(city.getEventManager().getEventCount(), eventsBefore);
    });

    //--------------------------------------------------
    // 19. OPTIONAL NAME SEARCH AFTER REMOVAL
    //--------------------------------------------------
    runner.run("Name Search Updates After Removal", []() {
        CityManager city;

        auto c1 = city.createCitizen("Ivan", 25, "Engineer");
        auto c2 = city.createCitizen("Ivan", 30, "Doctor");
        auto b1 = city.createResidentialBuilding("Block A", 10);
        auto b2 = city.createResidentialBuilding("Block A", 20);

        if constexpr (has_findCitizensByName<CityManager>::value) {
            EXPECT_EQ(city.findCitizensByName("Ivan").size(), 2);
        } else {
            EXPECT_EQ(city.queryCitizens([](const shared_ptr<Citizen>& c) { return c->getName() == "Ivan"; }).size(), 2);
        }

        if constexpr (has_findBuildingsByName<CityManager>::value) {
            EXPECT_EQ(city.findBuildingsByName("Block A").size(), 2);
        } else {
            EXPECT_EQ(city.queryBuildings([](const shared_ptr<Building>& b) { return b->getName() == "Block A"; }).size(), 2);
        }

        city.removeCitizen(c1->getId());
        city.removeBuilding(b1->getId());

        if constexpr (has_findCitizensByName<CityManager>::value) {
            auto citizens = city.findCitizensByName("Ivan");
            EXPECT_EQ(citizens.size(), 1);
            EXPECT_EQ(citizens[0]->getId(), c2->getId());
        } else {
            auto citizens = city.queryCitizens([](const shared_ptr<Citizen>& c) { return c->getName() == "Ivan"; });
            EXPECT_EQ(citizens.size(), 1);
            EXPECT_EQ(citizens[0]->getId(), c2->getId());
        }

        if constexpr (has_findBuildingsByName<CityManager>::value) {
            auto buildings = city.findBuildingsByName("Block A");
            EXPECT_EQ(buildings.size(), 1);
            EXPECT_EQ(buildings[0]->getId(), b2->getId());
        } else {
            auto buildings = city.queryBuildings([](const shared_ptr<Building>& b) { return b->getName() == "Block A"; });
            EXPECT_EQ(buildings.size(), 1);
            EXPECT_EQ(buildings[0]->getId(), b2->getId());
        }
    });

    //--------------------------------------------------
    // 20. SERIALIZATION HARDENING AGAINST UNSAFE CHARACTERS
    //--------------------------------------------------
    runner.run("Serialization Hardening Against Unsafe Characters", []() {
        CityManager city;

        bool citizenRejected = false;
        bool buildingRejected = false;

        try {
            city.createCitizen("Pipe|Name", 20, "X");
        }
        catch (...) {
            citizenRejected = true;
        }

        try {
            city.createResidentialBuilding("Block|A", 10);
        }
        catch (...) {
            buildingRejected = true;
        }

        if (!citizenRejected || !buildingRejected) {
            const string fileName = "unsafe_chars_roundtrip.txt";

            auto c = citizenRejected ? city.createCitizen("SafeCitizen", 20, "X")
                                     : city.createCitizen("Line\nBreak", 20, "X");
            auto b = buildingRejected ? city.createResidentialBuilding("SafeBlock", 10)
                                      : city.createResidentialBuilding("Mall\nA", 10);

            CitySerializer::saveToFile(city, fileName);

            CityManager loaded;
            CitySerializer::loadFromFile(loaded, fileName);

            EXPECT_EQ(loaded.getTotalCitizens(), city.getTotalCitizens());
            EXPECT_EQ(loaded.getTotalBuildings(), city.getTotalBuildings());

            ::remove(fileName.c_str());
        }
    });

    //--------------------------------------------------
    // 21. STRESS TEST
    //--------------------------------------------------
    runner.run("Stress Test", []() {
        CityManager city;

        auto home = city.createResidentialBuilding("MassHome", 5000);
        auto office = city.createCommercialBuilding("MassOffice", 5000);
        auto hospital = city.createServiceBuilding("MassHospital", 5000);

        for (int i = 0; i < 2000; ++i) {
            auto c = city.createCitizen(
                "Citizen" + to_string(i),
                18 + (i % 60),
                (i % 2 == 0) ? "Engineer" : "Teacher"
            );

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
                if (a->getAge() == b->getAge()) {
                    return a->getId() < b->getId();
                }
                return a->getAge() < b->getAge();
            }
        );

        EXPECT_EQ(sorted.size(), 2000);
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();
}
    catch (const std::exception& ex) {
        std::cerr << "Unhandled exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unhandled unknown exception" << std::endl;
        return 1;
    }

    return 0;
}