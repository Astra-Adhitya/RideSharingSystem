let currentUserId = null;
let currentUserRole = null;
let riderChartInstance = null;
let driverChartInstance = null;
let adminChartInstance = null;

// ===== FLOATING PARTICLES =====
(function createParticles() {
    const container = document.getElementById('particles');
    if(!container) return;
    for(let i = 0; i < 30; i++) {
        const p = document.createElement('div');
        p.className = 'particle';
        p.style.left = Math.random() * 100 + '%';
        p.style.animationDuration = (8 + Math.random() * 18) + 's';
        p.style.animationDelay = (Math.random() * 12) + 's';
        p.style.opacity = (0.2 + Math.random() * 0.5);
        container.appendChild(p);
    }
})();

// ===== ANIMATED COUNTER =====
function animateValue(el, start, end, duration) {
    const isINR = el.innerText.includes('₹');
    let startTime = null;
    function step(ts) {
        if(!startTime) startTime = ts;
        const progress = Math.min((ts - startTime) / duration, 1);
        const eased = 1 - Math.pow(1 - progress, 3);
        const current = start + (end - start) * eased;
        el.innerText = isINR ? `₹${current.toFixed(2)}` : Math.round(current);
        if(progress < 1) requestAnimationFrame(step);
    }
    requestAnimationFrame(step);
}

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

function toggleRiderTab(tab) {
    document.getElementById('btn-rider-book-tab').classList.remove('active');
    document.getElementById('btn-rider-history-tab').classList.remove('active');
    document.getElementById(`btn-rider-${tab}-tab`).classList.add('active');

    if (tab === 'book') {
        document.getElementById('rider-book-section').style.display = 'block';
        document.getElementById('rider-history-section').style.display = 'none';
    } else {
        document.getElementById('rider-book-section').style.display = 'none';
        document.getElementById('rider-history-section').style.display = 'block';
    }
}

function toggleDriverTab(tab) {
    document.getElementById('btn-driver-status-tab').classList.remove('active');
    document.getElementById('btn-driver-history-tab').classList.remove('active');
    document.getElementById(`btn-driver-${tab}-tab`).classList.add('active');

    if (tab === 'status') {
        document.getElementById('driver-status-section').style.display = 'block';
        document.getElementById('driver-history-section').style.display = 'none';
    } else {
        document.getElementById('driver-status-section').style.display = 'none';
        document.getElementById('driver-history-section').style.display = 'block';
    }
}

async function toggleDriverAvailability() {
    const toggle = document.getElementById('driver-toggle');
    const label = document.getElementById('driver-status-label');
    const heading = document.getElementById('driver-status-heading');
    const desc = document.getElementById('driver-status-desc');
    
    try {
        const res = await fetch('/api/toggleAvailability', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({userId: currentUserId})
        });
        const data = await res.json();
        if(data.success) {
            const isOnline = data.available;
            toggle.checked = isOnline;
            if(isOnline) {
                label.className = 'status-online';
                label.innerText = 'Online & Available';
                heading.innerText = 'You are currently online.';
                desc.innerText = 'Riders can see your vehicle details and book you for a ride. Stay available to earn more!';
            } else {
                label.className = '';
                label.style.cssText = 'color:#94a3b8; font-size:14px;';
                label.innerText = 'Offline';
                heading.innerText = 'You are currently offline.';
                desc.innerText = 'Riders cannot see or book you. Toggle the switch to go back online.';
            }
        }
    } catch(e) {
        console.error(e);
        toggle.checked = !toggle.checked;
    }
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
    document.querySelector('.container').classList.add('dashboard-mode');
    document.getElementById('auth-section').style.display = 'none';
    document.getElementById('dashboard-section').style.display = 'block';
    
    const roleEmoji = currentUserRole === 'rider' ? '🚗' : currentUserRole === 'driver' ? '🏎️' : '⚡';
    const roleColors = {
        rider: { bg: 'rgba(245, 158, 11, 0.12)', border: 'rgba(245, 158, 11, 0.3)', text: '#fbbf24' },
        driver: { bg: 'rgba(34, 197, 94, 0.12)', border: 'rgba(34, 197, 94, 0.3)', text: '#4ade80' },
        admin: { bg: 'rgba(13, 148, 136, 0.12)', border: 'rgba(13, 148, 136, 0.3)', text: '#5eead4' }
    };
    const rc = roleColors[currentUserRole] || roleColors.rider;
    document.getElementById('user-greeting').innerHTML = `
        ${roleEmoji} Welcome, <span style="background: linear-gradient(135deg, #fff, var(--primary-light)); -webkit-background-clip: text; -webkit-text-fill-color: transparent;">${currentUserId}</span>
        <span style="display:inline-flex; align-items:center; margin-left:10px; padding:4px 12px; background:${rc.bg}; border:1px solid ${rc.border}; border-radius:100px; font-size:12px; color:${rc.text}; font-weight:600; letter-spacing:0.5px; -webkit-text-fill-color:${rc.text};">${currentUserRole.toUpperCase()}</span>
    `;

    if(currentUserRole === 'rider') {
        document.getElementById('rider-view').style.display = 'block';
        document.getElementById('driver-view').style.display = 'none';
        document.getElementById('admin-view').style.display = 'none';
        fetchHistory();
    } else if(currentUserRole === 'driver') {
        document.getElementById('rider-view').style.display = 'none';
        document.getElementById('driver-view').style.display = 'block';
        document.getElementById('admin-view').style.display = 'none';
        fetchHistory();
    } else if(currentUserRole === 'admin') {
        document.getElementById('rider-view').style.display = 'none';
        document.getElementById('driver-view').style.display = 'none';
        document.getElementById('admin-view').style.display = 'block';
        fetchAdminStats();
    }
}

