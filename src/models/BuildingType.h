#pragma once

// Enum used throughout the system to identify a building's category without
// relying on string comparisons or dynamic_cast. Each subclass returns its
// corresponding value from getBuildingType(), which lets CityManager enforce
// business rules (e.g. only Residential buildings can be assigned as homes,
// non-Residential buildings can be workplaces).
enum class BuildingType {
    Residential,
    Commercial,
    Industrial,
    Service
};