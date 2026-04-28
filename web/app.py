from flask import Flask, render_template, jsonify, request, redirect, url_for
import csv, os, json, uuid, math
from datetime import datetime

app = Flask(__name__)

DATA_PATH    = os.path.join(os.path.dirname(__file__), '..', 'data', 'data.csv')
PENDING_PATH  = os.path.join(os.path.dirname(__file__), '..', 'data', 'pending_rides.json')
DRIVERS_PATH  = os.path.join(os.path.dirname(__file__), '..', 'data', 'drivers.json')

# Bengaluru landmarks with coordinates
LOCATIONS = {
    "Koramangala":     {"coords": [12.9279, 77.6271]},
    "Indiranagar":     {"coords": [12.9784, 77.6408]},
    "MG Road":         {"coords": [12.9756, 77.6097]},
    "Whitefield":      {"coords": [12.9698, 77.7499]},
    "Electronic City": {"coords": [12.8452, 77.6602]},
    "Jayanagar":       {"coords": [12.9304, 77.5829]},
    "HSR Layout":      {"coords": [12.9116, 77.6473]},
    "Marathahalli":    {"coords": [12.9591, 77.6974]},
    "Banashankari":    {"coords": [12.9255, 77.5468]},
    "Yelahanka":       {"coords": [13.1005, 77.5963]},
}


# ── Helpers ─────────────────────────────────────────────────────────────────

def haversine(lat1, lon1, lat2, lon2):
    R = 6371
    dlat = math.radians(lat2 - lat1)
    dlon = math.radians(lon2 - lon1)
    a = math.sin(dlat/2)**2 + math.cos(math.radians(lat1)) * math.cos(math.radians(lat2)) * math.sin(dlon/2)**2
    return round(R * 2 * math.asin(math.sqrt(a)), 1)


def calculate_fare(distance):
    return round(30 + distance * 18, 2)   # ₹30 base + ₹18/km


def read_data():
    rides = []
    try:
        with open(DATA_PATH, 'r', newline='') as f:
            for row in csv.reader(f):
                if len(row) < 6:
                    continue
                try:
                    rides.append({
                        'passenger': row[0].strip(),
                        'driver':    row[1].strip(),
                        'pickup':    row[2].strip(),
                        'drop':      row[3].strip(),
                        'distance':  float(row[4]),
                        'fare':      float(row[5]),
                    })
                except ValueError:
                    continue
    except FileNotFoundError:
        pass

    n   = len(rides)
    rev = sum(r['fare']     for r in rides)
    dst = sum(r['distance'] for r in rides)
    return rides, n, rev, round(rev/n, 2) if n else 0, dst, \
           len(set(r['driver']    for r in rides)), \
           len(set(r['passenger'] for r in rides))


def read_pending():
    try:
        with open(PENDING_PATH, 'r') as f:
            return json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        return []


def write_pending(data):
    os.makedirs(os.path.dirname(PENDING_PATH), exist_ok=True)
    with open(PENDING_PATH, 'w') as f:
        json.dump(data, f, indent=2)


def read_drivers():
    """Return list of driver dicts {name, vehicle} from drivers.json."""
    try:
        with open(DRIVERS_PATH, 'r') as f:
            return json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        return []


def write_drivers(data):
    os.makedirs(os.path.dirname(DRIVERS_PATH), exist_ok=True)
    with open(DRIVERS_PATH, 'w') as f:
        json.dump(data, f, indent=2)


def driver_names():
    """Convenience: just the names list for dropdowns."""
    return [d['name'] for d in read_drivers()]


def append_csv(ride):
    with open(DATA_PATH, 'a', newline='') as f:
        csv.writer(f).writerow([
            ride['passenger'], ride['driver'],
            ride['pickup'],    ride['drop'],
            ride['distance'],  ride['fare'],
        ])


# ── Routes ──────────────────────────────────────────────────────────────────

