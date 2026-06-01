# City Manager

## Overview

City Manager is an object-oriented city simulation and management system developed as a university project focused on the practical application of Object-Oriented Programming (OOP) principles. The system models a simplified city environment consisting of citizens, buildings, transportation infrastructure, resources, and events. It provides a centralized management layer that coordinates interactions between all entities while maintaining clear separation of responsibilities.

The project was designed with extensibility, maintainability, and architectural clarity in mind. Rather than focusing on graphical visualization, the system emphasizes domain modeling, business logic, and clean software design.

The simulation allows the creation and management of city entities, assignment of homes and workplaces, movement of citizens between locations, resource tracking, event logging, and persistence through file serialization.

---

## Project Goals

The primary goals of the project are:

* Demonstrate the four fundamental OOP principles:

  * Encapsulation
  * Inheritance
  * Polymorphism
  * Abstraction

* Apply object-oriented design in a realistic domain.

* Design a modular architecture with clearly separated responsibilities.

* Implement a scalable system that can be extended with additional building types, event types, resources, or simulation rules.

* Provide reliable data persistence through serialization and deserialization.

---

## Core Functionality

### City Management

The system maintains a city containing all managed entities. The city acts as the central environment in which all operations occur.

Supported operations include:

* Creating and managing city entities
* Tracking citizens and buildings
* Coordinating movement and assignments
* Managing events and statistics
* Saving and loading city state

---

### Building Management

The system supports multiple building types with specialized behavior.

Available building categories:

* Residential Building
* Commercial Building
* Industrial Building
* Service Building

Each building has:

* Unique identifier
* Name
* Capacity
* Occupancy tracking

Buildings enforce capacity constraints and may implement different acceptance rules for citizens.

---

### Citizen Management

Citizens represent the population of the city.

Each citizen stores:

* Unique identifier
* Name
* Age
* Profession
* Home assignment
* Workplace assignment
* Current location

The system supports:

* Creating citizens
* Updating citizen information
* Assigning homes
* Assigning workplaces
* Moving citizens between buildings
* Removing citizens

---

### Transportation and Movement

Although transportation infrastructure is simplified, the system models citizen movement between buildings.

Movement operations:

* Validate destination capacity
* Validate destination acceptance rules
* Update physical occupancy
* Generate movement events

The distinction between logical relationships and physical presence is maintained:

* Home and workplace assignments are logical relationships.
* Current location represents physical presence.

---

### Event System

The project includes a dedicated event management subsystem.

Every significant operation generates an event, creating an auditable history of city activity.

Examples include:

* Citizen creation
* Citizen removal
* Citizen movement
* Home assignment
* Workplace assignment
* Building creation
* Building removal

Events are stored and can be reviewed for debugging, monitoring, or reporting purposes.

---

### Data Persistence

The city state can be stored and restored using serialization.

The persistence system supports:

* Saving buildings
* Saving citizens
* Saving relationships
* Restoring object references
* Preserving original IDs

Validation is performed during loading to prevent malformed save files from producing invalid application state.

---

## System Architecture

The project follows a layered architecture.

### Domain Layer

Contains the core business entities:

* Citizen
* Building
* ResidentialBuilding
* CommercialBuilding
* IndustrialBuilding
* ServiceBuilding

These classes model the real-world concepts of the city.

---

### Management Layer

Contains coordination and orchestration logic.

Main class:

* CityManager

Responsibilities:

* Entity lifecycle management
* Validation enforcement
* Relationship management
* Event generation
* Simulation coordination

The CityManager acts as the central facade through which all modifications occur.

---

### Event Layer

Contains classes responsible for event tracking.

Main components:

* Event
* EventManager
* Specialized event types

The event subsystem follows an inheritance hierarchy that allows new event categories to be introduced with minimal modifications.

---

### Persistence Layer

Responsible for saving and loading city state.

Main component:

* CitySerializer

Responsibilities:

* Serialize entities
* Deserialize entities
* Restore relationships
* Validate loaded data

---

### Validation Layer

Responsible for input validation.

