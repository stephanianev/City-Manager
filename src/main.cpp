#include <iostream>
#include <vector>
#include <string>
#include <functional>

#include "core/CityManager.h"
#include "serialization/CitySerializer.h"

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
    void run(const string& name,
             function<void()> test) {

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

            cout << (r.passed ? "[PASS] "
                              : "[FAIL] ")
                 << r.name;

            if (!r.passed) {
                cout << " -> "
                     << r.message;
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

int main() {

    TestRunner runner;

    //--------------------------------------------------
    // 1. SAVE + LOAD BASIC
    //--------------------------------------------------
    runner.run("Save And Load Basic", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Ivan",
                25,
                "Engineer"
            );

        auto home =
            city.createResidentialBuilding(
                "Block A",
                100
            );

        city.assignHome(
            citizen->getId(),
            home->getId()
        );

        CitySerializer::saveToFile(
            city,
            "city_save.txt"
        );

        CityManager loaded;

        CitySerializer::loadFromFile(
            loaded,
            "city_save.txt"
        );

        EXPECT_EQ(
            loaded.getTotalCitizens(),
            1
        );

        EXPECT_EQ(
            loaded.getTotalBuildings(),
            1
        );
    });

    //--------------------------------------------------
    // 2. RELATIONSHIP RESTORATION
    //--------------------------------------------------
    runner.run("Relationship Restoration", []() {

        CityManager city;

        auto citizen =
            city.createCitizen(
                "Maria",
                30,
                "Teacher"
            );

        auto home =
            city.createResidentialBuilding(
                "Home",
                50
            );

        auto work =
            city.createCommercialBuilding(
                "School",
                40
            );

        city.assignHome(
            citizen->getId(),
            home->getId()
        );

        city.assignWorkplace(
            citizen->getId(),
            work->getId()
        );

        city.moveCitizen(
            citizen->getId(),
            work->getId()
        );

        CitySerializer::saveToFile(
            city,
            "city_save.txt"
        );

        CityManager loaded;

        CitySerializer::loadFromFile(
            loaded,
            "city_save.txt"
        );

        auto loadedCitizen =
            loaded.getCitizens()
                  .at(citizen->getId());

        EXPECT_TRUE(
            loadedCitizen->getHome()
                != nullptr
        );

        EXPECT_TRUE(
            loadedCitizen->getWorkplace()
                != nullptr
        );

        EXPECT_TRUE(
            loadedCitizen->getLocation()
                != nullptr
        );
    });

    //--------------------------------------------------
    // 3. LARGE SAVE/LOAD
    //--------------------------------------------------
    runner.run("Large Save Load", []() {

        CityManager city;

        for (int i = 0; i < 2000; i++) {

            city.createCitizen(
                "User" + to_string(i),
                20,
                "X"
            );

            city.createCommercialBuilding(
                "Building" + to_string(i),
                10
            );
        }

        CitySerializer::saveToFile(
            city,
            "large_save.txt"
        );

        CityManager loaded;

        CitySerializer::loadFromFile(
            loaded,
            "large_save.txt"
        );

        EXPECT_EQ(
            loaded.getTotalCitizens(),
            2000
        );

        EXPECT_EQ(
            loaded.getTotalBuildings(),
            2000
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}