#include "User.h"

// -------- USER --------
User::User(std::string id, std::string name, std::string phone) 
    : id(id), name(name), phone(phone) {}

std::string User::getId() const { return id; }
std::string User::getName() const { return name; }
std::string User::getPhone() const { return phone; }

// -------- RIDER --------
Rider::Rider(std::string id, std::string name, std::string phone) 
    : User(id, name, phone) {}

std::string Rider::getRole() const { return "rider"; }

// -------- DRIVER --------
Driver::Driver(std::string id, std::string name, std::string phone, std::string vehicle, bool available)
    : User(id, name, phone), vehicle(vehicle), available(available) {}

std::string Driver::getVehicle() const { return vehicle; }
bool Driver::isAvailable() const { return available; }
void Driver::setAvailable(bool status) { available = status; }

std::string Driver::getRole() const { return "driver"; }

// -------- ADMIN --------
Admin::Admin(std::string id, std::string name, std::string phone)
    : User(id, name, phone) {}

std::string Admin::getRole() const { return "admin"; }
