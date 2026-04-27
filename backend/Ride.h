/*
 * ============================================================
 *  FILE: Ride.h
 *  DESCRIPTION: Header file for the Ride class.
 *
 *  OOP CONCEPTS USED:
 *   1. ENCAPSULATION  — All ride data is private. External
 *      code can only read it through public getter methods.
 *   2. ABSTRACTION    — The fare calculation detail is hidden
 *      behind calculateFare(); callers just call getFare().
 *   3. SINGLE RESPONSIBILITY — Ride only models one booking;
 *      it does not manage collections or files (that is
 *      RideManager's job).
 *   4. CONST CORRECTNESS — All getters are marked 'const'
 *      so they can be called on const Ride objects/references.
 * ============================================================
 */

#ifndef RIDE_H   // Include guard: prevents multiple inclusion
#define RIDE_H

#include <string>  // For std::string
#include "User.h"  // Includes User hierarchy (not strictly needed
                   // here but kept for contextual clarity)

/* ----------------------------------------------------------
 *  CLASS: Ride
 *
 *  Represents a single completed or booked trip between a
 *  rider and a driver. All fields are private (ENCAPSULATION);
 *  external code accesses them through const getter methods.
 * ---------------------------------------------------------- */
class Ride {
private:
    // ENCAPSULATED DATA — private to this class only
    std::string rideId;    // Unique identifier (e.g. "RIDE_0001")
    std::string pickup;    // Pickup location name
    std::string drop;      // Drop-off location name
    double distance;       // Distance in kilometres
    double fare;           // Computed fare in INR (set by calculateFare)
    std::string driverId;  // ID of the driver who took this ride
    std::string riderId;   // ID of the rider who booked this ride

public:
    /*
     * CONSTRUCTOR
     * Initialises all fields via an initialiser list and then
     * immediately calls calculateFare() so the fare is always
     * consistent with the distance as soon as the object exists.
     *
     * Note: 'fare' is NOT a constructor parameter — it is
     * always derived from 'distance' (ABSTRACTION: the caller
     * doesn't need to supply or know the formula).
     */
    Ride(std::string rideId, std::string pickup, std::string drop,
         double distance, std::string driverId, std::string riderId);

    /*
     * calculateFare()
     *
     * ABSTRACTION: hides the pricing formula from outside code.
     * Called internally by the constructor; could also be
     * re-called if distance is ever updated. Kept public so
     * RideManager can trigger re-calculation if needed.
     *
     * Formula: fare = 5.0 (base) + distance × 2.5 (per km)
     */
    void calculateFare();

    // ---- GETTER METHODS (ENCAPSULATION / CONST CORRECTNESS) ----
    // Each method is 'const' — guaranteed not to mutate state.

    std::string getRideId()   const;  // Returns the unique ride ID
    std::string getPickup()   const;  // Returns pickup location
    std::string getDrop()     const;  // Returns drop location
    double      getDistance() const;  // Returns distance in km
    double      getFare()     const;  // Returns calculated fare (₹)
    std::string getDriverId() const;  // Returns the driver's user ID
    std::string getRiderId()  const;  // Returns the rider's user ID
};

#endif // RIDE_H  // End of include guard
