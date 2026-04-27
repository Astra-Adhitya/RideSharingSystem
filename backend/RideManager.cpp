#include "RideManager.h"
#include <fstream>
#include <iostream>

RideManager::RideManager(std::string usersFile, std::string ridesFile)
    : usersFile(usersFile), ridesFile(ridesFile) {
    loadData();
}

RideManager::~RideManager() {
    saveData();
    for(auto u : users) delete u;
    for(auto r : rides) delete r;
}

void RideManager::loadData() {
    std::ifstream uf(usersFile);
    if(uf.is_open()) {
        json j;
        try {
            uf >> j;
            for(auto& item : j) {
                if(item["role"] == "rider") {
                    users.push_back(new Rider(item["id"], item["name"], item["phone"]));
                } else if(item["role"] == "driver") {
                    users.push_back(new Driver(item["id"], item["name"], item["phone"], item["vehicle"], item["available"]));
                } else if(item["role"] == "admin") {
                    users.push_back(new Admin(item["id"], item["name"], item["phone"]));
                }
            }
        } catch(...) {}
        uf.close();
    }

    std::ifstream rf(ridesFile);
    if(rf.is_open()) {
        json j;
        try {
            rf >> j;
            for(auto& item : j) {
                rides.push_back(new Ride(item["rideId"], item["pickup"], item["drop"], item["distance"], item["driverId"], item["riderId"]));
            }
        } catch(...) {}
        rf.close();
    }
}

void RideManager::saveData() {
    json ju = json::array();
    for(auto u : users) {
        if(u->getRole() == "rider") {
            ju.push_back({{"id", u->getId()}, {"name", u->getName()}, {"phone", u->getPhone()}, {"role", "rider"}});
        } else if(u->getRole() == "driver") {
            Driver* d = dynamic_cast<Driver*>(u);
            ju.push_back({{"id", d->getId()}, {"name", d->getName()}, {"phone", d->getPhone()}, {"role", "driver"}, {"vehicle", d->getVehicle()}, {"available", d->isAvailable()}});
        } else if(u->getRole() == "admin") {
            ju.push_back({{"id", u->getId()}, {"name", u->getName()}, {"phone", u->getPhone()}, {"role", "admin"}});
        }
    }
    std::ofstream uf(usersFile);
    uf << ju.dump(4);
    uf.close();

    json jr = json::array();
    for(auto r : rides) {
        jr.push_back({
            {"rideId", r->getRideId()},
            {"pickup", r->getPickup()},
            {"drop", r->getDrop()},
            {"distance", r->getDistance()},
            {"fare", r->getFare()},
            {"driverId", r->getDriverId()},
            {"riderId", r->getRiderId()}
        });
    }
    std::ofstream rf(ridesFile);
    rf << jr.dump(4);
    rf.close();
}

bool RideManager::registerUser(const std::string& id, const std::string& name, const std::string& phone, const std::string& role, const std::string& vehicle) {
    for(auto u : users) {
        if(u->getId() == id) return false; // Already exists
    }
    if(role == "rider") {
        users.push_back(new Rider(id, name, phone));
    } else if(role == "driver") {
        users.push_back(new Driver(id, name, phone, vehicle, true));
    } else if(role == "admin") {
        users.push_back(new Admin(id, name, phone));
    } else {
        return false;
    }
    saveData();
    return true;
}

std::string RideManager::loginUser(const std::string& id) {
    for(auto u : users) {
        if(u->getId() == id) {
            return u->getRole();
        }
    }
    return "";
}

std::vector<User*> RideManager::getAvailableDrivers() {
    std::vector<User*> avail;
    for(auto u : users) {
        if(u->getRole() == "driver") {
            Driver* d = dynamic_cast<Driver*>(u);
            if(d->isAvailable()) avail.push_back(d);
        }
    }
    return avail;
}

Ride* RideManager::bookRide(const std::string& riderId, const std::string& driverId, const std::string& pickup, const std::string& drop, double distance) {
    // Check if rider and driver exist
    User* rider = nullptr;
    Driver* driver = nullptr;
    for(auto u : users) {
        if(u->getId() == riderId && u->getRole() == "rider") rider = u;
        if(u->getId() == driverId && u->getRole() == "driver") driver = dynamic_cast<Driver*>(u);
    }
    
    if(!rider || !driver || !driver->isAvailable()) return nullptr;

    std::string rideId = "RIDE_" + std::to_string(rides.size() + 1);
    Ride* newRide = new Ride(rideId, pickup, drop, distance, driverId, riderId);
    rides.push_back(newRide);
    driver->setAvailable(false); // Driver is now busy
    saveData();
    return newRide;
}

bool RideManager::toggleDriverAvailability(const std::string& driverId, bool available) {
    for(auto u : users) {
        if(u->getId() == driverId && u->getRole() == "driver") {
            Driver* d = dynamic_cast<Driver*>(u);
            d->setAvailable(available);
            saveData();
            return true;
        }
    }
    return false;
}

std::vector<Ride*> RideManager::getRideHistory(const std::string& userId) {
    std::vector<Ride*> history;
    for(auto r : rides) {
        if(r->getDriverId() == userId || r->getRiderId() == userId) {
            history.push_back(r);
        }
    }
    return history;
}

json RideManager::getAdminStats() {
    json result;
    json ridersList = json::array();
    json driversList = json::array();
    double totalIncome = 0.0;

    for(auto u : users) {
        if(u->getRole() == "rider") {
            ridersList.push_back({{"id", u->getId()}, {"name", u->getName()}});
        } else if(u->getRole() == "driver") {
            Driver* d = dynamic_cast<Driver*>(u);
            driversList.push_back({{"id", d->getId()}, {"name", d->getName()}, {"vehicle", d->getVehicle()}});
        }
    }
    
    for(auto r : rides) {
        totalIncome += r->getFare();
    }

    result["riders"] = ridersList;
    result["drivers"] = driversList;
    result["totalIncome"] = totalIncome;
    result["totalRides"] = rides.size();
    
    return result;
}
