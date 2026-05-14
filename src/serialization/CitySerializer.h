#pragma once

#include <string>

#include "../core/CityManager.h"

using namespace std;

class CitySerializer {
public:

    static void saveToFile(
        const CityManager& city,
        const string& filename
    );

    static void loadFromFile(
        CityManager& city,
        const string& filename
    );
};
