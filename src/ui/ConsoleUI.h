#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "../core/CityManager.h"

class ConsoleUI {
public:
    ConsoleUI();
    void run();

private:
    std::unique_ptr<CityManager> city;
    bool running;

    // Main loop helpers
    void printMainMenu() const;
    void resetCity();
    void ensureCity();

    // Submenus
    void citizenMenu();
    void buildingMenu();
    void relationshipMenu();
    void movementMenu();
    void queryMenu();
    void statsMenu();
    void eventMenu();
    void persistenceMenu();
    void demoScenario();

    // Input helpers
    std::string promptLine(const std::string& prompt, bool allowEmpty = false) const;
    int promptInt(const std::string& prompt) const;
    size_t promptSize(const std::string& prompt) const;
    double promptDouble(const std::string& prompt) const;
    int promptChoice(const std::string& prompt, int minChoice, int maxChoice) const;
    void pause() const;

    // Display helpers
    static std::string trim(const std::string& value);
    static std::string formatPercent(double value);
    static std::string buildingRef(const std::shared_ptr<Building>& building);
    static std::string professionLabel(const std::string& profession);

    void printCitizen(const std::shared_ptr<Citizen>& citizen) const;
    void printBuilding(const std::shared_ptr<Building>& building) const;
    void printCitizenList(const std::vector<std::shared_ptr<Citizen>>& citizens) const;
    void printBuildingList(const std::vector<std::shared_ptr<Building>>& buildings) const;
    void printDistribution(
        const std::unordered_map<std::string, int>& data,
        const std::string& title
    ) const;
    void printDistribution(
        const std::unordered_map<std::string, double>& data,
        const std::string& title
    ) const;
    void printOccupancyReport() const;
    void printGeneralStats() const;
    void printAllState() const;
    void printEvents(size_t limit = 50) const;
};
