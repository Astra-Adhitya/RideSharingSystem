import json
import os
import uuid
import random

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(BASE_DIR, "data")
USERS_FILE = os.path.join(DATA_DIR, "users.txt")
RIDES_FILE = os.path.join(DATA_DIR, "rides.txt")

def load_json(path, default):
    try:
        with open(path, "r") as f:
            return json.load(f)
    except Exception:
        return default

def save_json(path, data):
    with open(path, "w") as f:
        json.dump(data, f, indent=4)

users = load_json(USERS_FILE, [])
rides = load_json(RIDES_FILE, [])

# Generate 20 new riders
new_riders = []
for i in range(20):
    rider = {
        "id": f"rider_{i+300}",
        "name": f"Mock Rider {i+300}",
        "phone": f"555-030{i}",
        "role": "rider"
    }
    new_riders.append(rider)
    users.append(rider)

# Generate 10 new drivers
new_drivers = []
vehicles = ["Toyota Prius", "Honda Civic", "Ford Fusion", "Hyundai Elantra", "Chevy Malibu", "Nissan Altima", "VW Jetta", "Kia Optima", "Subaru Legacy", "Mazda 3"]
for i in range(10):
    driver = {
        "id": f"driver_{i+300}",
        "name": f"Mock Driver {i+300}",
        "phone": f"555-040{i}",
        "role": "driver",
        "vehicle": vehicles[i],
        "available": True
    }
    new_drivers.append(driver)
    users.append(driver)

# Get all riders and drivers to distribute trips
all_riders = [u for u in users if u.get("role") == "rider"]
all_drivers = [u for u in users if u.get("role") == "driver"]

# Generate 50 new rides
locations = ["Downtown", "Airport", "Mall", "University", "Hospital", "Stadium", "Train Station", "Tech Park"]

for i in range(50):
    rider = random.choice(all_riders)
    driver = random.choice(all_drivers)
    pickup = random.choice(locations)
    drop = random.choice([loc for loc in locations if loc != pickup])
    distance = round(random.uniform(2.0, 20.0), 1)
    fare = round(5.0 + distance * 2.5, 2)
    
    ride = {
        "rideId": "RIDE_" + str(uuid.uuid4())[:8].upper(),
        "pickup": pickup,
        "drop": drop,
        "distance": distance,
        "fare": fare,
        "driverId": driver["id"],
        "riderId": rider["id"]
    }
    rides.append(ride)

save_json(USERS_FILE, users)
save_json(RIDES_FILE, rides)

print("Successfully seeded multiple riders, drivers, and trips!")
