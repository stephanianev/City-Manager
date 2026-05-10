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
    // 1. BASIC MOVEMENT
    //--------------------------------------------------
    runner.run("Basic Citizen Movement", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                25,
                "Engineer"
            );

        auto building =
            city.createCommercialBuilding(
                "Mall",
                10
            );

        city.moveCitizen(
            citizen->getId(),
            building->getId()
        );

        EXPECT_TRUE(
            citizen->getLocation() != nullptr
        );

        EXPECT_EQ(
            citizen->getLocation()->getId(),
            building->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // 2. MOVE BETWEEN BUILDINGS
    //--------------------------------------------------
    runner.run("Move Between Buildings", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Anna",
                30,
                "Doctor"
            );

        auto b1 =
            city.createCommercialBuilding(
                "A",
                10
            );

        auto b2 =
            city.createCommercialBuilding(
                "B",
                10
            );

        city.moveCitizen(
            citizen->getId(),
            b1->getId()
        );

        city.moveCitizen(
            citizen->getId(),
            b2->getId()
        );

        EXPECT_EQ(
            b1->getOccupantCount(),
            0
        );

        EXPECT_EQ(
            b2->getOccupantCount(),
            1
        );

        EXPECT_EQ(
            citizen->getLocation()->getId(),
            b2->getId()
        );
    });

    //--------------------------------------------------
    // 3. INVALID CITIZEN
    //--------------------------------------------------
    runner.run("Move Invalid Citizen", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Mall",
                10
            );

        EXPECT_THROW(
            city.moveCitizen(
                999,
                building->getId()
            )
        );
    });

    //--------------------------------------------------
    // 4. INVALID BUILDING
    //--------------------------------------------------
    runner.run("Move To Invalid Building", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        EXPECT_THROW(
            city.moveCitizen(
                citizen->getId(),
                999
            )
        );
    });

    //--------------------------------------------------
    // 5. CAPACITY ENFORCEMENT
    //--------------------------------------------------
    runner.run("Movement Capacity Enforcement", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Tiny",
                1
            );

        auto c1 =
            city.createCitizen(
                "A",
                20,
                "X"
            );

        auto c2 =
            city.createCitizen(
                "B",
                20,
                "X"
            );

        city.moveCitizen(
            c1->getId(),
            building->getId()
        );

        EXPECT_THROW(
            city.moveCitizen(
                c2->getId(),
                building->getId()
            )
        );
    });

    //--------------------------------------------------
    // 6. DUPLICATE MOVE
    //--------------------------------------------------
    runner.run("Duplicate Move Same Building", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        auto building =
            city.createCommercialBuilding(
                "Mall",
                10
            );

        city.moveCitizen(
            citizen->getId(),
            building->getId()
        );

        city.moveCitizen(
            citizen->getId(),
            building->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // 7. HOME AND LOCATION SEPARATION
    //--------------------------------------------------
    runner.run("Home And Location Separation", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Peter",
                40,
                "Manager"
            );

        auto home =
            city.createResidentialBuilding(
                "Home",
                10
            );

        auto office =
            city.createCommercialBuilding(
                "Office",
                10
            );

        city.assignHome(
            citizen->getId(),
            home->getId()
        );

        city.moveCitizen(
            citizen->getId(),
            office->getId()
        );

        EXPECT_EQ(
            citizen->getHome()->getId(),
            home->getId()
        );

        EXPECT_EQ(
            citizen->getLocation()->getId(),
            office->getId()
        );
    });

    //--------------------------------------------------
    // 8. STRESS TEST
    //--------------------------------------------------
    runner.run("Mass Movement Stress Test", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Mega",
                10000
            );

        for (int i = 0; i < 5000; i++) {

            auto citizen =
                city.createCitizen(
                    "User",
                    20,
                    "X"
                );

            city.moveCitizen(
                citizen->getId(),
                building->getId()
            );
        }

        EXPECT_EQ(
            building->getOccupantCount(),
            5000
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}