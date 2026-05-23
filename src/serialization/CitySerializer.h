#pragma once

#include <string>

#include "../core/CityManager.h"

class CitySerializer {
public:

    static void saveToFile(
        const CityManager& city,
        const std::string& filename
    );

    static void loadFromFile(
        CityManager& city,
        const std::string& filename
    );
};
