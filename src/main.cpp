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
    // 1. RESTORE CITIZEN
    //--------------------------------------------------
    runner.run("Restore Citizen", []() {

        CityManager city;

        auto citizen =
            city.restoreCitizen(
                50,
                "Ivan",
                25,
                "Engineer"
            );

        EXPECT_EQ(citizen->getId(), 50);
        EXPECT_EQ(city.getTotalCitizens(), 1);
    });

    //--------------------------------------------------
    // 2. ID CONTINUATION AFTER RESTORE
    //--------------------------------------------------
    runner.run("ID Continuation", []() {

        CityManager city;

        city.restoreCitizen(
            100,
            "Restored",
            20,
            "X"
        );

        auto newCitizen =
            city.createCitizen(
                "New",
                30,
                "Y"
            );

        EXPECT_EQ(newCitizen->getId(), 101);
    });

    //--------------------------------------------------
    // 3. DUPLICATE RESTORE ID
    //--------------------------------------------------
    runner.run("Duplicate Restore ID", []() {

        CityManager city;

        city.restoreCitizen(
            1,
            "A",
            20,
            "X"
        );

        EXPECT_THROW(
            city.restoreCitizen(
                1,
                "B",
                30,
                "Y"
            )
        );
    });

    //--------------------------------------------------
    // 4. RESTORE BUILDING
    //--------------------------------------------------
    runner.run("Restore Building", []() {

        CityManager city;

        auto building =
            city.restoreResidentialBuilding(
                10,
                "Block A",
                100
            );

        EXPECT_EQ(building->getId(), 10);
        EXPECT_EQ(city.getTotalBuildings(), 1);
    });

    //--------------------------------------------------
    // 5. BUILDING ID CONTINUATION
    //--------------------------------------------------
    runner.run("Building ID Continuation", []() {

        CityManager city;

        city.restoreCommercialBuilding(
            500,
            "Office",
            50
        );

        auto next =
            city.createCommercialBuilding(
                "Mall",
                20
            );

        EXPECT_EQ(next->getId(), 501);
    });

    //--------------------------------------------------
    // 6. INVALID RESTORE DATA
    //--------------------------------------------------
    runner.run("Invalid Restore Data", []() {

        CityManager city;

        EXPECT_THROW(
            city.restoreCitizen(
                1,
                "",
                20,
                "X"
            )
        );

        EXPECT_THROW(
            city.restoreResidentialBuilding(
                1,
                "",
                10
            )
        );
    });

    //--------------------------------------------------
    // 7. LARGE RESTORE TEST
    //--------------------------------------------------
    runner.run("Large Restore Test", []() {

        CityManager city;

        for (int i = 1; i <= 5000; i++) {

            city.restoreCitizen(
                i,
                "User",
                20,
                "X"
            );
        }

        EXPECT_EQ(
            city.getTotalCitizens(),
            5000
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}