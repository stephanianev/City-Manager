#include <iostream>
#include <vector>
#include <string>
#include <functional>

#include "core/CityManager.h"

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

            cout << (r.passed ? "[PASS] " : "[FAIL] ")
                 << r.name;

            if (!r.passed) {
                cout << " -> " << r.message;
            }

            cout << endl;

            if (r.passed) {
                passed++;
            }
        }

        cout << "\nSummary: "
             << passed
             << "/"
             << results.size()
             << " tests passed\n";

        cout << "================================\n";
    }
};

#define EXPECT_TRUE(cond) \
    if (!(cond)) throw runtime_error("Condition failed: " #cond);

#define EXPECT_EQ(a, b) \
    if (!((a) == (b))) throw runtime_error("Expected equality failed");

#define EXPECT_THROW(stmt) \
    { bool thrown = false; \
      try { stmt; } catch (...) { thrown = true; } \
      if (!thrown) throw runtime_error("Expected exception not thrown"); }

int main() {

    TestRunner runner;

    //--------------------------------------------------
    // 1. BASIC REMOVE BUILDING
    //--------------------------------------------------
    runner.run("Basic Remove Building", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                25,
                "Engineer"
            );

        auto building =
            city.createResidentialBuilding(
                "Block A",
                10
            );

        city.assignHome(
            citizen->getId(),
            building->getId()
        );

        city.removeBuilding(
            building->getId()
        );

        EXPECT_EQ(
            city.getTotalBuildings(),
            0
        );

        EXPECT_TRUE(
            citizen->getHome() == nullptr
        );
    });

    //--------------------------------------------------
    // 2. REMOVE INVALID BUILDING
    //--------------------------------------------------
    runner.run("Remove Invalid Building", []() {

        CityManager city;

        EXPECT_THROW(
            city.removeBuilding(999)
        );
    });

    //--------------------------------------------------
    // 3. MULTIPLE BUILDINGS
    //--------------------------------------------------
    runner.run("Remove One Building From Many", []() {

        CityManager city;

        auto b1 =
            city.createResidentialBuilding(
                "A",
                10
            );

        auto b2 =
            city.createResidentialBuilding(
                "B",
                10
            );

        auto b3 =
            city.createResidentialBuilding(
                "C",
                10
            );

        city.removeBuilding(b2->getId());

        EXPECT_EQ(
            city.getTotalBuildings(),
            2
        );
    });

    //--------------------------------------------------
    // 4. CITIZEN STATE CLEANUP
    //--------------------------------------------------
    runner.run("Citizen Home Cleanup", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Anna",
                30,
                "Doctor"
            );

        auto building =
            city.createResidentialBuilding(
                "Hospital Block",
                5
            );

        city.assignHome(
            citizen->getId(),
            building->getId()
        );

        EXPECT_TRUE(
            citizen->getHome() != nullptr
        );

        city.removeBuilding(
            building->getId()
        );

        EXPECT_TRUE(
            citizen->getHome() == nullptr
        );
    });

    //--------------------------------------------------
    // 5. MASS CLEANUP
    //--------------------------------------------------
    runner.run("Mass Cleanup", []() {

        CityManager city;

        auto building =
            city.createResidentialBuilding(
                "Mega",
                10000
            );

        vector<shared_ptr<Citizen>> citizens;

        for (int i = 0; i < 5000; i++) {

            auto c =
                city.createCitizen(
                    "User",
                    20,
                    "X"
                );

            city.assignHome(
                c->getId(),
                building->getId()
            );

            citizens.push_back(c);
        }

        city.removeBuilding(
            building->getId()
        );

        for (auto& c : citizens) {

            EXPECT_TRUE(
                c->getHome() == nullptr
            );
        }
    });

    //--------------------------------------------------
    // 6. DOUBLE REMOVE
    //--------------------------------------------------
    runner.run("Remove Building Twice", []() {

        CityManager city;

        auto building =
            city.createResidentialBuilding(
                "Block",
                10
            );

        city.removeBuilding(
            building->getId()
        );

        EXPECT_THROW(
            city.removeBuilding(
                building->getId()
            )
        );
    });

    //--------------------------------------------------
    // 7. CITIZENS REMAIN ALIVE
    //--------------------------------------------------
    runner.run("Citizens Survive Building Removal", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                25,
                "Engineer"
            );

        auto building =
            city.createResidentialBuilding(
                "Block",
                10
            );

        city.assignHome(
            citizen->getId(),
            building->getId()
        );

        city.removeBuilding(
            building->getId()
        );

        EXPECT_EQ(
            city.getTotalCitizens(),
            1
        );

        EXPECT_EQ(
            citizen->getName(),
            "Ivan"
        );
    });

    //--------------------------------------------------
    // 8. STRESS TEST
    //--------------------------------------------------
    runner.run("Stress Remove Buildings", []() {

        CityManager city;

        vector<int> ids;

        for (int i = 0; i < 1000; i++) {

            auto b =
                city.createResidentialBuilding(
                    "Block",
                    100
                );

            ids.push_back(
                b->getId()
            );
        }

        for (int id : ids) {
            city.removeBuilding(id);
        }

        EXPECT_EQ(
            city.getTotalBuildings(),
            0
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}