function logout() {
    currentUserId = null;
    currentUserRole = null;
    document.querySelector('.container').classList.remove('dashboard-mode');
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
            alert(`Ride booked successfully! Fare: ₹${data.fare.toFixed(2)}`);
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
        
        const containerId = currentUserRole === 'rider' ? 'rider-history-list' : 'driver-history-list';
        const container = document.getElementById(containerId);
        container.innerHTML = '';

        if(history.length === 0) {
            container.innerHTML = '<p style="color:#64748b;">No past rides.</p>';
            if(currentUserRole === 'rider') {
                document.getElementById('rider-total-rides').innerText = '0';
                document.getElementById('rider-total-spent').innerText = '₹0.00';
            } else if(currentUserRole === 'driver') {
                document.getElementById('driver-total-rides').innerText = '0';
                document.getElementById('driver-total-earned').innerText = '₹0.00';
            }
            return;
        }

        let totalRides = history.length;
        let totalMoney = 0;
        let shortRides = 0;
        let longRides = 0;

        // reverse so earliest is at top or latest at top (latest at top is standard)
        history.reverse().forEach(r => {
            totalMoney += r.fare;
            if(r.distance < 5) shortRides++;
            else longRides++;

            const div = document.createElement('div');
            div.className = 'card';
            div.innerHTML = `
                <div class="card-info" style="width: 100%;">
                    <div style="display:flex; justify-content:space-between;">
                        <h4>Ride ${r.rideId}</h4>
                        <span style="color:#a3e635; font-weight:bold;">₹${r.fare.toFixed(2)}</span>
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

        // Update charts and stats
        const chartData = {
            labels: ['Short Rides (<5km)', 'Long Rides (>=5km)'],
            datasets: [{
                data: [shortRides, longRides],
                backgroundColor: ['#8b5cf6', '#34d399'],
                borderWidth: 0,
                hoverOffset: 4
            }]
        };
        const chartOptions = {
            plugins: {
                legend: { labels: { color: '#f8fafc' } }
            }
        };

        if(currentUserRole === 'rider') {
            document.getElementById('rider-total-rides').innerText = totalRides;
            document.getElementById('rider-total-spent').innerText = `₹${totalMoney.toFixed(2)}`;
            if(riderChartInstance) riderChartInstance.destroy();
            const ctx = document.getElementById('riderPieChart').getContext('2d');
            riderChartInstance = new Chart(ctx, { type: 'pie', data: chartData, options: chartOptions });
        } else if(currentUserRole === 'driver') {
            document.getElementById('driver-total-rides').innerText = totalRides;
            document.getElementById('driver-total-earned').innerText = `₹${totalMoney.toFixed(2)}`;
            if(driverChartInstance) driverChartInstance.destroy();
            const ctx = document.getElementById('driverPieChart').getContext('2d');
            driverChartInstance = new Chart(ctx, { type: 'pie', data: chartData, options: chartOptions });
        }

    } catch(e) {
        console.error(e);
    }
}

async function fetchAdminStats() {
    try {
        const res = await fetch('/api/admin/stats');
        const data = await res.json();
        
        document.getElementById('admin-income').innerText = `₹${data.totalIncome.toFixed(2)}`;
        document.getElementById('admin-rides').innerText = data.totalRides;

        const ridersContainer = document.getElementById('admin-riders-list');
        ridersContainer.innerHTML = '';
        data.riders.forEach(r => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            div.innerHTML = `
                <div class="card-info" style="width: 100%; display: flex; justify-content: space-between; align-items: center;">
                    <h4 style="font-size:14px; margin-bottom:0;">${r.name} <br><span style="font-size:11px; color:#94a3b8; font-weight:normal;">ID: ${r.id}</span></h4>
                    <div style="display:flex; align-items:center; gap: 10px;">
                        <span style="color:#a3e635; font-size:13px; font-weight:bold;">₹${r.spent.toFixed(2)} spent</span>
                        <button onclick="removeUser('${r.id}')" class="remove-btn">Remove</button>
                    </div>
                </div>`;
            ridersContainer.appendChild(div);
        });

        const driversContainer = document.getElementById('admin-drivers-list');
        driversContainer.innerHTML = '';
        data.drivers.forEach(d => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            const statusClass = d.available ? 'online' : 'offline';
            const statusText = d.available ? 'Online' : 'Offline';
            div.innerHTML = `
                <div class="card-info" style="width: 100%; display: flex; justify-content: space-between; align-items: center;">
                    <h4 style="font-size:14px; margin-bottom:0;"><span class="status-dot ${statusClass}"></span>${d.name} <br><span style="font-size:11px; color:#94a3b8; font-weight:normal;">${d.vehicle} • ${statusText}</span></h4>
                    <div style="display:flex; align-items:center; gap: 10px;">
                        <span style="color:#a3e635; font-size:13px; font-weight:bold;">₹${d.earned.toFixed(2)}</span>
                        <button onclick="removeUser('${d.id}')" class="remove-btn">Remove</button>
                    </div>
                </div>`;
            driversContainer.appendChild(div);
        });

        document.getElementById('admin-rider-count').innerText = data.riders.length;
        document.getElementById('admin-driver-count').innerText = data.drivers.length;

        // Animate the stat values
        const incomeEl = document.getElementById('admin-income');
        const ridesEl = document.getElementById('admin-rides');
        animateValue(incomeEl, 0, data.totalIncome, 800);
        animateValue(ridesEl, 0, data.totalRides, 800);

        const driversWithEarnings = data.drivers.filter(d => d.earned > 0).sort((a, b) => b.earned - a.earned);
        const topDrivers = driversWithEarnings.slice(0, 12);
        const chartLabels = topDrivers.map(d => d.name);
        const chartDataPoints = topDrivers.map(d => d.earned);
        const palette = [
            '#8b5cf6', '#34d399', '#f43f5e', '#fbbf24', '#38bdf8',
            '#a855f7', '#10b981', '#f97316', '#fb7185', '#2dd4bf',
            '#c084fc', '#4ade80'
        ];

        if(adminChartInstance) adminChartInstance.destroy();
        const ctx = document.getElementById('adminPieChart').getContext('2d');
        adminChartInstance = new Chart(ctx, {
            type: 'doughnut',
            data: {
                labels: chartLabels.length ? chartLabels : ['No Earnings'],
                datasets: [{
                    data: chartDataPoints.length ? chartDataPoints : [1],
                    backgroundColor: palette,
                    borderWidth: 0,
                    hoverOffset: 4
                }]
            },
            options: {
                plugins: {
                    legend: { display: false },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                let label = context.label || '';
                                if (label) label += ': ';
                                if (context.raw !== null) label += '₹' + parseFloat(context.raw).toFixed(2);
                                return label;
                            }
                        }
                    }
                }
            }
        });

    } catch(e) {
        console.error(e);
    }
}

function toggleRemoveMode() {
    const adminView = document.getElementById('admin-view');
    adminView.classList.toggle('remove-mode');
    
    const btn = document.getElementById('btn-manage-users');
    if (adminView.classList.contains('remove-mode')) {
        btn.innerText = 'Done Managing';
        btn.style.background = 'rgba(239,68,68,0.2)';
    } else {
        btn.innerText = 'Manage Users';
        btn.style.background = 'rgba(255,255,255,0.05)';
    }
}

async function removeUser(userId) {
    if(!confirm(`Are you sure you want to permanently remove user: ${userId}?`)) return;
    try {
        const res = await fetch('/api/admin/removeUser', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({userId})
        });
        const data = await res.json();
        if(data.success) {
            fetchAdminStats(); // refresh dashboard
        } else {
            alert(data.message || "Failed to remove user");
        }
    } catch(e) {
        console.error(e);
        alert("Error removing user");
    }
}
