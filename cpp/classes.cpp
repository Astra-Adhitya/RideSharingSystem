#include <direct.h>
#include "classes.h"

// -------- USER --------
User::User(int i, string n) {
    id = i;
    name = n;
}

string User::getName() const {
    return name;
}

void User::display() const {
    cout << "Name: " << name << endl;
}

// -------- DRIVER --------
Driver::Driver(int i, string n, string v) : User(i, n) {
    vehicleType = v;
}

string Driver::getVehicleType() const {
    return vehicleType;
}

void Driver::display() const {
    cout << "Driver: " << name << " | Vehicle: " << vehicleType << endl;
}

// -------- PASSENGER --------
Passenger::Passenger(int i, string n) : User(i, n) {}

void Passenger::display() const {
    cout << "Passenger: " << name << endl;
}

// -------- LOCATION --------
Location::Location(string p) {
    place = p;
}

string Location::getPlace() const {
    return place;
}

// -------- FARE CALCULATION --------
double BikeFare::calculateFare(double distance) {
    return 40 + (distance * 10);
}

double CarFare::calculateFare(double distance) {
    return 50 + (distance * 20);
}

// -------- RIDE --------
Ride::Ride(Passenger p, Driver d, Location pick, Location dr, double dist)
    : passenger(p), driver(d), pickup(pick), drop(dr) {
    distance = dist;
    fare = 0;
}

void Ride::setFare(double f) {
    fare = f;
}

void Ride::display() const {
    cout << "\n--- Ride Details ---\n";
    passenger.display();
    driver.display();
    cout << "Pickup: " << pickup.getPlace() << endl;
    cout << "Drop: " << drop.getPlace() << endl;
    cout << "Distance: " << distance << endl;
    cout << "Fare: " << fare << endl;
}

// -------- FILE HANDLING --------
void DataManager::saveRide(string pname, string dname, string pickup, string drop, double distance, double fare) {
    ofstream file("../data/data.csv", ios::app);
    if (!file) {
        cout << "ERROR: File not opened!" << endl;
        return;
    } else {
        cout << "File opened successfully\n";
}
    file << pname << ","
         << dname << ","
         << pickup << ","
         << drop << ","
         << distance << ","
         << fare << endl;

    file.close();
}

// -------- SYSTEM --------
void RideManager::start() {
    char cwd[256];
    _getcwd(cwd, sizeof(cwd));
    cout << "Current Working Directory: " << cwd << endl;
    int choice;

    while (true) {
        cout << "\n===== Ride Sharing System =====\n";
        cout << "1. Book Ride\n";
        cout << "2. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {

            string pname, vehicle, pickupLoc, dropLoc;
            double distance;

            cout << "\nEnter Passenger Name: ";
            cin >> pname;

            cout << "Enter Vehicle Type (bike/car): ";
            cin >> vehicle;

            cout << "Enter Pickup Location: ";
            cin >> pickupLoc;

            cout << "Enter Drop Location: ";
            cin >> dropLoc;

            cout << "Enter Distance: ";
            cin >> distance;

            Passenger p1(1, pname);
            Driver d1(1, "Rahul", vehicle);

            Location pick(pickupLoc);
            Location drop(dropLoc);

            // POLYMORPHISM
            FareCalculator* calc;

            if (vehicle == "bike")
                calc = new BikeFare();
            else
                calc = new CarFare();

            double fare = calc->calculateFare(distance);

            Ride ride(p1, d1, pick, drop, distance);
            ride.setFare(fare);

            ride.display();

            DataManager dm;
            dm.saveRide(p1.getName(), d1.getName(),
                        pick.getPlace(), drop.getPlace(),
                        distance, fare);

            delete calc;
        }

        else if (choice == 2) {
            cout << "Exiting program...\n";
            break;
        }

        else {
            cout << "Invalid choice!\n";
        }
    }
}