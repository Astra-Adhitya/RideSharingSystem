#ifndef RIDEMANAGER_H
#define RIDEMANAGER_H

#include <vector>
#include <string>
#include "User.h"
#include "Ride.h"
#include "json.hpp"

using json = nlohmann::json;

class RideManager {
private:
    std::vector<User*> users;
    std::vector<Ride*> rides;
    std::string usersFile;
    std::string ridesFile;

    void loadData();
    void saveData();

public:
    RideManager(std::string usersFile, std::string ridesFile);
    ~RideManager();

    bool registerUser(const std::string& id, const std::string& name, const std::string& phone, const std::string& role, const std::string& vehicle = "");
    std::string loginUser(const std::string& id); // returns role or empty if not found
    
    std::vector<User*> getAvailableDrivers();
    Ride* bookRide(const std::string& riderId, const std::string& driverId, const std::string& pickup, const std::string& drop, double distance);
    bool toggleDriverAvailability(const std::string& driverId, bool available);
    
    std::vector<Ride*> getRideHistory(const std::string& userId);
    
    json getAdminStats();
};

#endif // RIDEMANAGER_H
