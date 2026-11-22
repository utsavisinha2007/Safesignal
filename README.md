# SafeSignal — Real-Time Women's Safety & Emergency Response Platform

SafeSignal is a mission-critical, full-stack personal safety and incident management platform designed for student campuses and solo travelers. Powered by a high-performance **C++17 safety engine** and connected to a modern **React + Tailwind CSS** web application, SafeSignal demonstrates how foundational Data Structures and Algorithms (DSA) solve real-world hazard detection, triage, and emergency dispatch challenges.

---

## 🌟 Architecture & Dual-Interface System

SafeSignal provides two distinct interfaces powered by the exact same C++ safety backend:

```
                            ┌──────────────────────────────────┐
                            │    SafeSignal Safety Engine      │
                            │      (Native C++17 Core)         │
                            │                                  │
                            │  • Sliding-Window Risk Scoring   │
                            │  • Checkpoint Route Tracking     │
                            │  • 5-min Grace Safety Check-ins  │
                            │  • 3-min Verification Timeouts   │
                            │  • Priority Queue Dispatch Heap  │
                            └────────────────┬─────────────────┘
                                             │
                      ┌──────────────────────┴──────────────────────┐
                      ▼                                             ▼
        ┌───────────────────────────┐                 ┌───────────────────────────┐
        │  SafeSignal Console App   │                 │   SafeSignal REST API     │
        │     (SafeSignal.exe)      │                 │  (SafeSignalServer.exe)   │
        ├───────────────────────────┤                 ├───────────────────────────┤
        │ • User Mode (8 Options)   │                 │ • cpp-httplib (Port 8080) │
        │ • Demo Mode (30 Options)  │                 │ • nlohmann/json (CORS)    │
        └───────────────────────────┘                 │ • Mutex Synchronized      │
                                                      └─────────────┬─────────────┘
                                                                    │ HTTP / JSON
                                                                    ▼
                                                      ┌───────────────────────────┐
                                                      │    React Web Portal       │
                                                      │  (http://localhost:5173)  │
                                                      ├───────────────────────────┤
                                                      │ 1. Dashboard              │
                                                      │ 2. Journey Manager        │
                                                      │ 3. Emergency Contacts     │
                                                      │ 4. Safety Check-ins       │
                                                      │ 5. Emergency Center (SOS) │
                                                      │ 6. Activity History       │
                                                      │ 7. Demonstration Lab      │
                                                      └───────────────────────────┘
```

---

## 🧠 Core Data Structures & Algorithms (DSA)

Every safety decision runs natively in C++:

| Subsystem | Primary Data Structure | Algorithmic Mechanism | Time Complexity |
|---|---|---|---|
| **RiskAnalyzer** | `std::deque<TimestampedEvent>` | 10-minute sliding time window with multi-day rollover | $O(1)$ amortized insertion & pruning |
| **ResponseManager** | `std::priority_queue<Incident>` | Severity-based emergency triage Max-Heap | $O(\log n)$ insertion & extraction |
| **Incident Registry** | `std::unordered_map<int, Incident>` | $O(1)$ lookup, lifecycle transitions & duplicate suppression | $O(1)$ average-case lookup |
| **VerificationManager** | `std::unordered_map<int, VerificationRequest>` | $O(1)$ verification tracking with 3-minute timeout window | $O(1)$ average-case lookup |
| **JourneyManager** | `std::vector<std::string>` | Checkpoint progression with forward-lookahead skip recovery | $O(m)$ lookahead search |
| **CheckInManager** | `std::vector<CheckIn>` | Scheduled check-ins with 5-minute grace period enforcement | $O(c)$ deadline evaluation |
| **Contact Directory** | `std::vector<Contact>` | 3-tier emergency directory (Tier 1 Primary, Tier 2 Secondary, Tier 3 Police) | $O(n)$ priority filtering |

---

## 🛠️ Simulation Model Notice

SafeSignal is designed to run deterministically for education and academic demonstration without physical cellular modems or real GPS hardware:
- **Simulated GPS**: Waypoints and locations are submitted along planned checkpoints.
- **Simulated Clock**: The clock advances via timestamp updates (`HH:MM` or `HH:MM:SS`) to demonstrate grace periods and timeouts.
- **Simulated Dispatch**: Contact notifications (SMS/Calls) and security alerts are rendered directly to the console and audit logs.

