# SafeSignal REST API Specification

**Version:** 1.0.0  
**Host:** `127.0.0.1:8080`  
**Protocol:** HTTP/1.1  
**Content-Type:** `application/json`  
**CORS:** Enabled for local browser origins (e.g. `http://localhost:5173`)  

---

## 1. Overview

The SafeSignal API server (`SafeSignalServer.exe`) bridges web and client applications to the real C++ Data Structures & Algorithms (DSA) backend. All algorithms for risk scoring, sliding time windows (`std::deque`), emergency triage (`std::priority_queue`), route deviation detection, and incident lifecycle state machines run natively in C++.

Every request accessing or modifying system state is synchronized via `std::mutex stateMutex` to ensure complete thread-safety across concurrent client requests.

---

## 2. API Endpoints

### 2.1 System & Diagnostic Endpoints

#### `GET /api/health`
Checks server availability.
- **Response `200 OK`**:
```json
{
  "status": "ok",
  "server": "SafeSignalServer",
  "version": "1.0"
}
```

#### `GET /api/status`
Retrieves master real-time safety status from `SafetySystem`, `RiskAnalyzer`, `JourneyManager`, and `ResponseManager`.
- **Response `200 OK`**:
```json
{
  "riskScore": 0,
  "riskLevel": "Normal",
  "baseScore": 0,
  "frequencyBonus": 0,
  "patternBonus": 0,
  "isRiskHigh": false,
  "eventCount": 0,
  "detectedPatterns": [],
  "scoringBreakdown": [],
  "frequencyMap": {},
  "latestTimestamp": "00:00:00",
  "hasActiveJourney": false,
  "hasActiveVerification": false,
  "activeVerificationID": -1,
  "contactCount": 3,
  "activeIncidentCount": 0,
  "pendingIncidentCount": 0,
  "pendingCheckIns": 0,
  "missedCheckIns": 0,
  "totalCheckIns": 0
}
```

---

### 2.2 Journey Management Endpoints

#### `GET /api/journey`
Returns active route, checkpoints, and current progression.
- **Response `200 OK`**:
```json
{
  "active": true,
  "startLocation": "Science Library",
  "destination": "Dormitory Tower B",
  "expectedArrivalTime": "18:30:00",
  "route": ["Main Gate", "Campus Quad", "Dining Hall"],
  "currentCheckpointIndex": 1,
  "nextExpectedCheckpoint": "Campus Quad",
  "totalCheckpoints": 3
}
```

#### `POST /api/journey/start`
Starts a new route-monitored journey.
- **Request Body**:
```json
{
  "startLocation": "Science Library",
  "destination": "Dormitory Tower B",
  "eta": "18:30:00",
  "route": ["Main Gate", "Campus Quad", "Dining Hall"]
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Journey started successfully"
}
```

#### `POST /api/journey/location`
Submits simulated GPS / location milestone. The C++ `JourneyManager` checks for checkpoints or route deviation with forward-lookahead recovery.
- **Request Body**:
```json
{
  "location": "Campus Quad",
  "timestamp": "18:15:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Location updated"
}
```

