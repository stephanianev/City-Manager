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
    void run(
        const string& name,
        function<void()> test
    ) {

        try {

            test();

            results.push_back({
                name,
                true,
                "OK"
            });
        }
        catch (const exception& e) {

            results.push_back({
                name,
                false,
                e.what()
            });
        }
        catch (...) {

            results.push_back({
                name,
                false,
                "Unknown error"
            });
        }
    }

    void report() const {

        int passed = 0;

        cout
            << "\n========== TEST REPORT ==========\n";

        for (const auto& r : results) {

            cout
                << (r.passed
                    ? "[PASS] "
                    : "[FAIL] ")
                << r.name;

            if (!r.passed) {

                cout
                    << " -> "
                    << r.message;
            }

            cout << endl;

            if (r.passed) {
                passed++;
            }
        }

        cout
            << "\nSummary: "
            << passed
            << "/"
            << results.size()
            << " tests passed\n";

        cout
            << "================================\n";
    }
};

#define EXPECT_TRUE(cond) \
if (!(cond)) \
throw runtime_error("Condition failed");

#define EXPECT_EQ(a, b) \
if (!((a) == (b))) \
throw runtime_error("Expected equality failed");

#define EXPECT_THROW(stmt) \
{ \
    bool thrown = false; \
    try { stmt; } \
    catch (...) { thrown = true; } \
    if (!thrown) { \
        throw runtime_error( \
            "Expected exception not thrown" \
        ); \
    } \
}

int main() {

    TestRunner runner;

    //--------------------------------------------------
    // 1. VALIDATION NAME
    //--------------------------------------------------
    runner.run(
        "Validation Name",
        []() {

        CityManager city;

        EXPECT_THROW(
            city.createCitizen(
                "",
                20,
                "X"
            )
        );
    });

    //--------------------------------------------------
    // 2. VALIDATION AGE
    //--------------------------------------------------
    runner.run(
        "Validation Age",
        []() {

        CityManager city;

        EXPECT_THROW(
            city.createCitizen(
                "Ivan",
                -1,
                "X"
            )
        );
    });

    //--------------------------------------------------
    // 3. AVERAGE AGE
    //--------------------------------------------------
    runner.run(
        "Average Age",
        []() {

        CityManager city;

        city.createCitizen(
            "A",
            20,
            "X"
        );

        city.createCitizen(
            "B",
            40,
            "X"
        );

        EXPECT_EQ(
            city.getAverageCitizenAge(),
            30.0
        );
    });

    //--------------------------------------------------
    // 4. EMPLOYMENT RATE
    //--------------------------------------------------
    runner.run(
        "Employment Rate",
        []() {

        CityManager city;

        auto c1 =
            city.createCitizen(
                "A",
                20,
                "X"
            );

        city.createCitizen(
            "B",
            20,
            "X"
        );

        auto building =
            city.createCommercialBuilding(
                "Office",
                10
            );

        city.assignWorkplace(
            c1->getId(),
            building->getId()
        );

        EXPECT_EQ(
            city.getEmploymentRate(),
            0.5
        );
    });

    //--------------------------------------------------
    // 5. OCCUPANCY BY TYPE
    //--------------------------------------------------
    runner.run(
        "Occupancy By Type",
        []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                20,
                "X"
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

        auto stats =
            city.getAverageOccupancyByType();

        EXPECT_TRUE(
            stats["Residential"] > 0.0
        );

        EXPECT_EQ(
            building->getOccupantCount(),
            1
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}