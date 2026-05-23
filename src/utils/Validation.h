#pragma once

#include <string>

class Validation {
public:

    static void validateCitizenName(
        const std::string& name
    );

    static void validateCitizenAge(
        int age
    );

    static void validateBuildingName(
        const std::string& name
    );

    static void validateBuildingCapacity(
        size_t capacity
    );
};