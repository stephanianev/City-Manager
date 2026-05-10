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
    // 1. REMOVE CITIZEN
    //--------------------------------------------------
    runner.run("Remove Citizen", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        city.removeCitizen(
            citizen->getId()
        );

        EXPECT_EQ(
            city.getTotalCitizens(),
            0
        );
    });

    //--------------------------------------------------
    // 2. REMOVE INVALID CITIZEN
    //--------------------------------------------------
    runner.run("Remove Invalid Citizen", []() {

        CityManager city;

        EXPECT_THROW(
            city.removeCitizen(999)
        );
    });

    //--------------------------------------------------
    // 3. REMOVE CITIZEN FROM LOCATION
    //--------------------------------------------------
    runner.run("Citizen Removed From Occupancy", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Mall",
                10
            );

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
            );

        city.moveCitizen(
            citizen->getId(),
            building->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );

        city.removeCitizen(
            citizen->getId()
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            0
        );
    });

    //--------------------------------------------------
    // 4. REMOVE BUILDING
    //--------------------------------------------------
    runner.run("Remove Building", []() {

        CityManager city;

        auto building =
            city.createCommercialBuilding(
                "Office",
                10
            );

        city.removeBuilding(
            building->getId()
        );

        EXPECT_EQ(
            city.getTotalBuildings(),
            0
        );
    });

    //--------------------------------------------------
    // 5. REMOVE INVALID BUILDING
    //--------------------------------------------------
    runner.run("Remove Invalid Building", []() {

        CityManager city;

        EXPECT_THROW(
            city.removeBuilding(999)
        );
    });

    //--------------------------------------------------
    // 6. CLEAR HOME REFERENCES
    //--------------------------------------------------
    runner.run("Clear Home References", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "A",
                20,
                "X"
            );

        auto home =
            city.createResidentialBuilding(
                "Home",
                10
            );

        city.assignHome(
            citizen->getId(),
            home->getId()
        );

        city.removeBuilding(
            home->getId()
        );

        EXPECT_TRUE(
            citizen->getHome() == nullptr
        );
    });

    //--------------------------------------------------
    // 7. CLEAR WORKPLACE REFERENCES
    //--------------------------------------------------
    runner.run("Clear Workplace References", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "A",
                20,
                "X"
            );

        auto office =
            city.createCommercialBuilding(
                "Office",
                10
            );

        city.assignWorkplace(
            citizen->getId(),
            office->getId()
        );

        city.removeBuilding(
            office->getId()
        );

        EXPECT_TRUE(
            citizen->getWorkplace() == nullptr
        );
    });

    //--------------------------------------------------
    // 8. CLEAR LOCATION REFERENCES
    //--------------------------------------------------
    runner.run("Clear Location References", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "A",
                20,
                "X"
            );

        auto mall =
            city.createCommercialBuilding(
                "Mall",
                10
            );

        city.moveCitizen(
            citizen->getId(),
            mall->getId()
        );

        city.removeBuilding(
            mall->getId()
        );

        EXPECT_TRUE(
            citizen->getLocation() == nullptr
        );
    });

    //--------------------------------------------------
    // 9. MASS CITIZEN REMOVAL
    //--------------------------------------------------
    runner.run("Mass Citizen Removal", []() {

        CityManager city;

        for (int i = 0; i < 5000; i++) {

            auto citizen =
                city.createCitizen(
                    "User",
                    20,
                    "X"
                );

            city.removeCitizen(
                citizen->getId()
            );
        }

        EXPECT_EQ(
            city.getTotalCitizens(),
            0
        );
    });

    //--------------------------------------------------
    // 10. MASS BUILDING REMOVAL
    //--------------------------------------------------
    runner.run("Mass Building Removal", []() {

        CityManager city;

        vector<int> ids;

        for (int i = 0; i < 5000; i++) {

            auto building =
                city.createCommercialBuilding(
                    "Building",
                    10
                );

            ids.push_back(
                building->getId()
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