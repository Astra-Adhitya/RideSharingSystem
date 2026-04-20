#ifndef CLASSES_H
#define CLASSES_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// -------- BASE CLASS --------
class User {
protected:
    int id;
    string name;

public:
    User(int i, string n);
    string getName() const;
    virtual void display() const;
};

// -------- DERIVED CLASSES --------
class Driver : public User {
private:
    string vehicleType;

public:
    Driver(int i, string n, string v);
    string getVehicleType() const;
    void display() const override;
};

class Passenger : public User {
public:
    Passenger(int i, string n);
    void display() const override;
};

// -------- LOCATION --------
class Location {
private:
    string place;

public:
    Location(string p);
    string getPlace() const;
};

// -------- POLYMORPHISM --------
class FareCalculator {
public:
    virtual double calculateFare(double distance) = 0;
    virtual ~FareCalculator() {}
};

class BikeFare : public FareCalculator {
public:
    double calculateFare(double distance);
};

class CarFare : public FareCalculator {
public:
    double calculateFare(double distance);
};

// -------- RIDE --------
class Ride {
private:
    Passenger passenger;
    Driver driver;
    Location pickup;
    Location drop;
    double distance;
    double fare;

public:
    Ride(Passenger p, Driver d, Location pick, Location dr, double dist);
    void setFare(double f);
    void display() const;
};

// -------- FILE HANDLING --------
class DataManager {
public:
    void saveRide(string pname, string dname, string pickup, string drop, double distance, double fare);
};

// -------- SYSTEM --------
class RideManager {
public:
    void start();
};

#endif