#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <algorithm>
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

#define EXPECT_THROW(stmt) \
    { bool thrown = false; \
      try { stmt; } catch (...) { thrown = true; } \
      if (!thrown) throw runtime_error("Expected exception not thrown"); }

int main() {
    TestRunner runner;

    //--------------------------------------------------
    // 1. FIND CITIZENS BY NAME
    //--------------------------------------------------
    runner.run("Find Citizens By Name", []() {
        CityManager city;

        auto c1 = city.createCitizen("Ivan", 25, "Engineer");
        auto c2 = city.createCitizen("Maria", 30, "Doctor");
        auto c3 = city.createCitizen("Ivan", 40, "Teacher");

        auto result = city.findCitizensByName("Ivan");

        EXPECT_EQ(result.size(), 2);
        EXPECT_EQ(result[0]->getName(), "Ivan");
        EXPECT_EQ(result[1]->getName(), "Ivan");
        EXPECT_EQ(result[0]->getId(), c1->getId());
        EXPECT_EQ(result[1]->getId(), c3->getId());
    });

    //--------------------------------------------------
    // 2. FIND BUILDINGS BY NAME
    //--------------------------------------------------
    runner.run("Find Buildings By Name", []() {
        CityManager city;

        auto b1 = city.createResidentialBuilding("Block A", 10);
        auto b2 = city.createCommercialBuilding("Mall", 20);
        auto b3 = city.createServiceBuilding("Block A", 30);

        auto result = city.findBuildingsByName("Block A");

        EXPECT_EQ(result.size(), 2);
        EXPECT_EQ(result[0]->getName(), "Block A");
        EXPECT_EQ(result[1]->getName(), "Block A");
        EXPECT_EQ(result[0]->getId(), b1->getId());
        EXPECT_EQ(result[1]->getId(), b3->getId());
    });

    //--------------------------------------------------
    // 3. NO MATCH RETURNS EMPTY
    //--------------------------------------------------
    runner.run("Name Search No Match Returns Empty", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createResidentialBuilding("Block A", 10);

        auto citizens = city.findCitizensByName("Maria");
        auto buildings = city.findBuildingsByName("Mall");

        EXPECT_TRUE(citizens.empty());
        EXPECT_TRUE(buildings.empty());
    });

    //--------------------------------------------------
    // 4. CASE SENSITIVITY
    //--------------------------------------------------
    runner.run("Name Search Is Case Sensitive", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createCitizen("ivan", 30, "Doctor");

        auto upper = city.findCitizensByName("Ivan");
        auto lower = city.findCitizensByName("ivan");

        EXPECT_EQ(upper.size(), 1);
        EXPECT_EQ(lower.size(), 1);
        EXPECT_FALSE(upper[0]->getId() == lower[0]->getId());
    });

    //--------------------------------------------------
    // 5. READ-ONLY BEHAVIOR
    //--------------------------------------------------
    runner.run("Name Search Is Read Only", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createCitizen("Maria", 30, "Doctor");
        city.createResidentialBuilding("Block A", 10);
        city.createCommercialBuilding("Mall", 20);

        auto beforeCitizens = city.getTotalCitizens();
        auto beforeBuildings = city.getTotalBuildings();
        auto beforeEvents = city.getEventManager().getEventCount();

        auto result1 = city.findCitizensByName("Ivan");
        auto result2 = city.findBuildingsByName("Block A");

        EXPECT_EQ(beforeCitizens, city.getTotalCitizens());
        EXPECT_EQ(beforeBuildings, city.getTotalBuildings());
        EXPECT_EQ(beforeEvents, city.getEventManager().getEventCount());
        EXPECT_EQ(result1.size(), 1);
        EXPECT_EQ(result2.size(), 1);
    });

    //--------------------------------------------------
    // 6. INVALID SEARCH INPUT
    //--------------------------------------------------
    runner.run("Invalid Name Search Input", []() {
        CityManager city;

        EXPECT_THROW(city.findCitizensByName(""));
        EXPECT_THROW(city.findCitizensByName("   "));
        EXPECT_THROW(city.findBuildingsByName(""));
        EXPECT_THROW(city.findBuildingsByName("\t"));
    });

    //--------------------------------------------------
    // 7. DUPLICATES ARE RETURNED IN ID ORDER
    //--------------------------------------------------
    runner.run("Duplicate Name Results Are Stable", []() {
        CityManager city;

        auto c1 = city.createCitizen("Alex", 20, "A");
        auto c2 = city.createCitizen("Alex", 30, "B");
        auto c3 = city.createCitizen("Alex", 40, "C");

        auto result = city.findCitizensByName("Alex");

        EXPECT_EQ(result.size(), 3);
        EXPECT_EQ(result[0]->getId(), c1->getId());
        EXPECT_EQ(result[1]->getId(), c2->getId());
        EXPECT_EQ(result[2]->getId(), c3->getId());
    });

    //--------------------------------------------------
    // 8. LARGE NAME SEARCH
    //--------------------------------------------------
    runner.run("Large Name Search", []() {
        CityManager city;

        for (int i = 0; i < 5000; i++) {
            city.createCitizen("User" + to_string(i), 20, "X");
        }

        city.createCitizen("Target", 25, "Engineer");
        city.createCitizen("Target", 26, "Doctor");

        auto result = city.findCitizensByName("Target");

        EXPECT_EQ(result.size(), 2);
        EXPECT_EQ(result[0]->getName(), "Target");
        EXPECT_EQ(result[1]->getName(), "Target");
    });

    //--------------------------------------------------
    // 9. PERSISTENCE ROUNDTRIP DOES NOT BREAK SEARCH
    //--------------------------------------------------
    runner.run("Persistence Roundtrip Preserves Name Search", []() {
        const string fileName = "name_search_roundtrip.txt";

        {
            CityManager city;

            auto c1 = city.createCitizen("Ivan", 25, "Engineer");
            auto c2 = city.createCitizen("Maria", 30, "Doctor");
            auto b1 = city.createResidentialBuilding("Block A", 10);
            auto b2 = city.createCommercialBuilding("Office A", 20);
            auto b3 = city.createServiceBuilding("Hospital A", 30);

            city.assignHome(c1->getId(), b1->getId());
            city.assignWorkplace(c1->getId(), b2->getId());
            city.moveCitizen(c1->getId(), b2->getId());

            city.assignWorkplace(c2->getId(), b3->getId());
            city.moveCitizen(c2->getId(), b3->getId());

            CitySerializer::saveToFile(city, fileName);
        }

        CityManager loaded;
        CitySerializer::loadFromFile(loaded, fileName);

        auto citizensIvan = loaded.findCitizensByName("Ivan");
        auto citizensMaria = loaded.findCitizensByName("Maria");
        auto buildingsBlock = loaded.findBuildingsByName("Block A");
        auto buildingsOffice = loaded.findBuildingsByName("Office A");

        EXPECT_EQ(citizensIvan.size(), 1);
        EXPECT_EQ(citizensMaria.size(), 1);
        EXPECT_EQ(buildingsBlock.size(), 1);
        EXPECT_EQ(buildingsOffice.size(), 1);

        EXPECT_EQ(citizensIvan[0]->getName(), "Ivan");
        EXPECT_EQ(citizensMaria[0]->getName(), "Maria");

        ::remove(fileName.c_str());
    });

    //--------------------------------------------------
    // 10. SEARCH AFTER REMOVAL
    //--------------------------------------------------
    runner.run("Search After Removal Updates Correctly", []() {
        CityManager city;

        auto c1 = city.createCitizen("Ivan", 25, "Engineer");
        auto c2 = city.createCitizen("Ivan", 30, "Doctor");
        auto b1 = city.createResidentialBuilding("Block A", 10);
        auto b2 = city.createResidentialBuilding("Block A", 20);

        EXPECT_EQ(city.findCitizensByName("Ivan").size(), 2);
        EXPECT_EQ(city.findBuildingsByName("Block A").size(), 2);

        city.removeCitizen(c1->getId());
        city.removeBuilding(b1->getId());

        auto citizens = city.findCitizensByName("Ivan");
        auto buildings = city.findBuildingsByName("Block A");

        EXPECT_EQ(citizens.size(), 1);
        EXPECT_EQ(buildings.size(), 1);
        EXPECT_EQ(citizens[0]->getId(), c2->getId());
        EXPECT_EQ(buildings[0]->getId(), b2->getId());
    });

    //--------------------------------------------------
    // 11. SEARCH DOES NOT MODIFY STATE
    //--------------------------------------------------
    runner.run("Search Does Not Modify State", []() {
        CityManager city;

        city.createCitizen("Ivan", 25, "Engineer");
        city.createCitizen("Maria", 30, "Doctor");
        city.createResidentialBuilding("Block A", 10);
        city.createCommercialBuilding("Office A", 20);

        auto beforeCitizens = city.getTotalCitizens();
        auto beforeBuildings = city.getTotalBuildings();
        auto beforeEvents = city.getEventManager().getEventCount();

        city.findCitizensByName("Ivan");
        city.findBuildingsByName("Block A");

        EXPECT_EQ(city.getTotalCitizens(), beforeCitizens);
        EXPECT_EQ(city.getTotalBuildings(), beforeBuildings);
        EXPECT_EQ(city.getEventManager().getEventCount(), beforeEvents);
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}