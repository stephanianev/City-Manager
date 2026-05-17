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
    // 1. QUERY BY PROFESSION
    //--------------------------------------------------
    runner.run(
        "Query By Profession",
        []() {

        CityManager city;

        city.createCitizen(
            "Ivan",
            25,
            "Engineer"
        );

        city.createCitizen(
            "Maria",
            30,
            "Teacher"
        );

        auto result =
            city.queryCitizens(
                [](const shared_ptr<Citizen>& c) {

                    return c->getProfession()
                           == "Engineer";
                }
            );

        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(
            result[0]->getName(),
            "Ivan"
        );
    });

    //--------------------------------------------------
    // 2. QUERY BY AGE
    //--------------------------------------------------
    runner.run(
        "Query By Age",
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
            "Y"
        );

        auto result =
            city.queryCitizens(
                [](const shared_ptr<Citizen>& c) {

                    return c->getAge() >= 30;
                }
            );

        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(
            result[0]->getName(),
            "B"
        );
    });

    //--------------------------------------------------
    // 3. COMPLEX QUERY
    //--------------------------------------------------
    runner.run(
        "Complex Citizen Query",
        []() {

        CityManager city;

        city.createCitizen(
            "Ivan",
            35,
            "Engineer"
        );

        city.createCitizen(
            "Maria",
            25,
            "Engineer"
        );

        city.createCitizen(
            "Peter",
            40,
            "Teacher"
        );

        auto result =
            city.queryCitizens(
                [](const shared_ptr<Citizen>& c) {

                    return
                        c->getProfession()
                            == "Engineer"

                        &&

                        c->getAge() > 30;
                }
            );

        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(
            result[0]->getName(),
            "Ivan"
        );
    });

    //--------------------------------------------------
    // 4. QUERY BUILDINGS
    //--------------------------------------------------
    runner.run(
        "Query Buildings",
        []() {

        CityManager city;

        city.createResidentialBuilding(
            "Block A",
            100
        );

        city.createCommercialBuilding(
            "Mall",
            50
        );

        auto result =
            city.queryBuildings(
                [](const shared_ptr<Building>& b) {

                    return b->getType()
                           == "Commercial";
                }
            );

        EXPECT_EQ(result.size(), 1);
    });

    //--------------------------------------------------
    // 5. EMPTY RESULT
    //--------------------------------------------------
    runner.run(
        "Empty Query Result",
        []() {

        CityManager city;

        city.createCitizen(
            "Ivan",
            20,
            "Student"
        );

        auto result =
            city.queryCitizens(
                [](const shared_ptr<Citizen>& c) {

                    return c->getAge() > 100;
                }
            );

        EXPECT_TRUE(result.empty());
    });

    //--------------------------------------------------
    // 6. ALL MATCH
    //--------------------------------------------------
    runner.run(
        "All Match Query",
        []() {

        CityManager city;

        for (int i = 0; i < 100; i++) {

            city.createCitizen(
                "User",
                20,
                "X"
            );
        }

        auto result =
            city.queryCitizens(
                [](const shared_ptr<Citizen>&) {

                    return true;
                }
            );

        EXPECT_EQ(result.size(), 100);
    });

    //--------------------------------------------------
    // 7. STRESS QUERY
    //--------------------------------------------------
    runner.run(
        "Stress Query",
        []() {

        CityManager city;

        for (int i = 0; i < 10000; i++) {

            city.createCitizen(
                "User",
                i % 100,
                "Worker"
            );
        }

        auto result =
            city.queryCitizens(
                [](const shared_ptr<Citizen>& c) {

                    return c->getAge() >= 50;
                }
            );

        EXPECT_TRUE(!result.empty());
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}