#pragma once

#include <string>

using namespace std;

class Validation {
public:

    static void validateCitizenName(
        const string& name
    );

    static void validateCitizenAge(
        int age
    );

    static void validateBuildingName(
        const string& name
    );

    static void validateBuildingCapacity(
        size_t capacity
    );
};