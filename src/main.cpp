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
    // 1. COMMERCIAL BUILDING
    //--------------------------------------------------
    runner.run("Commercial Building Creation", []() {

        CityManager city;

        auto b =
            city.createCommercialBuilding(
                "Mall",
                100
            );

        EXPECT_EQ(
            b->getType(),
            "Commercial"
        );
    });

    //--------------------------------------------------
    // 2. INDUSTRIAL BUILDING
    //--------------------------------------------------
    runner.run("Industrial Building Creation", []() {

        CityManager city;

        auto b =
            city.createIndustrialBuilding(
                "Factory",
                200
            );

        EXPECT_EQ(
            b->getType(),
            "Industrial"
        );
    });

    //--------------------------------------------------
    // 3. SERVICE BUILDING
    //--------------------------------------------------
    runner.run("Service Building Creation", []() {

        CityManager city;

        auto b =
            city.createServiceBuilding(
                "Hospital",
                50
            );

        EXPECT_EQ(
            b->getType(),
            "Service"
        );
    });

    //--------------------------------------------------
    // 4. BASIC WORKPLACE ASSIGNMENT
    //--------------------------------------------------
    runner.run("Basic Workplace Assignment", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                25,
                "Engineer"
            );

        auto building =
            city.createCommercialBuilding(
                "Office",
                10
            );

        city.assignWorkplace(
            citizen->getId(),
            building->getId()
        );

        EXPECT_TRUE(
            citizen->getWorkplace() != nullptr
        );

        EXPECT_EQ(
            citizen->getWorkplace()->getId(),
            building->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // 5. RESIDENTIAL WORKPLACE BLOCK
    //--------------------------------------------------
    runner.run("Residential Workplace Forbidden", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Anna",
                30,
                "Doctor"
            );

        auto home =
            city.createResidentialBuilding(
                "Block",
                10
            );

        EXPECT_THROW(
            city.assignWorkplace(
                citizen->getId(),
                home->getId()
            )
        );
    });

    //--------------------------------------------------
    // 6. WORKPLACE REASSIGNMENT
    //--------------------------------------------------
    runner.run("Workplace Reassignment", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Peter",
                40,
                "Manager"
            );

        auto office1 =
            city.createCommercialBuilding(
                "Office1",
                10
            );

        auto office2 =
            city.createCommercialBuilding(
                "Office2",
                10
            );

        city.assignWorkplace(
            citizen->getId(),
            office1->getId()
        );

        city.assignWorkplace(
            citizen->getId(),
            office2->getId()
        );

        EXPECT_EQ(
            office1->getOccupantCount(),
            0
        );

        EXPECT_EQ(
            office2->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // 7. CAPACITY ENFORCEMENT
    //--------------------------------------------------
    runner.run("Workplace Capacity Enforcement", []() {

        CityManager city;

        auto office =
            city.createCommercialBuilding(
                "Tiny Office",
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

        city.assignWorkplace(
            c1->getId(),
            office->getId()
        );

        EXPECT_THROW(
            city.assignWorkplace(
                c2->getId(),
                office->getId()
            )
        );
    });

    //--------------------------------------------------
    // 8. MASS WORKPLACE ASSIGNMENT
    //--------------------------------------------------
    runner.run("Mass Workplace Assignment", []() {

        CityManager city;

        auto factory =
            city.createIndustrialBuilding(
                "Mega Factory",
                10000
            );

        for (int i = 0; i < 5000; i++) {

            auto c =
                city.createCitizen(
                    "Worker",
                    20,
                    "Worker"
                );

            city.assignWorkplace(
                c->getId(),
                factory->getId()
            );
        }

        EXPECT_EQ(
            factory->getOccupantCount(),
            5000
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}