#ifndef RIDE_SHARING_H
#define RIDE_SHARING_H

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct Location {
    double x{};
    double y{};

    double distanceTo(const Location& other) const;
};

class Rider {
public:
    Rider(int id, std::string name, Location pickup, Location dropoff);

    int id() const;
    const std::string& name() const;
    const Location& pickup() const;
    const Location& dropoff() const;

private:
    int id_;
    std::string name_;
    Location pickup_;
    Location dropoff_;
};

class Driver {
public:
    Driver(int id, std::string name, double rating, Location position);

    int id() const;
    const std::string& name() const;
    double rating() const;
    bool available() const;
    const Location& position() const;

    void setAvailable(bool value);
    void moveTo(const Location& destination);

private:
    int id_;
    std::string name_;
    double rating_{};
    bool available_{true};
    Location position_;
};

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double calculateFare(double distanceKm, double minutes) const = 0;
    virtual std::string name() const = 0;
};

class StandardPricing : public PricingStrategy {
public:
    double calculateFare(double distanceKm, double minutes) const override;
    std::string name() const override;
};

class SurgePricing : public PricingStrategy {
public:
    explicit SurgePricing(double surgeMultiplier);

    double calculateFare(double distanceKm, double minutes) const override;
    std::string name() const override;

private:
    double surgeMultiplier_{1.0};
};

class EcoPricing : public PricingStrategy {
public:
    double calculateFare(double distanceKm, double minutes) const override;
    std::string name() const override;
};

class DispatchStrategy {
public:
    virtual ~DispatchStrategy() = default;
    virtual Driver* chooseDriver(std::vector<Driver>& drivers, const Rider& rider) const = 0;
    virtual std::string name() const = 0;
};

class HighestRatedDispatch : public DispatchStrategy {
public:
    Driver* chooseDriver(std::vector<Driver>& drivers, const Rider& rider) const override;
    std::string name() const override;
};

class NearestDriverDispatch : public DispatchStrategy {
public:
    Driver* chooseDriver(std::vector<Driver>& drivers, const Rider& rider) const override;
    std::string name() const override;
};

struct Trip {
    int id{};
    const Rider* rider{};
    Driver* driver{};
    double distanceKm{};
    double durationMinutes{};
    double fare{};
    std::string pricingModel;
    std::string dispatchModel;
};

class RideService {
public:
    RideService(std::unique_ptr<PricingStrategy> pricingStrategy,
                std::unique_ptr<DispatchStrategy> dispatchStrategy);

    void addDriver(Driver driver);
    const std::vector<Driver>& drivers() const;

    void setPricingStrategy(std::unique_ptr<PricingStrategy> strategy);
    void setDispatchStrategy(std::unique_ptr<DispatchStrategy> strategy);

    std::optional<Trip> requestTrip(const Rider& rider);

private:
    double estimateDurationMinutes(double distanceKm) const;

    std::vector<Driver> drivers_;
    std::unique_ptr<PricingStrategy> pricingStrategy_;
    std::unique_ptr<DispatchStrategy> dispatchStrategy_;
    int nextTripId_{1};
};

#endif  // RIDE_SHARING_H
