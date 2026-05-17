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

int main() {

    TestRunner runner;

    //--------------------------------------------------
    // 1. SORT CITIZENS BY AGE
    //--------------------------------------------------
    runner.run(
        "Sort Citizens By Age",
        []() {

        CityManager city;

        city.createCitizen(
            "A",
            40,
            "X"
        );

        city.createCitizen(
            "B",
            20,
            "X"
        );

        auto queried =
            city.queryCitizens(
                [](const shared_ptr<Citizen>&) {
                    return true;
                }
            );

        auto sorted =
            city.sortCitizens(
                queried,
                [](const shared_ptr<Citizen>& a,
                   const shared_ptr<Citizen>& b) {

                    return a->getAge()
                           < b->getAge();
                }
            );

        EXPECT_EQ(
            sorted[0]->getAge(),
            20
        );
    });

    //--------------------------------------------------
    // 2. PROFESSION DISTRIBUTION
    //--------------------------------------------------
    runner.run(
        "Profession Distribution",
        []() {

        CityManager city;

        city.createCitizen(
            "A",
            20,
            "Engineer"
        );

        city.createCitizen(
            "B",
            30,
            "Engineer"
        );

        city.createCitizen(
            "C",
            40,
            "Teacher"
        );

        auto report =
            city.getProfessionDistribution();

        EXPECT_EQ(
            report["Engineer"],
            2
        );

        EXPECT_EQ(
            report["Teacher"],
            1
        );
    });

    //--------------------------------------------------
    // 3. BUILDING TYPE REPORT
    //--------------------------------------------------
    runner.run(
        "Building Type Distribution",
        []() {

        CityManager city;

        city.createResidentialBuilding(
            "R1",
            10
        );

        city.createCommercialBuilding(
            "C1",
            20
        );

        city.createCommercialBuilding(
            "C2",
            20
        );

        auto report =
            city.getBuildingTypeDistribution();

        EXPECT_EQ(
            report["Commercial"],
            2
        );
    });

    //--------------------------------------------------
    // 4. OCCUPANCY REPORT
    //--------------------------------------------------
    runner.run(
        "Occupancy Report",
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

        auto report =
            city.getBuildingOccupancyReport();

        EXPECT_TRUE(
            !report.empty()
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}