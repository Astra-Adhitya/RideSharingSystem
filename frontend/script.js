let currentUserId = null;
let currentUserRole = null;

function toggleAuth(tab) {
    document.getElementById('btn-login-tab').classList.remove('active');
    document.getElementById('btn-register-tab').classList.remove('active');
    document.getElementById(`btn-${tab}-tab`).classList.add('active');

    if (tab === 'login') {
        document.getElementById('login-form').style.display = 'block';
        document.getElementById('register-form').style.display = 'none';
    } else {
        document.getElementById('login-form').style.display = 'none';
        document.getElementById('register-form').style.display = 'block';
    }
    document.getElementById('auth-error').innerText = '';
}

function toggleVehicleField() {
    const r = document.getElementById('reg-role').value;
    document.getElementById('reg-vehicle').style.display = (r === 'driver') ? 'block' : 'none';
}

async function login() {
    const id = document.getElementById('login-id').value;
    if(!id) return showError("Enter User ID");

    try {
        const res = await fetch('/api/login', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({id})
        });
        const data = await res.json();
        if(data.success) {
            currentUserId = id;
            currentUserRole = data.role;
            showDashboard();
        } else {
            showError(data.message || "Login failed");
        }
    } catch (e) {
        showError("Server error.");
    }
}

async function register() {
    const id = document.getElementById('reg-id').value;
    const name = document.getElementById('reg-name').value;
    const phone = document.getElementById('reg-phone').value;
    const role = document.getElementById('reg-role').value;
    const vehicle = document.getElementById('reg-vehicle').value;

    if(!id || !name || !phone) return showError("Fill all required fields");
    if(role === 'driver' && !vehicle) return showError("Enter vehicle details");

    try {
        const res = await fetch('/api/register', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({id, name, phone, role, vehicle})
        });
        const data = await res.json();
        if(data.success) {
            alert("Registration successful. Please login.");
            toggleAuth('login');
        } else {
            showError(data.message || "Registration failed");
        }
    } catch(e) {
        showError("Server error");
    }
}

function showError(msg) {
    document.getElementById('auth-error').innerText = msg;
}

function showDashboard() {
    document.getElementById('auth-section').style.display = 'none';
    document.getElementById('dashboard-section').style.display = 'block';
    document.getElementById('user-greeting').innerText = `Hello, ${currentUserId} (${currentUserRole})`;

    if(currentUserRole === 'rider') {
        document.getElementById('rider-view').style.display = 'block';
        document.getElementById('driver-view').style.display = 'none';
        document.getElementById('admin-view').style.display = 'none';
        document.getElementById('history-wrapper').style.display = 'block';
        fetchHistory();
    } else if(currentUserRole === 'driver') {
        document.getElementById('rider-view').style.display = 'none';
        document.getElementById('driver-view').style.display = 'block';
        document.getElementById('admin-view').style.display = 'none';
        document.getElementById('history-wrapper').style.display = 'block';
        fetchHistory();
    } else if(currentUserRole === 'admin') {
        document.getElementById('rider-view').style.display = 'none';
        document.getElementById('driver-view').style.display = 'none';
        document.getElementById('history-wrapper').style.display = 'none';
        document.getElementById('admin-view').style.display = 'block';
        fetchAdminStats();
    }
}

function logout() {
    currentUserId = null;
    currentUserRole = null;
    document.getElementById('auth-section').style.display = 'block';
    document.getElementById('dashboard-section').style.display = 'none';
    document.getElementById('login-id').value = '';
}

async function fetchDrivers() {
    try {
        const res = await fetch('/api/availableDrivers');
        const drivers = await res.json();
        
        const container = document.getElementById('drivers-list');
        container.innerHTML = '';

        if(drivers.length === 0) {
            container.innerHTML = '<p>No drivers available right now.</p>';
            return;
        }

        drivers.forEach(d => {
            const div = document.createElement('div');
            div.className = 'card';
            div.innerHTML = `
                <div class="card-info">
                    <h4>${d.name}</h4>
                    <p>Vehicle: ${d.vehicle}</p>
                </div>
                <button onclick="bookRide('${d.id}')">Book</button>
            `;
            container.appendChild(div);
        });
    } catch(e) {
        console.error(e);
    }
}

async function bookRide(driverId) {
    const pickup = document.getElementById('pickup').value;
    const drop = document.getElementById('drop').value;
    const distanceStr = document.getElementById('distance').value;
    const distance = parseFloat(distanceStr);

    if(!pickup || !drop || isNaN(distance)) {
        alert("Please fill pickup, drop, and valid distance first.");
        return;
    }

    try {
        const res = await fetch('/api/bookRide', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({
                riderId: currentUserId,
                driverId: driverId,
                pickup: pickup,
                drop: drop,
                distance: distance
            })
        });
        const data = await res.json();
        if(data.success) {
            alert(`Ride booked successfully! Fare: $${data.fare.toFixed(2)}`);
            fetchHistory();
            fetchDrivers(); // refresh list
        } else {
            alert(data.message || "Booking failed");
        }
    } catch(e) {
        console.error(e);
    }
}

async function fetchHistory() {
    if(!currentUserId) return;
    try {
        const res = await fetch(`/api/rideHistory/${currentUserId}`);
        const history = await res.json();
        
        const container = document.getElementById('history-list');
        container.innerHTML = '';

        if(history.length === 0) {
            container.innerHTML = '<p style="color:#64748b;">No past rides.</p>';
            return;
        }

        // reverse so earliest is at top or latest at top (latest at top is standard)
        history.reverse().forEach(r => {
            const div = document.createElement('div');
            div.className = 'card';
            div.innerHTML = `
                <div class="card-info" style="width: 100%;">
                    <div style="display:flex; justify-content:space-between;">
                        <h4>Ride ${r.rideId}</h4>
                        <span style="color:#a3e635; font-weight:bold;">$${r.fare.toFixed(2)}</span>
                    </div>
                    <p>${r.pickup} → ${r.drop} (${r.distance}km)</p>
                    <p style="font-size:12px; margin-top:5px; color:#64748b;">
                       Role in ride: ${r.driverId === currentUserId ? 'Driver' : 'Rider'} | 
                       With: ${r.driverId === currentUserId ? r.riderId : r.driverId}
                    </p>
                </div>
            `;
            container.appendChild(div);
        });
    } catch(e) {
        console.error(e);
    }
}

async function fetchAdminStats() {
    try {
        const res = await fetch('/api/admin/stats');
        const data = await res.json();
        
        document.getElementById('admin-income').innerText = `$${data.totalIncome.toFixed(2)}`;
        document.getElementById('admin-rides').innerText = data.totalRides;

        const ridersContainer = document.getElementById('admin-riders-list');
        ridersContainer.innerHTML = '';
        data.riders.forEach(r => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            div.innerHTML = `<div class="card-info"><h4 style="font-size:14px;">${r.name}</h4><p style="font-size:12px;">ID: ${r.id}</p></div>`;
            ridersContainer.appendChild(div);
        });

        const driversContainer = document.getElementById('admin-drivers-list');
        driversContainer.innerHTML = '';
        data.drivers.forEach(d => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            div.innerHTML = `<div class="card-info"><h4 style="font-size:14px;">${d.name}</h4><p style="font-size:12px;">ID: ${d.id} | ${d.vehicle}</p></div>`;
            driversContainer.appendChild(div);
        });

    } catch(e) {
        console.error(e);
    }
}
