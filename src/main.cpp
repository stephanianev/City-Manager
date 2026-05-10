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
    // 1. FIND BY PROFESSION
    //--------------------------------------------------
    runner.run("Find Citizens By Profession", []() {

        CityManager city;

        city.createCitizen("A", 20, "Engineer");
        city.createCitizen("B", 30, "Doctor");
        city.createCitizen("C", 40, "Engineer");

        auto engineers =
            city.findCitizensByProfession(
                "Engineer"
            );

        EXPECT_EQ(engineers.size(), 2);
    });

    //--------------------------------------------------
    // 2. EMPTY PROFESSION RESULT
    //--------------------------------------------------
    runner.run("Empty Profession Result", []() {

        CityManager city;

        city.createCitizen("A", 20, "Engineer");

        auto result =
            city.findCitizensByProfession(
                "Pilot"
            );

        EXPECT_EQ(result.size(), 0);
    });

    //--------------------------------------------------
    // 3. LIST CITIZENS IN BUILDING
    //--------------------------------------------------
    runner.run("List Citizens In Building", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Mall",
                10
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

        city.moveCitizen(
            c2->getId(),
            building->getId()
        );

        auto occupants =
            city.listCitizensInBuilding(
                building->getId()
            );

        EXPECT_EQ(occupants.size(), 2);
    });

    //--------------------------------------------------
    // 4. INVALID BUILDING QUERY
    //--------------------------------------------------
    runner.run("Invalid Building Query", []() {

        CityManager city;

        EXPECT_THROW(
            city.listCitizensInBuilding(
                999
            )
        );
    });

    //--------------------------------------------------
    // 5. BUILDINGS WITH CAPACITY
    //--------------------------------------------------
    runner.run("Buildings With Capacity", []() {

        CityManager city;

        auto b1 =
            city.createCommercialBuilding(
                "A",
                1
            );

        auto b2 =
            city.createCommercialBuilding(
                "B",
                2
            );

        auto c =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        city.moveCitizen(
            c->getId(),
            b1->getId()
        );

        auto available =
            city.findBuildingsWithCapacity();

        EXPECT_EQ(available.size(), 1);
        EXPECT_EQ(
            available[0]->getId(),
            b2->getId()
        );
    });

    //--------------------------------------------------
    // 6. TOTAL COUNTS
    //--------------------------------------------------
    runner.run("Total Counts", []() {

        CityManager city;

        city.createCitizen("A", 20, "X");
        city.createCitizen("B", 20, "X");

        city.createCommercialBuilding(
            "Mall",
            10
        );

        city.createResidentialBuilding(
            "Block",
            10
        );

        EXPECT_EQ(
            city.getTotalCitizens(),
            2
        );

        EXPECT_EQ(
            city.getTotalBuildings(),
            2
        );
    });

    //--------------------------------------------------
    // 7. HOMELESS COUNT
    //--------------------------------------------------
    runner.run("Homeless Count", []() {

        CityManager city;

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

        auto home =
            city.createResidentialBuilding(
                "Home",
                10
            );

        city.assignHome(
            c1->getId(),
            home->getId()
        );

        EXPECT_EQ(
            city.getHomelessCount(),
            1
        );
    });

    //--------------------------------------------------
    // 8. UNEMPLOYED COUNT
    //--------------------------------------------------
    runner.run("Unemployed Count", []() {

        CityManager city;

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

        auto office =
            city.createCommercialBuilding(
                "Office",
                10
            );

        city.assignWorkplace(
            c1->getId(),
            office->getId()
        );

        EXPECT_EQ(
            city.getUnemployedCount(),
            1
        );
    });

    //--------------------------------------------------
    // 9. OCCUPANCY RATE
    //--------------------------------------------------
    runner.run("Occupancy Rate", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Office",
                4
            );

        for (int i = 0; i < 2; i++) {

            auto c =
                city.createCitizen(
                    "User",
                    20,
                    "X"
                );

            city.moveCitizen(
                c->getId(),
                building->getId()
            );
        }

        double rate =
            city.getOccupancyRate(
                building->getId()
            );

        EXPECT_TRUE(rate == 0.5);
    });

    //--------------------------------------------------
    // 10. STRESS QUERY TEST
    //--------------------------------------------------
    runner.run("Stress Query Test", []() {

        CityManager city;

        for (int i = 0; i < 10000; i++) {

            city.createCitizen(
                "User",
                20,
                "Engineer"
            );
        }

        auto result =
            city.findCitizensByProfession(
                "Engineer"
            );

        EXPECT_EQ(result.size(), 10000);
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}