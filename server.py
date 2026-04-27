"""
Ride Sharing System - Python Server Layer
Serves frontend static files and REST APIs backed by JSON data files.
The C++ backend classes define the data model; this server reads/writes the same JSON files.
"""
import json
import os
import uuid
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse

# ---- Paths ----
BASE_DIR   = os.path.dirname(os.path.abspath(__file__))
FRONTEND   = os.path.join(BASE_DIR, "frontend")
DATA_DIR   = os.path.join(BASE_DIR, "data")
USERS_FILE = os.path.join(DATA_DIR, "users.txt")
RIDES_FILE = os.path.join(DATA_DIR, "rides.txt")

MIME = {
    ".html": "text/html",
    ".css":  "text/css",
    ".js":   "application/javascript",
    ".json": "application/json",
    ".png":  "image/png",
    ".ico":  "image/x-icon",
}

# ---- Data helpers ----
def load_json(path, default):
    try:
        with open(path, "r") as f:
            return json.load(f)
    except Exception:
        return default

def save_json(path, data):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump(data, f, indent=4)

def calc_fare(distance):
    """Same formula as Ride::calculateFare() in C++"""
    return round(5.0 + distance * 2.5, 2)

# ---- HTTP Handler ----
class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        print(f"[{self.address_string()}] {fmt % args}")

    def send_json(self, code, obj):
        body = json.dumps(obj).encode()
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", len(body))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(body)

    def serve_static(self, path):
        # Default root to index.html
        if path == "/" or path == "":
            path = "/index.html"
        file_path = os.path.join(FRONTEND, path.lstrip("/"))
        if not os.path.isfile(file_path):
            self.send_response(404)
            self.end_headers()
            return
        ext  = os.path.splitext(file_path)[1]
        mime = MIME.get(ext, "application/octet-stream")
        with open(file_path, "rb") as f:
            body = f.read()
        self.send_response(200)
        self.send_header("Content-Type", mime)
        self.send_header("Content-Length", len(body))
        self.end_headers()
        self.wfile.write(body)

    def read_body(self):
        length = int(self.headers.get("Content-Length", 0))
        return json.loads(self.rfile.read(length)) if length else {}

    # ---- GET ----
    def do_GET(self):
        path = urlparse(self.path).path

        if path == "/api/availableDrivers":
            users = load_json(USERS_FILE, [])
            drivers = [
                {"id": u["id"], "name": u["name"], "vehicle": u.get("vehicle", "")}
                for u in users
                if u.get("role") == "driver" and u.get("available", True)
            ]
            self.send_json(200, drivers)

        elif path.startswith("/api/rideHistory/"):
            uid   = path.split("/api/rideHistory/")[-1]
            rides = load_json(RIDES_FILE, [])
            hist  = [r for r in rides if r.get("driverId") == uid or r.get("riderId") == uid]
            self.send_json(200, hist)

        elif path == "/api/admin/stats":
            users  = load_json(USERS_FILE, [])
            rides  = load_json(RIDES_FILE, [])
            
            driver_earnings = {}
            rider_spent = {}
            for r in rides:
                driver_earnings[r.get("driverId")] = driver_earnings.get(r.get("driverId"), 0) + r.get("fare", 0)
                rider_spent[r.get("riderId")] = rider_spent.get(r.get("riderId"), 0) + r.get("fare", 0)
                
            riders  = [{"id": u["id"], "name": u["name"], "spent": round(rider_spent.get(u["id"], 0), 2)} for u in users if u.get("role") == "rider"]
            drivers = [{"id": u["id"], "name": u["name"], "vehicle": u.get("vehicle", ""), "earned": round(driver_earnings.get(u["id"], 0), 2)} for u in users if u.get("role") == "driver"]
            total_income = round(sum(r.get("fare", 0) for r in rides), 2)
            self.send_json(200, {
                "riders":      riders,
                "drivers":     drivers,
                "totalRides":  len(rides),
                "totalIncome": total_income,
            })

        else:
            self.serve_static(path)

    # ---- POST ----
    def do_POST(self):
        path = urlparse(self.path).path

        if path == "/api/register":
            data    = self.read_body()
            uid     = data.get("id", "").strip()
            name    = data.get("name", "").strip()
            phone   = data.get("phone", "").strip()
            role    = data.get("role", "").strip()
            vehicle = data.get("vehicle", "").strip()

            if not uid or not name or not phone or role not in ("rider", "driver", "admin"):
                self.send_json(400, {"success": False, "message": "Missing or invalid fields."})
                return

            users = load_json(USERS_FILE, [])
            if any(u["id"] == uid for u in users):
                self.send_json(200, {"success": False, "message": "User with this ID already exists."})
                return

            user = {"id": uid, "name": name, "phone": phone, "role": role}
            if role == "driver":
                user["vehicle"]   = vehicle
                user["available"] = True
            save_json(USERS_FILE, users + [user])
            self.send_json(200, {"success": True})

        elif path == "/api/login":
            data  = self.read_body()
            uid   = data.get("id", "").strip()
            users = load_json(USERS_FILE, [])
            match = next((u for u in users if u["id"] == uid), None)
            if match:
                self.send_json(200, {"success": True, "role": match["role"]})
            else:
                self.send_json(200, {"success": False, "message": "Invalid ID"})

        elif path == "/api/bookRide":
            data      = self.read_body()
            rider_id  = data.get("riderId", "")
            driver_id = data.get("driverId", "")
            pickup    = data.get("pickup", "")
            drop      = data.get("drop", "")
            distance  = float(data.get("distance", 0))

            users = load_json(USERS_FILE, [])
            # Find driver and mark unavailable
            updated = False
            for u in users:
                if u["id"] == driver_id and u.get("role") == "driver" and u.get("available", True):
                    u["available"] = False
                    updated = True
                    break
            if not updated:
                self.send_json(200, {"success": False, "message": "Driver unavailable or not found."})
                return

            save_json(USERS_FILE, users)

            fare    = calc_fare(distance)
            ride_id = "RIDE_" + str(uuid.uuid4())[:8].upper()
            ride    = {
                "rideId":   ride_id,
                "pickup":   pickup,
                "drop":     drop,
                "distance": distance,
                "fare":     fare,
                "driverId": driver_id,
                "riderId":  rider_id,
            }
            rides = load_json(RIDES_FILE, [])
            save_json(RIDES_FILE, rides + [ride])
            self.send_json(200, {"success": True, "rideId": ride_id, "fare": fare})

        elif path == "/api/admin/removeUser":
            data = self.read_body()
            uid = data.get("userId", "").strip()
            if not uid:
                self.send_json(400, {"success": False, "message": "No user ID provided."})
                return
                
            users = load_json(USERS_FILE, [])
            new_users = [u for u in users if u.get("id") != uid]
            
            if len(users) == len(new_users):
                self.send_json(404, {"success": False, "message": "User not found."})
                return
                
            save_json(USERS_FILE, new_users)
            self.send_json(200, {"success": True, "message": "User removed successfully."})

        else:
            self.send_json(404, {"error": "Not found"})

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()


if __name__ == "__main__":
    os.makedirs(DATA_DIR, exist_ok=True)
    # Seed admin account if users file is empty / missing
    users = load_json(USERS_FILE, [])
    if not any(u.get("role") == "admin" for u in users):
        users.append({"id": "admin", "name": "Admin", "phone": "admin", "role": "admin"})
        save_json(USERS_FILE, users)
        print("Seeded default admin account (ID: admin)")

    server = HTTPServer(("0.0.0.0", 8080), Handler)
    print("Ride Sharing System running at http://localhost:8080")
    print("Default admin login ID: admin")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")
