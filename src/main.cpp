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
        } catch (const exception& e) {
            results.push_back({name, false, e.what()});
        } catch (...) {
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
             << passed << "/" << results.size()
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
    // 1. BASIC CREATION
    //--------------------------------------------------
    runner.run("Basic Creation", []() {
        CityManager city;

        auto c = city.createCitizen("Ivan", 25, "Engineer");

        EXPECT_TRUE(c != nullptr);
        EXPECT_EQ(c->getId(), 1);
        EXPECT_EQ(c->getName(), "Ivan");
        EXPECT_EQ(c->getProfession(), "Engineer");
    });

    //--------------------------------------------------
    // 2. ID SEQUENCE
    //--------------------------------------------------
    runner.run("ID Sequence", []() {
        CityManager city;

        auto c1 = city.createCitizen("A", 20, "X");
        auto c2 = city.createCitizen("B", 30, "Y");

        EXPECT_EQ(c1->getId(), 1);
        EXPECT_EQ(c2->getId(), 2);
    });

    //--------------------------------------------------
    // 3. STATE INTEGRITY AFTER FAILURE
    //--------------------------------------------------
    runner.run("State Integrity After Failure", []() {
        CityManager city;

        city.createCitizen("Valid", 20, "X");

        EXPECT_THROW(city.createCitizen("", 20, "X"));
        EXPECT_THROW(city.createCitizen("   ", 20, "X"));
        EXPECT_THROW(city.createCitizen("Test", -1, "X"));
        EXPECT_THROW(city.createCitizen("Test", 200, "X"));

        EXPECT_EQ(city.getTotalCitizens(), 1);
    });

    //--------------------------------------------------
    // 4. ID CONTINUITY AFTER FAILURES
    //--------------------------------------------------
    runner.run("ID Continuity After Failure", []() {
        CityManager city;

        city.createCitizen("A", 20, "X");

        EXPECT_THROW(city.createCitizen("", 20, "X"));

        auto c = city.createCitizen("B", 30, "Y");

        EXPECT_EQ(c->getId(), 2);
    });

    //--------------------------------------------------
    // 5. BOUNDARY AGES
    //--------------------------------------------------
    runner.run("Boundary Ages", []() {
        CityManager city;

        auto c1 = city.createCitizen("Baby", 0, "None");
        auto c2 = city.createCitizen("Old", 120, "Retired");

        EXPECT_EQ(c1->getAge(), 0);
        EXPECT_EQ(c2->getAge(), 120);
    });

    //--------------------------------------------------
    // 6. EMPTY PROFESSION
    //--------------------------------------------------
    runner.run("Empty Profession Allowed", []() {
        CityManager city;

        auto c = city.createCitizen("NoJob", 25, "");

        EXPECT_EQ(c->getProfession(), "");
    });

    //--------------------------------------------------
    // 7. LARGE INPUT STRINGS
    //--------------------------------------------------
    runner.run("Large Name Input", []() {
        CityManager city;

        string longName(10000, 'A');

        auto c = city.createCitizen(longName, 30, "X");

        EXPECT_EQ(c->getName(), longName);
    });

    //--------------------------------------------------
    // 8. MANY CITIZENS (STRESS)
    //--------------------------------------------------
    runner.run("Stress Test 10000 Citizens", []() {
        CityManager city;

        for (int i = 0; i < 10000; i++) {
            city.createCitizen("User" + to_string(i), 20, "X");
        }

        EXPECT_EQ(city.getTotalCitizens(), 10000);
    });

    //--------------------------------------------------
    // 9. DUPLICATE DATA (ALLOWED)
    //--------------------------------------------------
    runner.run("Duplicate Names Allowed", []() {
        CityManager city;

        auto c1 = city.createCitizen("Ivan", 20, "X");
        auto c2 = city.createCitizen("Ivan", 20, "X");

        EXPECT_TRUE(c1->getId() != c2->getId());
    });

    //--------------------------------------------------
    // 10. WHITESPACE EDGE CASES
    //--------------------------------------------------
    runner.run("Whitespace Edge Cases", []() {
        CityManager city;

        EXPECT_THROW(city.createCitizen("\t\n", 20, "X"));
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();
}