#include "RideSharing.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>

double Location::distanceTo(const Location& other) const {
    const double dx = x - other.x;
    const double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

Rider::Rider(int id, std::string name, Location pickup, Location dropoff)
    : id_(id), name_(std::move(name)), pickup_(pickup), dropoff_(dropoff) {}

int Rider::id() const { return id_; }
const std::string& Rider::name() const { return name_; }
const Location& Rider::pickup() const { return pickup_; }
const Location& Rider::dropoff() const { return dropoff_; }

Driver::Driver(int id, std::string name, double rating, Location position)
    : id_(id), name_(std::move(name)), rating_(rating), position_(position) {}

int Driver::id() const { return id_; }
const std::string& Driver::name() const { return name_; }
double Driver::rating() const { return rating_; }
bool Driver::available() const { return available_; }
const Location& Driver::position() const { return position_; }

void Driver::setAvailable(bool value) { available_ = value; }

void Driver::moveTo(const Location& destination) { position_ = destination; }

double StandardPricing::calculateFare(double distanceKm, double minutes) const {
    const double baseFare = 2.50;
    const double perKm = 1.25;
    const double perMinute = 0.35;
    return baseFare + (distanceKm * perKm) + (minutes * perMinute);
}

std::string StandardPricing::name() const { return "Standard"; }

SurgePricing::SurgePricing(double surgeMultiplier) : surgeMultiplier_(surgeMultiplier) {}

double SurgePricing::calculateFare(double distanceKm, double minutes) const {
    StandardPricing standard;
    return standard.calculateFare(distanceKm, minutes) * surgeMultiplier_;
}

std::string SurgePricing::name() const { return "Surge x" + std::to_string(surgeMultiplier_); }

double EcoPricing::calculateFare(double distanceKm, double minutes) const {
    StandardPricing standard;
    const double fare = standard.calculateFare(distanceKm, minutes);
    const double discount = 0.10;  // Encourage eco rides
    const double minimumFare = 5.00;
    return std::max(minimumFare, fare * (1.0 - discount));
}

std::string EcoPricing::name() const { return "Eco (10% off)"; }

Driver* HighestRatedDispatch::chooseDriver(std::vector<Driver>& drivers, const Rider&) const {
    Driver* best = nullptr;
    double bestRating = -1.0;

    for (auto& driver : drivers) {
        if (!driver.available()) {
            continue;
        }
        if (driver.rating() > bestRating) {
            bestRating = driver.rating();
            best = &driver;
        }
    }

    return best;
}

std::string HighestRatedDispatch::name() const { return "Highest rated"; }

Driver* NearestDriverDispatch::chooseDriver(std::vector<Driver>& drivers, const Rider& rider) const {
    Driver* closest = nullptr;
    double bestDistance = std::numeric_limits<double>::infinity();

    for (auto& driver : drivers) {
        if (!driver.available()) {
            continue;
        }
        const double dist = driver.position().distanceTo(rider.pickup());
        if (dist < bestDistance) {
            bestDistance = dist;
            closest = &driver;
        }
    }

    return closest;
}

std::string NearestDriverDispatch::name() const { return "Nearest driver"; }

RideService::RideService(std::unique_ptr<PricingStrategy> pricingStrategy,
                         std::unique_ptr<DispatchStrategy> dispatchStrategy)
    : pricingStrategy_(std::move(pricingStrategy)),
      dispatchStrategy_(std::move(dispatchStrategy)) {}

void RideService::addDriver(Driver driver) { drivers_.push_back(std::move(driver)); }

const std::vector<Driver>& RideService::drivers() const { return drivers_; }

void RideService::setPricingStrategy(std::unique_ptr<PricingStrategy> strategy) {
    pricingStrategy_ = std::move(strategy);
}

void RideService::setDispatchStrategy(std::unique_ptr<DispatchStrategy> strategy) {
    dispatchStrategy_ = std::move(strategy);
}

std::optional<Trip> RideService::requestTrip(const Rider& rider) {
    if (!pricingStrategy_ || !dispatchStrategy_) {
        return std::nullopt;
    }

    Driver* chosen = dispatchStrategy_->chooseDriver(drivers_, rider);
    if (chosen == nullptr) {
        return std::nullopt;
    }

    chosen->setAvailable(false);

    const double distance = rider.pickup().distanceTo(rider.dropoff());
    const double durationMinutes = estimateDurationMinutes(distance);
    const double fare = pricingStrategy_->calculateFare(distance, durationMinutes);

    Trip trip{nextTripId_++, &rider, chosen, distance, durationMinutes, fare,
              pricingStrategy_->name(), dispatchStrategy_->name()};

    chosen->moveTo(rider.dropoff());
    chosen->setAvailable(true);

    return trip;
}

double RideService::estimateDurationMinutes(double distanceKm) const {
    const double averageSpeedKmh = 40.0;  // Rough city average
    const double hours = (distanceKm <= 0.01) ? 0.05 : distanceKm / averageSpeedKmh;
    return hours * 60.0;
}
