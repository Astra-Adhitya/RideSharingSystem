import json, random

USERS_FILE = "data/users.txt"
RIDES_FILE = "data/rides.txt"

def load_json(fp, default):
    try:
        with open(fp, 'r') as f:
            return json.load(f)
    except:
        return default

def save_json(fp, data):
    with open(fp, 'w') as f:
        json.dump(data, f, indent=2)

users = load_json(USERS_FILE, [])
rides = load_json(RIDES_FILE, [])

# Add a new rider
new_rider = {
    "id": "arjun_01",
    "name": "Arjun Sharma",
    "phone": "555-9901",
    "role": "rider"
}

# Add a new driver
new_driver = {
    "id": "priya_01",
    "name": "Priya Patel",
    "phone": "555-9902",
    "role": "driver",
    "vehicle": "Maruti Swift Dzire",
    "available": True
}

# Only add if not already present
if not any(u['id'] == new_rider['id'] for u in users):
    users.append(new_rider)
if not any(u['id'] == new_driver['id'] for u in users):
    users.append(new_driver)

# Generate some rides for them
locations = ["MG Road", "Koramangala", "Indiranagar", "Whitefield", "Electronic City", "Jayanagar", "BTM Layout", "HSR Layout"]
all_drivers = [u for u in users if u.get("role") == "driver"]

for i in range(8):
    pickup = random.choice(locations)
    drop = random.choice([l for l in locations if l != pickup])
    distance = round(random.uniform(3.0, 18.0), 1)
    fare = round(5.0 + distance * 2.5, 2)
    driver = random.choice(all_drivers)
    ride = {
        "rideId": f"RIDE_NEW{i:04d}",
        "pickup": pickup,
        "drop": drop,
        "distance": distance,
        "fare": fare,
        "driverId": driver["id"],
        "riderId": new_rider["id"]
    }
    rides.append(ride)

# A few rides where Priya is the driver
for i in range(5):
    pickup = random.choice(locations)
    drop = random.choice([l for l in locations if l != pickup])
    distance = round(random.uniform(2.0, 15.0), 1)
    fare = round(5.0 + distance * 2.5, 2)
    all_riders = [u for u in users if u.get("role") == "rider"]
    rider = random.choice(all_riders)
    ride = {
        "rideId": f"RIDE_PRI{i:04d}",
        "pickup": pickup,
        "drop": drop,
        "distance": distance,
        "fare": fare,
        "driverId": new_driver["id"],
        "riderId": rider["id"]
    }
    rides.append(ride)

save_json(USERS_FILE, users)
save_json(RIDES_FILE, rides)
print(f"Added rider: {new_rider['name']} (ID: {new_rider['id']})")
print(f"Added driver: {new_driver['name']} (ID: {new_driver['id']})")
print("Added 13 new rides. Done!")