---

## 🚀 Quick Start Guide (Windows / PowerShell)

### Option A: Launch Web Application (One-Click)
```powershell
.\start_safesignal.ps1
```
This automatically starts `SafeSignalServer.exe` on `127.0.0.1:8080` and launches the Vite React frontend on `http://localhost:5173`.

---

### Option B: Manual Multi-Terminal Startup

#### Terminal 1 — C++ REST API Server
```powershell
# Compile Server (if not already compiled)
g++ -std=c++17 -O2 -o SafeSignalServer.exe server/server_main.cpp server/api_handlers.cpp src/*.cpp -Iinclude -Iserver -lws2_32

# Run Server
.\SafeSignalServer.exe
```

#### Terminal 2 — React Web Frontend
```powershell
cd web
npm install
npm run dev
```
Open **`http://localhost:5173`** in your browser.

#### Terminal 3 (Optional) — Standalone Console Application
```powershell
# Run original Dual-Mode Console Application
.\SafeSignal.exe
```

---

## 🧪 Testing & Verification

SafeSignal includes 7 C++ test suites and 1 comprehensive REST API integration test suite covering **572 automated assertions**:

### 1. Run All 7 C++ Backend Test Suites (550 / 550 Passed)
```powershell
.\test_contact.exe
.\test_checkin.exe
.\test_risk_analyzer.exe
.\test_verification.exe
.\test_safety_system_verification.exe
.\test_dispatch_lifecycle.exe
.\test_end_to_end_system.exe
```

### 2. Run C++ REST API Integration Test Suite (22 / 22 Passed)
```powershell
powershell -ExecutionPolicy Bypass -File .\tests\test_api_server.ps1
```

### 3. Build Frontend for Production
```powershell
cd web
npm run build
```

---

## 📂 Project Directory Structure

```
safesignal/
├── docs/
│   ├── architecture.md                # System Architecture & State Machine specs
│   └── api_specification.md           # REST API Endpoint reference
├── include/                           # C++ Header Files
│   ├── CheckInManager.h
│   ├── Contact.h
│   ├── Event.h
│   ├── Incident.h
│   ├── JourneyManager.h
│   ├── ResponseManager.h
│   ├── RiskAnalyzer.h
│   ├── SafetySystem.h
│   └── VerificationManager.h
├── src/                               # C++ Source Files
│   ├── CheckInManager.cpp
│   ├── Contact.cpp
│   ├── Event.cpp
│   ├── Incident.cpp
│   ├── JourneyManager.cpp
│   ├── ResponseManager.cpp
│   ├── RiskAnalyzer.cpp
│   ├── SafetySystem.cpp
│   └── VerificationManager.cpp
├── server/                            # C++ REST API Server
│   ├── third_party/
│   │   ├── httplib.h                  # cpp-httplib (v0.18.3, MIT)
│   │   └── json.hpp                   # nlohmann/json (v3.11.3, MIT)
│   ├── api_handlers.h
│   ├── api_handlers.cpp
│   └── server_main.cpp
├── web/                               # React + Vite + Tailwind CSS Frontend
│   ├── package.json
│   ├── vite.config.js
│   ├── index.html
│   └── src/
│       ├── components/                # Navbar, Sidebar, StatusBadge, DisclosureBanner
│       ├── pages/                     # Dashboard, Journey, Contacts, Check-ins, SOS, History, Demo
│       ├── services/                  # api.js client
│       └── App.jsx
├── tests/                             # Unit & Integration Tests
│   ├── test_checkin.cpp
│   ├── test_contact.cpp
│   ├── test_dispatch_lifecycle.cpp
│   ├── test_end_to_end_system.cpp
│   ├── test_risk_analyzer.cpp
│   ├── test_safety_system_verification.cpp
│   ├── test_verification.cpp
│   └── test_api_server.ps1            # 22-step API test suite
├── main.cpp                           # Console Entry Point (User + Demo Mode)
├── start_safesignal.ps1               # Unified startup launcher
└── README.md
```
