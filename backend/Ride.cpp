/*
 * ============================================================
 *  FILE: Ride.cpp
 *  DESCRIPTION: Implementation of the Ride class.
 *
 *  OOP CONCEPTS DEMONSTRATED:
 *   1. CONSTRUCTOR INITIALISER LIST — initialises all members
 *      before the constructor body runs; then calls
 *      calculateFare() to compute the derived 'fare' field.
 *   2. ENCAPSULATION — 'fare' is private and computed
 *      internally; external code cannot set it directly.
 *   3. ABSTRACTION — the fare formula is hidden inside
 *      calculateFare(); callers just use getFare().
 *   4. CONST CORRECTNESS — every getter is marked 'const'.
 * ============================================================
 */

#include "Ride.h"  // Include the Ride class declaration

/* ----------------------------------------------------------
 *  Ride CONSTRUCTOR
 *
 *  CONSTRUCTOR INITIALISER LIST:
 *  All six data members that come from parameters are
 *  initialised in the list before the body runs.
 *  'fare' is intentionally LEFT OUT of the list because it
 *  is computed (not supplied), so it is set by calculateFare()
 *  in the constructor body.
 * ---------------------------------------------------------- */
Ride::Ride(std::string rideId, std::string pickup, std::string drop,
           double distance, std::string driverId, std::string riderId)
    : rideId(rideId),       // Initialise unique ride ID
      pickup(pickup),       // Initialise pickup location
      drop(drop),           // Initialise drop location
      distance(distance),   // Initialise distance in km
      driverId(driverId),   // Initialise driver's user ID
      riderId(riderId)      // Initialise rider's user ID
{
    // ABSTRACTION: call the private helper to compute the fare
    // so it is always consistent from the moment the object exists.
    calculateFare();
}

/* ----------------------------------------------------------
 *  calculateFare()
 *
 *  ABSTRACTION — hides the pricing algorithm:
 *    fare = base charge + (distance × per-km rate)
 *         = ₹5.00      + (distance × ₹2.50)
 *
 *  By keeping the formula inside this method we can change
 *  the pricing logic in ONE place without touching any caller.
 *  This is also an example of the SINGLE RESPONSIBILITY
 *  PRINCIPLE: this method's only job is to compute the fare.
 * ---------------------------------------------------------- */
void Ride::calculateFare() {
    const double BASE_FARE    = 5.0;   // Fixed starting charge (₹)
    const double RATE_PER_KM  = 2.5;   // Charge per kilometre (₹/km)

    // 'fare' is a private member — only this class can write it
    // (ENCAPSULATION). External code reads it via getFare().
    fare = BASE_FARE + (distance * RATE_PER_KM);
}

/* ----------------------------------------------------------
 *  GETTER METHODS — ENCAPSULATION + CONST CORRECTNESS
 *
 *  Each method is marked 'const' meaning it promises not to
 *  change any member variable. This allows them to be called
 *  on const Ride objects or through const references/pointers.
 *
 *  Returning by value (copy) keeps the internal state safe;
 *  external code cannot accidentally alter member data.
 * ---------------------------------------------------------- */

// Returns the unique ride identifier (e.g. "RIDE_0042")
std::string Ride::getRideId()   const { return rideId;   }

// Returns the pickup location string
std::string Ride::getPickup()   const { return pickup;   }

// Returns the drop-off location string
std::string Ride::getDrop()     const { return drop;     }

// Returns the trip distance in kilometres
double      Ride::getDistance() const { return distance; }

// Returns the fare calculated by calculateFare() (₹)
double      Ride::getFare()     const { return fare;     }

// Returns the ID of the driver who performed this ride
std::string Ride::getDriverId() const { return driverId; }

// Returns the ID of the rider who booked this ride
std::string Ride::getRiderId()  const { return riderId;  }
