#pragma once

#include <string>

#include "../core/CityManager.h"

// Handles persistence of the full city state to/from a plain-text file.
//
// File format:
//   [BUILDINGS]
//   <id>|<type>|<name>|<capacity>
//   ...
//   [CITIZENS]
//   <id>|<name>|<age>|<profession>|<homeId>|<workplaceId>|<locationId>
//   ...
//
// -1 is used as a sentinel for "no building assigned" in the citizen record.
//
// Buildings are saved first so that citizen relationships (which reference
// building IDs) can be fully reconstructed during loading.
//
// All methods are static — the serializer is stateless and holds no data.
class CitySerializer {
public:

    // Writes the city's full state to a file. Overwrites existing content.
    // Throws runtime_error if the file cannot be opened.
    static void saveToFile(
        const CityManager& city,
        const std::string& filename
    );

    // Reads a save file and populates a (typically empty) CityManager.
    // First restores buildings, then citizens, then re-links all relationships.
    // Throws runtime_error if the file is missing or malformed.
    static void loadFromFile(
        CityManager& city,
        const std::string& filename
    );
};
