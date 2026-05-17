#include "Validation.h"

#include <stdexcept>
#include <algorithm>
#include <cctype>

using namespace std;

namespace {

bool isWhitespaceOnly(
    const string& str
) {

    return all_of(
        str.begin(),
        str.end(),
        [](unsigned char c) {

            return isspace(c);
        }
    );
}

}

void Validation::validateCitizenName(
    const string& name
) {

    if (name.empty()
        || isWhitespaceOnly(name)) {

        throw invalid_argument(
            "Citizen name cannot be empty."
        );
    }
}

void Validation::validateCitizenAge(
    int age
) {

    if (age < 0 || age > 120) {

        throw invalid_argument(
            "Invalid citizen age."
        );
    }
}

void Validation::validateBuildingName(
    const string& name
) {

    if (name.empty()
        || isWhitespaceOnly(name)) {

        throw invalid_argument(
            "Building name cannot be empty."
        );
    }
}

void Validation::validateBuildingCapacity(
    size_t capacity
) {

    if (capacity == 0) {

        throw invalid_argument(
            "Building capacity must be positive."
        );
    }
}