#### `POST /api/journey/end`
Safely ends the current journey and clears associated check-ins.
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Journey ended"
}
```

---

### 2.3 Emergency Contact Endpoints

#### `GET /api/contacts`
Lists all registered emergency contacts grouped by priority tiers.
- **Response `200 OK`**:
```json
{
  "contacts": [
    {
      "id": 101,
      "name": "Sarah Connor",
      "phone": "555-0199",
      "relationship": "Mother",
      "priority": 1,
      "priorityDescription": "Primary Contact"
    }
  ],
  "count": 1
}
```

#### `POST /api/contacts`
Registers a new emergency contact.
- **Request Body**:
```json
{
  "id": 102,
  "name": "Campus Police Desk",
  "phone": "911",
  "relationship": "Campus Security",
  "priority": 3
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Contact registered successfully"
}
```

---

### 2.4 Safety Check-in Endpoints

#### `GET /api/checkins`
Returns all scheduled check-in records and statistics.
- **Response `200 OK`**:
```json
{
  "checkins": [
    {
      "id": 1,
      "scheduledTime": "18:15:00",
      "description": "Crossing Bridge",
      "location": "North Bridge",
      "isCompleted": false,
      "isMissed": false,
      "status": "PENDING"
    }
  ],
  "total": 1,
  "pending": 1,
  "completed": 0,
  "lateConfirmations": 0,
  "missed": 0,
  "gracePeriodMinutes": 5
}
```

#### `POST /api/checkins/schedule`
Schedules a safety check-in (requires active journey).
- **Request Body**:
```json
{
  "time": "18:20:00",
  "description": "Passing Student Union",
  "location": "Student Union"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "checkInID": 2,
  "message": "Check-in scheduled"
}
```

#### `POST /api/checkins/confirm`
User confirms they are safe for a scheduled check-in.
- **Request Body**:
```json
{
  "checkInID": 1
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Check-in confirmed"
}
```

---

### 2.5 Emergency & Verification Endpoints

#### `POST /api/emergency/verify/confirm-safe`
User responds to a safety verification request: "I AM SAFE". Escalation is averted.
- **Request Body**:
```json
{
  "requestID": 5000
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Safety confirmed — escalation averted"
}
```

#### `POST /api/emergency/verify/request-help`
User responds to verification request: "I NEED HELP". Immediate emergency dispatch initiates.
- **Request Body**:
```json
{
  "requestID": 5000
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Emergency help dispatched"
}
```

#### `POST /api/emergency/sos`
One-touch SOS distress trigger. Escalates directly to priority dispatch.
- **Request Body**:
```json
{
  "location": "Campus Quad",
  "timestamp": "18:22:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "SOS event logged and emergency dispatch initiated",
  "eventID": 9001
}
```

#### `GET /api/incidents/active`
Lists currently active emergencies undergoing tiered notification dispatch.
- **Response `200 OK`**:
```json
{
  "incidents": [
    {
      "id": 1,
      "description": "EMERGENCY ESCALATION: User requested immediate help",
      "location": "Campus Quad",
      "severity": 5,
      "status": "AWAITING_ACKNOWLEDGEMENT",
      "isActive": true,
      "currentTier": 1,
      "tierDeadlineTimestamp": "18:24:00",
      "notifications": [
        {
          "incidentID": 1,
          "contactID": 101,
          "contactName": "Sarah Connor",
          "contactPhone": "555-0199",
          "priorityLevel": 1,
          "timestamp": "18:22:00",
          "acknowledged": false,
          "acknowledgementTimestamp": ""
        }
      ]
    }
  ],
  "count": 1
}
```

#### `POST /api/incidents/acknowledge`
Simulates emergency contact acknowledging receipt of distress dispatch.
- **Request Body**:
```json
{
  "incidentID": 1,
  "contactID": 101,
  "timestamp": "18:23:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Incident acknowledged"
}
```

#### `POST /api/incidents/resolve`
Resolves and closes an incident with documented responder notes.
- **Request Body**:
```json
{
  "incidentID": 1,
  "notes": "Student safely escorted to dorm",
  "timestamp": "18:35:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Incident resolved"
}
```

#### `POST /api/incidents/cancel`
Cancels an incident as a false alarm.
- **Request Body**:
```json
{
  "incidentID": 1,
  "reason": "Accidental SOS trigger",
  "timestamp": "18:24:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Incident cancelled"
}
```

---

### 2.6 Simulation & Timeline Endpoints

#### `POST /api/sim/time`
Advances simulated timeline, checking check-in grace periods and verification timeouts.
- **Request Body**:
```json
{
  "currentTime": "18:35:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "message": "Simulated time advanced to 18:35:00"
}
```

#### `POST /api/sim/event`
Injects simulated sensor events (e.g. `Fall Detected`, `Impact`, `Route Deviation`, `Motion Alert`).
- **Request Body**:
```json
{
  "type": "Fall Detected",
  "location": "Science Quad",
  "timestamp": "18:30:00"
}
```
- **Response `200 OK`**:
```json
{
  "success": true,
  "eventID": 9002,
  "newRiskScore": 55,
  "message": "Event ingested"
}
```
