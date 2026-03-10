# Restaurant Management System (C)

## Overview
Restaurant Management System is a console application that models day-to-day restaurant operations: table assignment, menu management, order processing, and bill generation. It was built as a data-structures-focused project and organized as modular C code. The project demonstrates practical logic and state management in a small but realistic workflow.

## Problem It Solves
Small restaurants need a simple way to track table usage and customer orders while keeping billing accurate. This project centralizes that flow so staff can assign tables, create and process orders, and generate bills with clear status transitions.

## Architecture
Console Client -> Business Logic Modules -> In-Memory Data Structures

- Console client: interactive menu loop for operations.
- Business logic: table assignment, order queueing, billing, and menu CRUD.
- Persistence model: in-memory structures (hash table for menu items, circular list for tables, doubly linked list/queue for orders, linked list for bills).

## Tech Stack
- Language: C (C17-style, Visual Studio project)
- Platform: Windows console application
- IDE/Build: Visual Studio solution (`.sln`, `.vcxproj`)
- Core data structures: hash table, circular linked list, doubly linked list, queue

## Key Features
- Assign and release restaurant tables.
- Create orders with multiple menu items and quantities.
- Process pending orders in FIFO sequence.
- Generate bills from completed orders and calculate totals.
- Add, lookup, delete, and list menu items.
- Validate user input to reduce invalid states.

## Engineering Decisions
- In-memory data structures were chosen to emphasize algorithm/data-structure skills rather than external dependencies.
- Orders are tracked in both a sorted active list and a processing queue to support visibility and FIFO execution.
- Billing is gated behind order-status checks to prevent charging before processing is complete.
- Input validation is centralized into helper functions to keep menu actions simpler and more consistent.
- Cleanup logic explicitly frees allocated memory to demonstrate ownership and lifecycle handling in C.

## Project Structure
- `RestaurantManagementSystem/RestaurantManagement.c`: application entry point and main business logic.
- `RestaurantManagementSystem/RestaurantManagement.h`: shared models, enums, constants, and function declarations.
- `RestaurantManagementSystem/RestaurantManagementSystem.sln`: Visual Studio solution file.
- `docs/images/`: runtime screenshots or console demo artifacts.

## How to Run
1. Open `RestaurantManagementSystem/RestaurantManagementSystem.sln` in Visual Studio 2022 (or compatible).
2. Select `Debug | x64` (or another valid configuration).
3. Build the solution.
4. Run the console application and follow the on-screen menu.

## Visual Demonstration
This is a console-first project. The following artifacts were captured by running the compiled executable with scripted inputs:

- [Startup and graceful exit](docs/images/run-exit-demo.txt)
- [Validation: non-numeric table input](docs/images/run-validation-text-input.txt)
- [Validation: minimum table count enforcement](docs/images/run-validation-table-count.txt)

You can replace these with manual screenshots from a local interactive run in Visual Studio.

## Why This Project Stands Out
- Demonstrates deliberate use of multiple core data structures in one coherent workflow.
- Shows practical engineering habits: modular organization, input validation, and explicit memory management.
- Represents a realistic student scope: small, understandable, and technically meaningful.

## License
MIT License. See [LICENSE](LICENSE).
