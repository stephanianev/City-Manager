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
    // 1. BASIC HOME ASSIGNMENT
    //--------------------------------------------------
    runner.run("Basic Home Assignment", []() {

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

        EXPECT_TRUE(
            citizen->getHome() != nullptr
        );

        EXPECT_EQ(
            citizen->getHome()->getId(),
            building->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // 2. INVALID CITIZEN
    //--------------------------------------------------
    runner.run("Assign Invalid Citizen", []() {

        CityManager city;

        auto building =
            city.createResidentialBuilding(
                "Block",
                10
            );

        EXPECT_THROW(
            city.assignHome(
                999,
                building->getId()
            )
        );
    });

    //--------------------------------------------------
    // 3. INVALID BUILDING
    //--------------------------------------------------
    runner.run("Assign Invalid Building", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        EXPECT_THROW(
            city.assignHome(
                citizen->getId(),
                999
            )
        );
    });

    //--------------------------------------------------
    // 4. CAPACITY LIMIT
    //--------------------------------------------------
    runner.run("Building Capacity Enforcement", []() {

        CityManager city;

        auto b =
            city.createResidentialBuilding(
                "Small",
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

        city.assignHome(
            c1->getId(),
            b->getId()
        );

        EXPECT_THROW(
            city.assignHome(
                c2->getId(),
                b->getId()
            )
        );
    });

    //--------------------------------------------------
    // 5. REASSIGN HOME
    //--------------------------------------------------
    runner.run("Home Reassignment", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

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

        city.assignHome(
            citizen->getId(),
            b1->getId()
        );

        city.assignHome(
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
            citizen->getHome()->getId(),
            b2->getId()
        );
    });

    //--------------------------------------------------
    // 6. DUPLICATE ASSIGNMENT
    //--------------------------------------------------
    runner.run("Duplicate Home Assignment", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        auto building =
            city.createResidentialBuilding(
                "A",
                10
            );

        city.assignHome(
            citizen->getId(),
            building->getId()
        );

        city.assignHome(
            citizen->getId(),
            building->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // 7. STRESS TEST
    //--------------------------------------------------
    runner.run("Mass Assignment Stress Test", []() {

        CityManager city;

        auto building =
            city.createResidentialBuilding(
                "Mega",
                10000
            );

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