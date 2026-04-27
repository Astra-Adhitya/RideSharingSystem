import sys

with open('e:/codes/AntiGravity Google/RideSharingSystem/frontend/script.js', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace riders loop
content = content.replace('''        const ridersContainer = document.getElementById('admin-riders-list');
        const removeSelect = document.getElementById('admin-remove-select');
        ridersContainer.innerHTML = '';
        removeSelect.innerHTML = '<option value="">Select a user...</option>';
        data.riders.forEach(r => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            div.innerHTML = `
                <div class="card-info" style="width: 100%; display: flex; justify-content: space-between; align-items: center;">
                    <h4 style="font-size:14px; margin-bottom:0;">${r.name} <br><span style="font-size:11px; color:#94a3b8; font-weight:normal;">ID: ${r.id}</span></h4>
                    <span style="color:#a3e635; font-size:13px; font-weight:bold;">₹${r.spent.toFixed(2)} spent</span>
                </div>`;
            ridersContainer.appendChild(div);

            const opt = document.createElement('option');
            opt.value = r.id;
            opt.innerText = `Rider: ${r.name} (${r.id})`;
            removeSelect.appendChild(opt);
        });''', '''        const ridersContainer = document.getElementById('admin-riders-list');
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
        });''')

# Replace drivers loop
content = content.replace('''        const driversContainer = document.getElementById('admin-drivers-list');
        driversContainer.innerHTML = '';
        data.drivers.forEach(d => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            div.innerHTML = `
                <div class="card-info" style="width: 100%; display: flex; justify-content: space-between; align-items: center;">
                    <h4 style="font-size:14px; margin-bottom:0;">${d.name} <br><span style="font-size:11px; color:#94a3b8; font-weight:normal;">${d.vehicle}</span></h4>
                    <span style="color:#a3e635; font-size:13px; font-weight:bold;">₹${d.earned.toFixed(2)} earned</span>
                </div>`;
            driversContainer.appendChild(div);

            const opt = document.createElement('option');
            opt.value = d.id;
            opt.innerText = `Driver: ${d.name} (${d.id})`;
            removeSelect.appendChild(opt);
        });''', '''        const driversContainer = document.getElementById('admin-drivers-list');
        driversContainer.innerHTML = '';
        data.drivers.forEach(d => {
            const div = document.createElement('div');
            div.className = 'card';
            div.style.padding = '10px';
            div.innerHTML = `
                <div class="card-info" style="width: 100%; display: flex; justify-content: space-between; align-items: center;">
                    <h4 style="font-size:14px; margin-bottom:0;">${d.name} <br><span style="font-size:11px; color:#94a3b8; font-weight:normal;">${d.vehicle}</span></h4>
                    <div style="display:flex; align-items:center; gap: 10px;">
                        <span style="color:#a3e635; font-size:13px; font-weight:bold;">₹${d.earned.toFixed(2)} earned</span>
                        <button onclick="removeUser('${d.id}')" class="remove-btn">Remove</button>
                    </div>
                </div>`;
            driversContainer.appendChild(div);
        });''')

# Replace functions
content = content.replace('''function toggleRemoveSection() {
    const sec = document.getElementById('admin-remove-section');
    sec.style.display = sec.style.display === 'none' ? 'block' : 'none';
}

async function executeRemove() {
    const userId = document.getElementById('admin-remove-select').value;
    if(!userId) return alert("Please select a user to remove.");
    
    if(!confirm(`Are you sure you want to permanently remove user: ${userId}?`)) return;
    
    try {
        const res = await fetch('/api/admin/removeUser', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({userId})
        });
        const data = await res.json();
        if(data.success) {
            document.getElementById('admin-remove-section').style.display = 'none';
            fetchAdminStats(); // refresh dashboard
        } else {
            alert(data.message || "Failed to remove user");
        }
    } catch(e) {
        console.error(e);
        alert("Error removing user");
    }
}''', '''function toggleRemoveMode() {
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
}''')

with open('e:/codes/AntiGravity Google/RideSharingSystem/frontend/script.js', 'w', encoding='utf-8') as f:
    f.write(content)
print('Done!')
