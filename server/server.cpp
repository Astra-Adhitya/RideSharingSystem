#include "httplib.h"
#include "../backend/RideManager.h"
#include "../backend/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

// Helper: read a file into a string
std::string readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main() {
    httplib::Server svr;
    RideManager manager("../data/users.txt", "../data/rides.txt");

    // ---- API ROUTES (must come before static file handler) ----

    // 1. POST /api/register
    svr.Post("/api/register", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            auto j = json::parse(req.body);
            std::string id      = j["id"];
            std::string name    = j["name"];
            std::string phone   = j["phone"];
            std::string role    = j["role"];
            std::string vehicle = j.contains("vehicle") ? j["vehicle"].get<std::string>() : "";

            bool success = manager.registerUser(id, name, phone, role, vehicle);
            json result  = {{"success", success}};
            if (!success)
                result["message"] = "User with this ID already exists or invalid role.";
            res.set_content(result.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid Input\"}", "application/json");
        }
    });

    // 2. POST /api/login
    svr.Post("/api/login", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            auto j = json::parse(req.body);
            std::string id   = j["id"];
            std::string role = manager.loginUser(id);
            if (!role.empty()) {
                json result = {{"success", true}, {"role", role}};
                res.set_content(result.dump(), "application/json");
            } else {
                json result = {{"success", false}, {"message", "Invalid ID"}};
                res.set_content(result.dump(), "application/json");
            }
        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"Bad Request\"}", "application/json");
        }
    });

    // 3. GET /api/availableDrivers
    svr.Get("/api/availableDrivers", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        auto drivers    = manager.getAvailableDrivers();
        json result     = json::array();
        for (auto d : drivers) {
            Driver* dr = dynamic_cast<Driver*>(d);
            if (!dr) continue;
            result.push_back({{"id", dr->getId()}, {"name", dr->getName()}, {"vehicle", dr->getVehicle()}});
        }
        res.set_content(result.dump(), "application/json");
    });

    // 4. POST /api/bookRide
    svr.Post("/api/bookRide", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            auto j          = json::parse(req.body);
            std::string riderId   = j["riderId"];
            std::string driverId  = j["driverId"];
            std::string pickup    = j["pickup"];
            std::string drop      = j["drop"];
            double distance       = j["distance"];

            Ride* r = manager.bookRide(riderId, driverId, pickup, drop, distance);
            if (r) {
                json result = {{"success", true}, {"rideId", r->getRideId()}, {"fare", r->getFare()}};
                res.set_content(result.dump(), "application/json");
            } else {
                json result = {{"success", false}, {"message", "Booking failed. Driver may be unavailable."}};
                res.set_content(result.dump(), "application/json");
            }
        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"Bad Request\"}", "application/json");
        }
    });

    // 5. GET /api/rideHistory/:userId
    svr.Get(R"(/api/rideHistory/(.+))", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        std::string userId = req.matches[1];
        auto history       = manager.getRideHistory(userId);
        json result        = json::array();
        for (auto r : history) {
            result.push_back({
                {"rideId",   r->getRideId()},
                {"pickup",   r->getPickup()},
                {"drop",     r->getDrop()},
                {"distance", r->getDistance()},
                {"fare",     r->getFare()},
                {"driverId", r->getDriverId()},
                {"riderId",  r->getRiderId()}
            });
        }
        res.set_content(result.dump(), "application/json");
    });

    // 6. GET /api/admin/stats
    svr.Get("/api/admin/stats", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        json result = manager.getAdminStats();
        res.set_content(result.dump(), "application/json");
    });

    // ---- STATIC FILE SERVING (last, as catch-all) ----
    svr.Get("/styles.css", [](const httplib::Request&, httplib::Response& res) {
        std::string body = readFile("../frontend/styles.css");
        res.set_content(body, "text/css");
    });

    svr.Get("/script.js", [](const httplib::Request&, httplib::Response& res) {
        std::string body = readFile("../frontend/script.js");
        res.set_content(body, "application/javascript");
    });

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string body = readFile("../frontend/index.html");
        res.set_content(body, "text/html");
    });

    std::cout << "Server running at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
