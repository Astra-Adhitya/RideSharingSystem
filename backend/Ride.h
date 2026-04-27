#ifndef RIDE_H
#define RIDE_H

#include <string>
#include "User.h"

class Ride {
private:
    std::string rideId;
    std::string pickup;
    std::string drop;
    double distance;
    double fare;
    std::string driverId;
    std::string riderId;

public:
    Ride(std::string rideId, std::string pickup, std::string drop, double distance, std::string driverId, std::string riderId);

    void calculateFare(); // Basic formula

    std::string getRideId() const;
    std::string getPickup() const;
    std::string getDrop() const;
    double getDistance() const;
    double getFare() const;
    std::string getDriverId() const;
    std::string getRiderId() const;
};

#endif // RIDE_H
