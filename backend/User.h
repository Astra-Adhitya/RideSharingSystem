#ifndef USER_H
#define USER_H

#include <string>

// -------- BASE CLASS --------
class User {
protected:
    std::string id;
    std::string name;
    std::string phone;

public:
    User(std::string id, std::string name, std::string phone);
    virtual ~User() = default;

    std::string getId() const;
    std::string getName() const;
    std::string getPhone() const;

    virtual std::string getRole() const = 0; // Pure virtual function for polymorphism
};

// -------- DERIVED CLASSES --------
class Rider : public User {
public:
    Rider(std::string id, std::string name, std::string phone);
    std::string getRole() const override;
};

class Driver : public User {
private:
    std::string vehicle;
    bool available;

public:
    Driver(std::string id, std::string name, std::string phone, std::string vehicle, bool available = true);

    std::string getVehicle() const;
    bool isAvailable() const;
    void setAvailable(bool status);

    std::string getRole() const override;
};

class Admin : public User {
public:
    Admin(std::string id, std::string name, std::string phone);
    std::string getRole() const override;
};

#endif // USER_H
