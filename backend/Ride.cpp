#include "Ride.h"

Ride::Ride(std::string rideId, std::string pickup, std::string drop, double distance, std::string driverId, std::string riderId)
    : rideId(rideId), pickup(pickup), drop(drop), distance(distance), driverId(driverId), riderId(riderId) {
    calculateFare();
}

void Ride::calculateFare() {
    // Basic flat rate of 5.0 + 2.5 per distance unit
    fare = 5.0 + (distance * 2.5);
}

std::string Ride::getRideId() const { return rideId; }
std::string Ride::getPickup() const { return pickup; }
std::string Ride::getDrop() const { return drop; }
double Ride::getDistance() const { return distance; }
double Ride::getFare() const { return fare; }
std::string Ride::getDriverId() const { return driverId; }
std::string Ride::getRiderId() const { return riderId; }
