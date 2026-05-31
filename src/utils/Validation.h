#pragma once

#include <string>

// Static utility class for input validation shared between CityManager and
// the model constructors.
//
// Why validate in two places?
//   - Validation:: is called by CityManager before constructing objects, so
//     invalid data never reaches the constructors at all during normal use.
//   - The constructors also guard themselves because they can be exercised
//     directly in restore/test paths, so they should never silently accept
//     bad input.
//
// The forbidden-character check (pipe, newline, carriage-return) is
// specifically to protect the pipe-delimited save format in CitySerializer:
// allowing these characters would corrupt the file and break loading.
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