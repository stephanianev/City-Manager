#include "CitySerializer.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <stdexcept>

using namespace std;

namespace {

// Splits a line on the given delimiter and returns the tokens.
// Standard getline-based split, with one special case: if the line ends with
// the delimiter (e.g. "1|Ivan|25||..." where profession is empty), getline
// does not produce a trailing empty token — but we need that token to
// correctly represent a citizen with no profession ("Jobless"). So we add
// an empty string manually in that case.
vector<string> split(
    const string& line,
    char delimiter
) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    if (!line.empty() && line.back() == delimiter) {
        tokens.push_back("");
    }

    return tokens;
}

int parseIntField(
    const string& text,
    const string& fieldName
) {
    size_t pos = 0;
    int value = 0;

    try {
        value = stoi(text, &pos);
    } catch (...) {
        throw runtime_error("Malformed " + fieldName + " in save file: '" + text + "'");
    }

    if (pos != text.size()) {
        throw runtime_error("Malformed " + fieldName + " in save file: '" + text + "'");
    }

    return value;
}

size_t parseSizeField(
    const string& text,
    const string& fieldName
) {
    size_t pos = 0;
    unsigned long long raw = 0;

    if (text.empty() || text.front() == '-') {
        throw runtime_error("Malformed " + fieldName + " in save file: '" + text + "'");
    }

    try {
        raw = stoull(text, &pos);
    } catch (...) {
        throw runtime_error("Malformed " + fieldName + " in save file: '" + text + "'");
    }

    if (pos != text.size()) {
        throw runtime_error("Malformed " + fieldName + " in save file: '" + text + "'");
    }

    if (raw > numeric_limits<size_t>::max()) {
        throw runtime_error("Out-of-range " + fieldName + " in save file");
    }

    return static_cast<size_t>(raw);
}

// Returns the building's ID, or -1 if the pointer is null.
// -1 is the save-file sentinel for "no building assigned".
int safeId(
    shared_ptr<Building> building
) {
    return building
        ? building->getId()
        : -1;
}

}

void CitySerializer::saveToFile(
    const CityManager& city,
    const string& filename
) {

    ofstream out(filename);

    if (!out) {
        throw runtime_error(
            "Failed to open file for saving"
        );
    }

    //--------------------------------------
    // BUILDINGS
    // Format: id|type|name|capacity
    // Buildings are written first so that citizen relationship IDs can be
    // resolved when the file is loaded back in.
    //--------------------------------------

    out << "[BUILDINGS]\n";

    for (const auto& [id, building] : city.getBuildings()) {

        out
            << building->getId() << "|"
            << building->getType() << "|"
            << building->getName() << "|"
            << building->getCapacity()
            << "\n";
    }

    //--------------------------------------
    // CITIZENS
    // Format: id|name|age|profession|homeId|workplaceId|locationId
    // -1 is used for homeId/workplaceId/locationId when not assigned.
    //--------------------------------------

    out << "[CITIZENS]\n";

    for (const auto& [id, citizen] : city.getCitizens()) {

        out
            << citizen->getId()       << "|"
            << citizen->getName()     << "|"
            << citizen->getAge()      << "|"
            << citizen->getProfession() << "|"
            << safeId(citizen->getHome())      << "|"
            << safeId(citizen->getWorkplace()) << "|"
            << safeId(citizen->getLocation())
            << "\n";
    }
}

void CitySerializer::loadFromFile(
    CityManager& city,
    const string& filename
) {

    ifstream in(filename);

    if (!in) {
        throw runtime_error(
            "Failed to open file for loading"
        );
    }

    //--------------------------------------
    // Deferred relationship restoration
    //
    // We cannot link citizens to buildings during the citizen parsing pass
    // because some referenced buildings might not have been parsed yet (though
    // in practice buildings come first in the file). Using a deferred list
    // makes the loader order-independent and safe.
    //--------------------------------------
    struct CitizenRelations {
        int citizenId;
        int homeId;
        int workplaceId;
        int locationId;
    };

    vector<CitizenRelations> relations;

    string line;

    enum class Section {
        None,
        Buildings,
        Citizens
    };

    Section currentSection = Section::None;

    //--------------------------------------
    // Parse file line by line
    //--------------------------------------

    while (getline(in, line)) {

        // Normalize Windows line endings (\r\n -> \n) before processing.
        if (!line.empty() && line.back() == '\r') line.pop_back();

        //----------------------------------
        // Section header detection
        //----------------------------------

        if (line == "[BUILDINGS]") {
            currentSection = Section::Buildings;
            continue;
        }

        if (line == "[CITIZENS]") {
            currentSection = Section::Citizens;
            continue;
        }

        //----------------------------------
        // Building record
        // Expected format: id|type|name|capacity  (4 fields)
        //----------------------------------

        if (currentSection == Section::Buildings) {

            auto parts = split(line, '|');

            if (parts.size() != 4) {
                throw runtime_error("Malformed building record in save file");
            }

            int id = parseIntField(parts[0], "building ID");

            if (id <= 0) {
                throw runtime_error("Building ID must be positive in save file");
            }

            string type     = parts[1];
            string name     = parts[2];
            size_t capacity = parseSizeField(parts[3], "building capacity");

            // Dispatch to the appropriate restore method based on the type string.
            if      (type == "Residential") city.restoreResidentialBuilding(id, name, capacity);
            else if (type == "Commercial")  city.restoreCommercialBuilding (id, name, capacity);
            else if (type == "Industrial")  city.restoreIndustrialBuilding (id, name, capacity);
            else if (type == "Service")     city.restoreServiceBuilding    (id, name, capacity);
            else throw runtime_error("Unknown building type: " + type);
        }

        //----------------------------------
        // Citizen record
        // Expected format: id|name|age|profession|homeId|workplaceId|locationId (7 fields)
        //----------------------------------

        else if (currentSection == Section::Citizens) {

            auto parts = split(line, '|');

            if (parts.size() != 7) {
                throw runtime_error("Malformed citizen record in save file");
            }

            int id = parseIntField(parts[0], "citizen ID");
            if (id <= 0) {
                throw runtime_error("Citizen ID must be positive in save file");
            }
            string name = parts[1];
            int age = parseIntField(parts[2], "citizen age");
            string profession  = parts[3];  // may be empty (Jobless)
            int homeId = parseIntField(parts[4], "home ID");
            int workplaceId = parseIntField(parts[5], "workplace ID");
            int locationId  = parseIntField(parts[6], "location ID");

            // Restore the citizen first (no relationships yet).
            city.restoreCitizen(id, name, age, profession);

            // Store the relationship IDs for the second pass below.
            relations.push_back({ id, homeId, workplaceId, locationId });
        }
    }

    //--------------------------------------
    // Second pass: re-link citizen <-> building relationships.
    // By this point all buildings and citizens are in the maps, so
    // assignHome / assignWorkplace / moveCitizen are safe to call.
    // IDs of -1 mean "not assigned" and are skipped.
    //--------------------------------------

    for (const auto& r : relations) {

        if (r.homeId != -1) {
            city.assignHome(r.citizenId, r.homeId);
        }

        if (r.workplaceId != -1) {
            city.assignWorkplace(r.citizenId, r.workplaceId);
        }

        if (r.locationId != -1) {
            city.moveCitizen(r.citizenId, r.locationId);
        }
    }
}