// Simple ride-sharing simulation showcasing OOP with pricing and dispatch polymorphism.
#include "RideSharing.h"

#include <iomanip>
#include <iostream>
#include <memory>

void printTrip(const Trip& trip) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Trip #" << trip.id << " (" << trip.pricingModel << ", " << trip.dispatchModel
              << ")\n";
    std::cout << "  Rider: " << trip.rider->name() << " -> Driver: " << trip.driver->name() << "\n";
    std::cout << "  Distance: " << trip.distanceKm << " km, Duration: " << trip.durationMinutes
              << " min, Fare: $" << trip.fare << "\n\n";
}

void printDriverSummary(const RideService& service) {
    std::cout << "Drivers:\n";
    for (const auto& driver : service.drivers()) {
        std::cout << "  #" << driver.id() << " " << driver.name() << " | Rating: " << driver.rating()
                  << " | Position: (" << driver.position().x << ", " << driver.position().y << ")\n";
    }
    std::cout << "\n";
}

int main() {
    RideService service(std::make_unique<StandardPricing>(),
                        std::make_unique<NearestDriverDispatch>());

    service.addDriver(Driver{1, "Maya", 4.98, Location{1.0, 2.0}});
    service.addDriver(Driver{2, "Leo", 4.67, Location{5.0, 1.0}});
    service.addDriver(Driver{3, "Amina", 4.85, Location{3.0, 4.0}});

    printDriverSummary(service);

    Rider rider1{1, "Alex", Location{0.0, 0.0}, Location{4.0, 3.0}};
    if (auto trip = service.requestTrip(rider1)) {
        printTrip(*trip);
    } else {
        std::cout << "No driver available for " << rider1.name() << "\n\n";
    }

    // Switch to surge pricing and highest-rated dispatch to demonstrate polymorphism.
    service.setPricingStrategy(std::make_unique<SurgePricing>(1.8));
    service.setDispatchStrategy(std::make_unique<HighestRatedDispatch>());

    Rider rider2{2, "Sam", Location{10.0, 5.0}, Location{2.0, 1.0}};
    if (auto trip = service.requestTrip(rider2)) {
        printTrip(*trip);
    } else {
        std::cout << "No driver available for " << rider2.name() << "\n\n";
    }

    // Eco pricing encourages shared/green rides; still nearest driver for a shorter hop.
    service.setPricingStrategy(std::make_unique<EcoPricing>());
    service.setDispatchStrategy(std::make_unique<NearestDriverDispatch>());

    Rider rider3{3, "Jamie", Location{2.5, 2.0}, Location{2.0, 2.2}};
    if (auto trip = service.requestTrip(rider3)) {
        printTrip(*trip);
    } else {
        std::cout << "No driver available for " << rider3.name() << "\n\n";
    }

    printDriverSummary(service);

    return 0;
}
