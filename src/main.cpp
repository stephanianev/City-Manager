#include <iostream>
#include <cassert>

#include "core/CityManager.h"

using namespace std;

int main() {
    CityManager city;

    cout << "=== TEST 1 ===\n";
    auto c = city.createCitizen("Ivan", 25, "Engineer");
    assert(c->getId() == 1);

    cout << "=== TEST 2 ===\n";
    auto c1 = city.createCitizen("Anna", 30, "Doctor");
    auto c2 = city.createCitizen("Peter", 40, "Teacher");

    assert(c1->getId() == 2);
    assert(c2->getId() == 3);

    cout << "=== TEST 3 ===\n";
    assert(city.getTotalCitizens() == 3);

    cout << "=== TEST 4 ===\n";
    try {
        city.createCitizen("", 20, "X");
        assert(false);
    } catch (...) {}

    cout << "=== TEST 5 ===\n";
    try {
        city.createCitizen("   ", 20, "X");
        assert(false);
    } catch (...) {}

    cout << "=== TEST 6 ===\n";
    try {
        city.createCitizen("Test", -1, "X");
        assert(false);
    } catch (...) {}

    cout << "=== TEST 7 ===\n";
    auto c3 = city.createCitizen("NoJob", 22, "");
    assert(c3->getProfession() == "");

    cout << "=== TEST 8 ===\n";
    auto c4 = city.createCitizen("Next", 50, "X");
    assert(c4->getId() == 5);

    cout << "\nALL TESTS PASSED\n";
}