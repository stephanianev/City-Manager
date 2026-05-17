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
throw runtime_error("Condition failed: " #cond);

#define EXPECT_EQ(a, b) \
if (!((a) == (b))) \
throw runtime_error("Expected equality failed");

int main() {

    TestRunner runner;

    //--------------------------------------------------
    // 1. CITIZEN CREATION EVENT
    //--------------------------------------------------
    runner.run(
        "Citizen Creation Event",
        []() {

        CityManager city;

        city.createCitizen(
            "Ivan",
            25,
            "Engineer"
        );

        EXPECT_EQ(
            city.getEventManager()
                .getEventCount(),
            1
        );
    });

    //--------------------------------------------------
    // 2. BUILDING CREATION EVENT
    //--------------------------------------------------
    runner.run(
        "Building Creation Event",
        []() {

        CityManager city;

        city.createResidentialBuilding(
            "Block A",
            100
        );

        EXPECT_EQ(
            city.getEventManager()
                .getEventCount(),
            1
        );
    });

    //--------------------------------------------------
    // 3. ASSIGN HOME EVENT
    //--------------------------------------------------
    runner.run(
        "Assign Home Event",
        []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                25,
                "Engineer"
            );

        auto building =
            city.createResidentialBuilding(
                "Home",
                100
            );

        city.assignHome(
            citizen->getId(),
            building->getId()
        );

        EXPECT_EQ(
            city.getEventManager()
                .getEventCount(),
            3
        );
    });

    //--------------------------------------------------
    // 4. FAILED OPERATION NO EVENT
    //--------------------------------------------------
    runner.run(
        "Failed Operation No Event",
        []() {

        CityManager city;

        city.createCitizen(
            "Ivan",
            25,
            "Engineer"
        );

        size_t before =
            city.getEventManager()
                .getEventCount();

        try {

            city.assignHome(
                999,
                999
            );
        }
        catch (...) {}

        EXPECT_EQ(
            city.getEventManager()
                .getEventCount(),
            before
        );
    });

    //--------------------------------------------------
    // 5. REMOVE BUILDING EVENT
    //--------------------------------------------------
    runner.run(
        "Remove Building Event",
        []() {

        CityManager city;

        auto building =
            city.createResidentialBuilding(
                "Block",
                50
            );

        city.removeBuilding(
            building->getId()
        );

        EXPECT_EQ(
            city.getEventManager()
                .getEventCount(),
            2
        );
    });

    //--------------------------------------------------
    // 6. EVENT ORDER
    //--------------------------------------------------
    runner.run(
        "Event Order",
        []() {

        CityManager city;

        city.createCitizen(
            "A",
            20,
            "X"
        );

        city.createResidentialBuilding(
            "B",
            10
        );

        const auto& events =
            city.getEventManager()
                .getEvents();

        EXPECT_EQ(
            events[0]->getType(),
            "CitizenCreated"
        );

        EXPECT_EQ(
            events[1]->getType(),
            "BuildingCreated"
        );
    });

    //--------------------------------------------------
    // 7. LARGE EVENT STRESS
    //--------------------------------------------------
    runner.run(
        "Large Event Stress",
        []() {

        CityManager city;

        for (int i = 0; i < 5000; i++) {

            city.createCitizen(
                "User",
                20,
                "X"
            );
        }

        EXPECT_EQ(
            city.getEventManager()
                .getEventCount(),
            5000
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}