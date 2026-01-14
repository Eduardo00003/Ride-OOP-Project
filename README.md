# Ride OOP

Small C++17 console simulation of a ride-sharing service that demonstrates object-oriented design with interchangeable pricing and driver-dispatch strategies.

## Project structure
- `RideSharing.h` / `RideSharing.cpp`: Core domain types (Location, Rider, Driver) plus pricing and dispatch strategy implementations and the `RideService` orchestrator.
- `main.cpp`: Sets up sample drivers and riders, swaps pricing/dispatch strategies, and prints trip summaries.
- `Makefile`: Simple build and clean targets for the `rideshare` binary.

## Build and run
Requirements: `g++` with C++17 support and `make`.

```bash
# Build
make

# Run
./rideshare

# Clean build artifacts
make clean
```

If you prefer compiling without `make`:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp RideSharing.cpp -o rideshare
./rideshare
```
