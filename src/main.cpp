#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "events/EventManager.h"
#include "events/ModelEvents.h"

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

        cout << "\n========== TEST REPORT ==========\n";

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

#define EXPECT_THROW(stmt) \
{ \
    bool thrown = false; \
    try { stmt; } \
    catch (...) { thrown = true; } \
    if (!thrown) \
        throw runtime_error( \
            "Expected exception not thrown" \
        ); \
}

int main() {

    TestRunner runner;

    //--------------------------------------------------
    // 1. EVENT CREATION
    //--------------------------------------------------
    runner.run("Event Creation", []() {

        CitizenCreatedEvent event(
            1,
            "Ivan"
        );

        EXPECT_EQ(
            event.getType(),
            "CitizenCreated"
        );

        EXPECT_TRUE(
            !event.getDescription().empty()
        );
    });

    //--------------------------------------------------
    // 2. TIMESTAMP VALIDITY
    //--------------------------------------------------
    runner.run("Timestamp Validity", []() {

        CitizenCreatedEvent event(
            1,
            "Ivan"
        );

        EXPECT_TRUE(
            event.getTimestamp() > 0
        );
    });

    //--------------------------------------------------
    // 3. EVENT MANAGER STORAGE
    //--------------------------------------------------
    runner.run("Event Manager Storage", []() {

        EventManager manager;

        manager.addEvent(
            make_shared<CitizenCreatedEvent>(
                1,
                "Ivan"
            )
        );

        EXPECT_EQ(
            manager.getEventCount(),
            1
        );
    });

    //--------------------------------------------------
    // 4. MULTIPLE EVENTS
    //--------------------------------------------------
    runner.run("Multiple Events", []() {

        EventManager manager;

        for (int i = 0; i < 1000; i++) {

            manager.addEvent(
                make_shared<CitizenCreatedEvent>(
                    i,
                    "User"
                )
            );
        }

        EXPECT_EQ(
            manager.getEventCount(),
            1000
        );
    });

    //--------------------------------------------------
    // 5. NULL EVENT REJECTION
    //--------------------------------------------------
    runner.run("Null Event Rejection", []() {

        EventManager manager;

        EXPECT_THROW(
            manager.addEvent(nullptr)
        );
    });

    //--------------------------------------------------
    // 6. CLEAR EVENTS
    //--------------------------------------------------
    runner.run("Clear Events", []() {

        EventManager manager;

        manager.addEvent(
            make_shared<CitizenCreatedEvent>(
                1,
                "Ivan"
            )
        );

        manager.clear();

        EXPECT_EQ(
            manager.getEventCount(),
            0
        );
    });

    //--------------------------------------------------
    // 7. POLYMORPHIC STORAGE
    //--------------------------------------------------
    runner.run("Polymorphic Storage", []() {

        EventManager manager;

        manager.addEvent(
            make_shared<CitizenCreatedEvent>(
                1,
                "Ivan"
            )
        );

        manager.addEvent(
            make_shared<BuildingCreatedEvent>(
                5,
                "Block A",
                "Residential"
            )
        );

        EXPECT_EQ(
            manager.getEventCount(),
            2
        );
    });

    //--------------------------------------------------
    // FINAL REPORT
    //--------------------------------------------------
    runner.report();

    return 0;
}