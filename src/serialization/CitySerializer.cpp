#include "CitySerializer.h"

#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

namespace {

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

    return tokens;
}

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
    //--------------------------------------

    out << "[BUILDINGS]\n";

    for (const auto& [id, building]
         : city.getBuildings()) {

        out
            << building->getId() << "|"
            << building->getType() << "|"
            << building->getName() << "|"
            << building->getCapacity()
            << "\n";
    }

    //--------------------------------------
    // CITIZENS
    //--------------------------------------

    out << "[CITIZENS]\n";

    for (const auto& [id, citizen]
         : city.getCitizens()) {

        out
            << citizen->getId() << "|"
            << citizen->getName() << "|"
            << citizen->getAge() << "|"
            << citizen->getProfession() << "|"
            << safeId(citizen->getHome()) << "|"
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
    // Temporary relationship storage
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

    Section currentSection =
        Section::None;

    //--------------------------------------
    // Parse file
    //--------------------------------------

    while (getline(in, line)) {

        if (line.empty()) {
            continue;
        }

        //----------------------------------
        // Section headers
        //----------------------------------

        if (line == "[BUILDINGS]") {
            currentSection =
                Section::Buildings;
            continue;
        }

        if (line == "[CITIZENS]") {
            currentSection =
                Section::Citizens;
            continue;
        }

        //----------------------------------
        // BUILDINGS
        //----------------------------------

        if (currentSection ==
            Section::Buildings) {

            auto parts = split(line, '|');

            if (parts.size() != 4) {
                throw runtime_error("Malformed building record in save file");
            }

            int id =
                stoi(parts[0]);

            string type =
                parts[1];

            string name =
                parts[2];

            size_t capacity =
                stoull(parts[3]);

            if (type == "Residential") {

                city.restoreResidentialBuilding(
                    id,
                    name,
                    capacity
                );
            }
            else if (type == "Commercial") {

                city.restoreCommercialBuilding(
                    id,
                    name,
                    capacity
                );
            }
            else if (type == "Industrial") {

                city.restoreIndustrialBuilding(
                    id,
                    name,
                    capacity
                );
            }
            else if (type == "Service") {

                city.restoreServiceBuilding(
                    id,
                    name,
                    capacity
                );
            }
        }

        //----------------------------------
        // CITIZENS
        //----------------------------------

        else if (currentSection ==
                 Section::Citizens) {

            auto parts = split(line, '|');

            if (parts.size() != 7) {
                throw runtime_error("Malformed citizen record in save file");
            }

            int id =
                stoi(parts[0]);

            string name =
                parts[1];

            int age =
                stoi(parts[2]);

            string profession =
                parts[3];

            int homeId =
                stoi(parts[4]);

            int workplaceId =
                stoi(parts[5]);

            int locationId =
                stoi(parts[6]);

            city.restoreCitizen(
                id,
                name,
                age,
                profession
            );

            relations.push_back({
                id,
                homeId,
                workplaceId,
                locationId
            });
        }
    }

    //--------------------------------------
    // Reconstruct relationships
    //--------------------------------------

    for (const auto& r : relations) {

        if (r.homeId != -1) {

            city.assignHome(
                r.citizenId,
                r.homeId
            );
        }

        if (r.workplaceId != -1) {

            city.assignWorkplace(
                r.citizenId,
                r.workplaceId
            );
        }

        if (r.locationId != -1) {

            city.moveCitizen(
                r.citizenId,
                r.locationId
            );
        }
    }
}