@app.route('/')
def home():
    rides, n, rev, avg, dst, ud, up = read_data()
    pending = read_pending()
    return render_template('home.html',
        total_rides=n, total_revenue=round(rev,2), avg_fare=avg,
        total_distance=round(dst,2), unique_drivers=ud, unique_passengers=up,
        recent_rides=rides[-5:][::-1], pending_count=len(pending),
        locations=LOCATIONS, driver_count=len(read_drivers()),
    )


@app.route('/data')
def data():
    rides, n, *_ = read_data()
    return render_template('data.html', rides=rides, total_rides=n)


@app.route('/stats')
def stats():
    rides, n, rev, avg, dst, ud, up = read_data()
    return render_template('stats.html', rides=rides,
        total_rides=n, total_revenue=round(rev,2), avg_fare=avg,
        total_distance=round(dst,2), unique_drivers=ud, unique_passengers=up,
    )


@app.route('/book', methods=['GET', 'POST'])
def book():
    locs = list(LOCATIONS.keys())
    if request.method == 'POST':
        passenger = request.form.get('passenger','').strip()
        pickup    = request.form.get('pickup','').strip()
        drop      = request.form.get('drop','').strip()

        if not passenger or not pickup or not drop or pickup == drop:
            return render_template('book.html', locations=locs,
                error="Please fill all fields and choose different pickup & drop.")

        pc = LOCATIONS[pickup]['coords']
        dc = LOCATIONS[drop]['coords']
        dist     = haversine(pc[0], pc[1], dc[0], dc[1])
        est_fare = calculate_fare(dist)

        ride = {
            'id':           str(uuid.uuid4())[:8],
            'passenger':    passenger,
            'pickup':       pickup,   'drop':          drop,
            'pickup_coords': pc,      'drop_coords':   dc,
            'distance':     dist,     'est_fare':      est_fare,
            'timestamp':    datetime.now().strftime('%Y-%m-%d %H:%M'),
            'status':       'pending',
        }
        pending = read_pending()
        pending.append(ride)
        write_pending(pending)
        return render_template('book.html', locations=locs, success=ride)

    return render_template('book.html', locations=locs)


@app.route('/driver')
def driver():
    return render_template('driver.html',
        pending=read_pending(), drivers=read_drivers(), locations=LOCATIONS)


@app.route('/accept/<ride_id>', methods=['POST'])
def accept(ride_id):
    driver_name = request.form.get('driver_name', '').strip()
    pending = read_pending()
    ride = next((r for r in pending if r['id'] == ride_id), None)
    if ride and driver_name:
        append_csv({
            'passenger': ride['passenger'], 'driver': driver_name,
            'pickup':    ride['pickup'],    'drop':   ride['drop'],
            'distance':  ride['distance'],  'fare':   calculate_fare(ride['distance']),
        })
        write_pending([r for r in pending if r['id'] != ride_id])
    return redirect(url_for('driver'))


@app.route('/admin')
def admin():
    rides, n, rev, avg, dst, ud, up = read_data()
    pending = read_pending()
    drivers = read_drivers()
    return render_template('admin.html', rides=rides,
        total_rides=n, total_revenue=round(rev,2), avg_fare=avg,
        total_distance=round(dst,2), unique_drivers=ud, unique_passengers=up,
        pending=pending, drivers=drivers, locations=LOCATIONS,
    )


@app.route('/add-driver', methods=['POST'])
def add_driver():
    name    = request.form.get('driver_name', '').strip()
    vehicle = request.form.get('vehicle', '').strip() or 'Unknown Vehicle'
    if name:
        drivers = read_drivers()
        # Avoid duplicates (case-insensitive)
        if not any(d['name'].lower() == name.lower() for d in drivers):
            drivers.append({'name': name, 'vehicle': vehicle})
            write_drivers(drivers)
    return redirect(url_for('admin') + '#tab-drivers')


@app.route('/api/rides')
def api_rides():
    rides, *_ = read_data()
    return jsonify(rides)


@app.route('/api/pending')
def api_pending():
    return jsonify(read_pending())


@app.route('/api/locations')
def api_locations():
    return jsonify(LOCATIONS)


if __name__ == '__main__':
    app.run(debug=True)