/*
 * ============================================================
 *  FILE: User.h
 *  DESCRIPTION: Header file declaring the User class hierarchy.
 *
 *  OOP CONCEPTS USED:
 *   1. ABSTRACTION     - User is an abstract base class (has pure
 *                        virtual function getRole()) that hides
 *                        implementation details from the caller.
 *   2. ENCAPSULATION   - Member data (id, name, phone) are in the
 *                        'protected' section so they are accessible
 *                        to derived classes but hidden from outside.
 *   3. INHERITANCE     - Rider, Driver, and Admin inherit publicly
 *                        from User, gaining its interface/data.
 *   4. POLYMORPHISM    - getRole() is declared as a pure virtual
 *                        function, allowing each derived class to
 *                        provide its own concrete implementation.
 *                        Callers work through User* pointers.
 *   5. VIRTUAL DESTRUCTOR - Ensures proper cleanup of derived
 *                        class objects when deleted via a base
 *                        class pointer (prevents memory leaks).
 * ============================================================
 */

#ifndef USER_H   // Include guard: prevents double inclusion
#define USER_H

#include <string>  // Standard string class

/* ----------------------------------------------------------
 *  ABSTRACT BASE CLASS: User
 *
 *  Represents any person in the ride-sharing system.
 *  Cannot be instantiated directly because getRole() is pure
 *  virtual — it is an ABSTRACT CLASS.
 * ---------------------------------------------------------- */
class User {
protected:
    // ENCAPSULATION: 'protected' data is accessible to derived
    // classes but not directly from outside the class hierarchy.
    std::string id;     // Unique identifier for the user
    std::string name;   // Full display name
    std::string phone;  // Contact phone number

public:
    /*
     * CONSTRUCTOR
     * Uses an initialiser list to set all three fields.
     * The base constructor is called from derived class
     * constructors via the initialiser list.
     */
    User(std::string id, std::string name, std::string phone);

    /*
     * VIRTUAL DESTRUCTOR
     * Declared virtual so that when a Rider/Driver/Admin object
     * is destroyed through a User* pointer, the correct
     * derived destructor is called first (prevents memory leaks).
     * '= default' tells the compiler to generate the body.
     */
    virtual ~User() = default;

    // ACCESSOR METHODS (Getters) — expose read-only access to
    // encapsulated private data. 'const' means they won't
    // modify the object.
    std::string getId()    const;  // Returns the user's unique ID
    std::string getName()  const;  // Returns the user's full name
    std::string getPhone() const;  // Returns the user's phone number

    /*
     * PURE VIRTUAL FUNCTION — ABSTRACTION + POLYMORPHISM
     *
     * By making getRole() pure virtual (= 0), we:
     *   a) Force every concrete derived class to provide its own
     *      implementation (Rider returns "rider", etc.)
     *   b) Make User an abstract class (cannot be instantiated).
     *   c) Enable RUNTIME POLYMORPHISM: a User* can point to any
     *      subtype and getRole() resolves to the correct version
     *      at runtime via the virtual table (vtable).
     *
     * 'override' is added in derived classes to catch typos at
     * compile time.
     */
    virtual std::string getRole() const = 0;
};


/* ----------------------------------------------------------
 *  DERIVED CLASS: Rider
 *
 *  INHERITANCE: Inherits all data and methods from User.
 *  Adds no extra data; only provides a concrete getRole().
 * ---------------------------------------------------------- */
class Rider : public User {
public:
    /*
     * CONSTRUCTOR
     * Calls the base-class (User) constructor via initialiser
     * list to initialise the inherited id, name, phone fields.
     */
    Rider(std::string id, std::string name, std::string phone);

    /*
     * OVERRIDE of pure virtual getRole()
     * POLYMORPHISM: when called on a User* pointing to a Rider
     * object, this version runs and returns "rider".
     * 'override' keyword guarantees we are truly overriding a
     * virtual function (compile-time safety).
     */
    std::string getRole() const override;
};


/* ----------------------------------------------------------
 *  DERIVED CLASS: Driver
 *
 *  INHERITANCE: Inherits from User and adds its own private
 *  data: vehicle description and availability status.
 *  ENCAPSULATION: vehicle and available are private — external
 *  code must use getters/setters.
 * ---------------------------------------------------------- */
class Driver : public User {
private:
    // Additional data specific to drivers only
    std::string vehicle;  // Description of the driver's vehicle
    bool available;       // true = online/available for bookings

public:
    /*
     * CONSTRUCTOR
     * Parameters include vehicle and available in addition to
     * the base User parameters.
     * 'bool available = true' is a DEFAULT ARGUMENT: if the
     * caller omits it, the driver starts as available.
     */
    Driver(std::string id, std::string name, std::string phone,
           std::string vehicle, bool available = true);

    // GETTER: returns the vehicle description (read-only)
    std::string getVehicle()   const;

    // GETTER: returns true if the driver is currently available
    bool        isAvailable()  const;

    /*
     * SETTER: allows changing availability status.
     * This is the only way external code can mutate 'available',
     * enforcing ENCAPSULATION (controlled write access).
     */
    void        setAvailable(bool status);

    // OVERRIDE of pure virtual getRole() → returns "driver"
    std::string getRole() const override;
};


/* ----------------------------------------------------------
 *  DERIVED CLASS: Admin
 *
 *  INHERITANCE: Inherits from User with no extra data.
 *  Represents a system administrator account.
 * ---------------------------------------------------------- */
class Admin : public User {
public:
    /*
     * CONSTRUCTOR
     * Delegates directly to the User base constructor.
     */
    Admin(std::string id, std::string name, std::string phone);

    // OVERRIDE of pure virtual getRole() → returns "admin"
    std::string getRole() const override;
};

#endif // USER_H   // End of include guard
