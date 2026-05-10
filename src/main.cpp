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

            if (r.passed) passed++;
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
    // 1. BASIC REMOVE
    //--------------------------------------------------
    runner.run("Basic Citizen Removal", []() {

        CityManager city;

        auto c = city.createCitizen(
            "Ivan",
            25,
            "Engineer"
        );

        EXPECT_EQ(city.getTotalCitizens(), 1);

        city.removeCitizen(c->getId());

        EXPECT_EQ(city.getTotalCitizens(), 0);
    });

    //--------------------------------------------------
    // 2. REMOVE INVALID ID
    //--------------------------------------------------
    runner.run("Remove Invalid Citizen", []() {

        CityManager city;

        EXPECT_THROW(
            city.removeCitizen(999)
        );
    });

    //--------------------------------------------------
    // 3. REMOVE TWICE
    //--------------------------------------------------
    runner.run("Remove Same Citizen Twice", []() {

        CityManager city;

        auto c = city.createCitizen(
            "Ivan",
            20,
            "X"
        );

        city.removeCitizen(c->getId());

        EXPECT_THROW(
            city.removeCitizen(c->getId())
        );
    });

    //--------------------------------------------------
    // 4. MULTIPLE CITIZENS
    //--------------------------------------------------
    runner.run("Remove One From Many", []() {

        CityManager city;

        auto c1 = city.createCitizen("A", 20, "X");
        auto c2 = city.createCitizen("B", 20, "X");
        auto c3 = city.createCitizen("C", 20, "X");

        city.removeCitizen(c2->getId());

        EXPECT_EQ(city.getTotalCitizens(), 2);
    });

    //--------------------------------------------------
    // 5. ID CONTINUITY AFTER REMOVAL
    //--------------------------------------------------
    runner.run("ID Continuity After Removal", []() {

        CityManager city;

        auto c1 = city.createCitizen("A", 20, "X");

        city.removeCitizen(c1->getId());

        auto c2 = city.createCitizen("B", 20, "X");

        EXPECT_EQ(c2->getId(), 2);
    });

    //--------------------------------------------------
    // 6. STRESS REMOVE
    //--------------------------------------------------
    runner.run("Stress Remove 1000 Citizens", []() {

        CityManager city;

        vector<int> ids;

        for (int i = 0; i < 1000; i++) {

            auto c = city.createCitizen(
                "User",
                20,
                "X"
            );

            ids.push_back(c->getId());
        }

        for (int id : ids) {
            city.removeCitizen(id);
        }

        EXPECT_EQ(city.getTotalCitizens(), 0);
    });

    //--------------------------------------------------
    // 7. REMOVE FROM EMPTY SYSTEM
    //--------------------------------------------------
    runner.run("Remove From Empty System", []() {

        CityManager city;

        EXPECT_THROW(
            city.removeCitizen(1)
        );
    });

    //--------------------------------------------------
    // 8. LARGE IDS TEST
    //--------------------------------------------------
    runner.run("Large Amount Creation Then Remove", []() {

        CityManager city;

        vector<int> ids;

        for (int i = 0; i < 5000; i++) {

            auto c = city.createCitizen(
                "User" + to_string(i),
                20,
                "X"
            );

            ids.push_back(c->getId());
        }

        for (int i = 0; i < 2500; i++) {
            city.removeCitizen(ids[i]);
        }

        EXPECT_EQ(city.getTotalCitizens(), 2500);
    });

    //--------------------------------------------------
    // 9. BUILDING CREATION
    //--------------------------------------------------
    runner.run("Basic Building Creation", []() {

        CityManager city;

        auto b =
            city.createResidentialBuilding(
                "Block A",
                10
            );

        EXPECT_TRUE(b != nullptr);

        EXPECT_EQ(
            city.getTotalBuildings(),
            1
        );

        EXPECT_EQ(
            b->getType(),
            "Residential"
        );
    });

    //--------------------------------------------------
    // 10. INVALID BUILDINGS
    //--------------------------------------------------
    runner.run("Invalid Building Validation", []() {

        CityManager city;

        EXPECT_THROW(
            city.createResidentialBuilding(
                "",
                10
            )
        );

        EXPECT_THROW(
            city.createResidentialBuilding(
                "   ",
                10
            )
        );

        EXPECT_THROW(
            city.createResidentialBuilding(
                "Block",
                0
            )
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}