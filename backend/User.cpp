/*
 * ============================================================
 *  FILE: User.cpp
 *  DESCRIPTION: Implementation file for the User class hierarchy.
 *
 *  OOP CONCEPTS DEMONSTRATED:
 *   1. CONSTRUCTOR INITIALISER LISTS  — efficient member
 *      initialisation before the constructor body runs.
 *   2. BASE-CLASS CONSTRUCTOR CHAINING — Rider/Driver/Admin
 *      constructors delegate to User's constructor.
 *   3. FUNCTION OVERRIDING (POLYMORPHISM) — each derived class
 *      provides its own getRole() definition.
 *   4. ENCAPSULATION — only public methods expose class state.
 * ============================================================
 */

#include "User.h"  // Include the class declarations

/* ==========================================================
 *  BASE CLASS: User — Method Implementations
 * ========================================================== */

/*
 * User CONSTRUCTOR
 *
 * Initialiser list ": id(id), name(name), phone(phone)" copies
 * the arguments directly into the member variables before the
 * constructor body executes. This is more efficient than
 * assigning inside the body because it avoids double
 * initialisation for non-trivial types like std::string.
 */
User::User(std::string id, std::string name, std::string phone)
    : id(id), name(name), phone(phone) {}
//  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  CONSTRUCTOR INITIALISER LIST (OOP best practice)

/* GETTER methods — ENCAPSULATION
 * 'const' after the signature means the method will NOT modify
 * any member variables (read-only access).
 * Returning by value copies the string; returning by reference
 * would expose internal state, breaking encapsulation.
 */
std::string User::getId()    const { return id;    }
std::string User::getName()  const { return name;  }
std::string User::getPhone() const { return phone; }


/* ==========================================================
 *  DERIVED CLASS: Rider — Method Implementations
 * ========================================================== */

/*
 * Rider CONSTRUCTOR
 *
 * INHERITANCE + CONSTRUCTOR CHAINING:
 * The Rider constructor must initialise the inherited User
 * portion of the object. It does this by calling User's
 * constructor in the initialiser list ": User(id, name, phone)".
 * This is the only way to pass arguments to a base constructor.
 */
Rider::Rider(std::string id, std::string name, std::string phone)
    : User(id, name, phone) {}
//  ^^^^^^^^^^^^^^^^^^^^^^
//  Calls the User base constructor — CONSTRUCTOR CHAINING

/*
 * getRole() OVERRIDE
 *
 * POLYMORPHISM: This function satisfies the pure virtual
 * contract defined in User. When a User* points to a Rider
 * object and getRole() is called, the virtual table (vtable)
 * dispatches to THIS implementation at RUNTIME.
 */
std::string Rider::getRole() const { return "rider"; }


/* ==========================================================
 *  DERIVED CLASS: Driver — Method Implementations
 * ========================================================== */

/*
 * Driver CONSTRUCTOR
 *
 * INHERITANCE + EXTRA MEMBERS:
 * Like Rider, chains to User constructor. Additionally
 * initialises vehicle and available (data unique to Driver).
 * 'available' has a default value of true in the header, so
 * if omitted by the caller, the driver starts online.
 */
Driver::Driver(std::string id, std::string name, std::string phone,
               std::string vehicle, bool available)
    : User(id, name, phone),   // Call base constructor — CHAINING
      vehicle(vehicle),         // Initialise driver-specific data
      available(available) {}   // ENCAPSULATED private member

/*
 * GETTER: getVehicle()
 * Returns a copy of the vehicle string — read-only access to
 * the private 'vehicle' member (ENCAPSULATION).
 */
std::string Driver::getVehicle()  const { return vehicle;   }

/*
 * GETTER: isAvailable()
 * Returns the availability status — read-only access to the
 * private 'available' member (ENCAPSULATION).
 */
bool        Driver::isAvailable() const { return available; }

/*
 * SETTER: setAvailable()
 *
 * ENCAPSULATION — controlled write access:
 * External code cannot write to 'available' directly because
 * it is private. This setter is the only sanctioned way to
 * change it, ensuring we can add validation logic here if
 * needed in the future without changing the public interface.
 */
void Driver::setAvailable(bool status) {
    available = status;  // Controlled mutation of private member
}

/*
 * getRole() OVERRIDE
 * POLYMORPHISM: returns "driver" when called through a User*
 * that points to a Driver object.
 */
std::string Driver::getRole() const { return "driver"; }


/* ==========================================================
 *  DERIVED CLASS: Admin — Method Implementations
 * ========================================================== */

/*
 * Admin CONSTRUCTOR
 * Minimal — just chains to the User base constructor because
 * Admin adds no extra data members.
 */
Admin::Admin(std::string id, std::string name, std::string phone)
    : User(id, name, phone) {}  // CONSTRUCTOR CHAINING

/*
 * getRole() OVERRIDE
 * POLYMORPHISM: returns "admin" when called through a User*
 * pointing to an Admin object.
 */
std::string Admin::getRole() const { return "admin"; }