Main component:

* Validation

Responsibilities:

* Name validation
* Age validation
* Capacity validation
* Identifier validation

This ensures that invalid data cannot enter the system through either user operations or file loading.

---

## Object-Oriented Design

### Encapsulation

All entities protect their internal state through private or protected members.

Examples:

* Citizen data is modified through controlled methods.
* Building occupancy cannot be modified directly by external code.
* Validation is enforced through dedicated interfaces.

This prevents invalid state transitions and preserves class invariants.

---

### Inheritance

Inheritance is used to represent specialized variants of common concepts.

Building hierarchy:

Building
├── ResidentialBuilding
├── CommercialBuilding
├── IndustrialBuilding
└── ServiceBuilding

Event hierarchy:

Event
├── CitizenCreatedEvent
├── CitizenRemovedEvent
├── CitizenMovedEvent
├── HomeAssignedEvent
├── WorkplaceAssignedEvent
└── BuildingCreatedEvent
(and additional event types)

Inheritance eliminates code duplication and promotes extensibility.

---

### Polymorphism

The system uses polymorphism through abstract base classes.

Examples:

* Buildings can implement different acceptance behavior.
* Events provide specialized descriptions.
* Collections can store pointers to base types while operating on derived objects.

This allows the system to remain open for extension without modifying existing code.

---

### Abstraction

Abstract classes define common interfaces while hiding implementation details.

Examples:

* Building
* Event

These classes capture the shared characteristics of their derived types while preventing meaningless direct instantiation.

---

## Design Decisions

### Why Building Is Abstract

A generic building has no practical meaning within the simulation.

Only specific building types such as residential or industrial buildings can exist. Making Building abstract enforces this rule at the type level.

---

### Why Event Is Abstract

Different events contain different contextual information but share common event behavior.

The abstract Event class provides a common interface while allowing specialized event implementations.

---

### Why CityManager Centralizes Operations

Allowing arbitrary classes to modify relationships directly would make it difficult to maintain consistency.

CityManager serves as the single authority responsible for:

* Validation
* Relationship updates
* Event generation

This significantly reduces the risk of inconsistent state.

---

### Why Weak References Are Used

Citizens reference buildings and buildings reference citizens.

Using only shared ownership would create cyclic dependencies and memory leaks.

The project uses weak references where appropriate to break ownership cycles while still allowing relationships to be represented safely.

---

### Why Validation Is Centralized

Validation rules are implemented in a dedicated utility rather than duplicated across the system.

Benefits include:

* Consistency
* Reusability
* Easier maintenance
* Reduced duplication

---

## Error Handling

The project uses exception-based error handling.

Operations may fail due to:

* Invalid identifiers
* Invalid names
* Invalid capacities
* Missing entities
* Full buildings
* Malformed save files

Exceptions ensure that errors are detected immediately and cannot silently corrupt application state.

---

## Extensibility

The architecture was intentionally designed to support future expansion.

Potential extensions include:

* Transportation networks
* Route planning
* Resource management
* Budget simulation
* Utility systems
* Traffic simulation
* Disaster management
* Population growth
* Advanced statistics
* Real-time simulation

Most of these features can be added without significant modifications to the existing architecture.

---

## Technologies

* C++17
* Standard Template Library (STL)
* Smart pointers (`shared_ptr`, `weak_ptr`)
* Object-Oriented Design Principles
* File Serialization

---

## Educational Value

This project serves as a comprehensive demonstration of object-oriented software design.

It showcases:

* Class design
* Inheritance hierarchies
* Polymorphic behavior
* Ownership management
* Serialization
* Validation
* Event-driven architecture
* Separation of concerns

The resulting system provides a solid foundation for further development while remaining sufficiently complex to demonstrate practical software engineering principles.

---

## In short

City Manager is a modular, extensible, and object-oriented city simulation system designed to demonstrate clean architecture and proper application of OOP concepts. Through the interaction of citizens, buildings, events, and management services, the project models a simplified city while maintaining strong encapsulation, clear responsibility boundaries, and long-term maintainability.
