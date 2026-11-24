# 🛡️ SafeSignal – Smart Personal Safety & Emergency Response System

**SafeSignal** is a smart personal safety and emergency response web application designed to identify potentially dangerous situations through simulated real-time sensor analysis, risk assessment, and automated emergency escalation.

The system uses Data Structures and Algorithms (DSA) to analyze incoming safety readings, detect abnormal patterns, prioritize incidents, and initiate appropriate emergency response workflows.

🌐 **Live Website:** https://web-puce-six-93.vercel.app/

---

## 📌 Table of Contents

1. Project Overview
2. Problem Statement
3. Key Features
4. Technology Stack
5. System Architecture
6. Data Structures and Algorithms
7. Working of the System
8. Installation and Setup
9. Project Limitations
10. Future Scope

## 🎯 1. Project Overview

SafeSignal aims to improve personal safety, particularly for individuals travelling alone or moving through potentially unsafe environments.

Unlike conventional emergency applications that primarily depend on manual SOS activation, SafeSignal demonstrates a proactive approach by analyzing simulated sensor readings and identifying unusual patterns that may indicate potential danger.

The application provides a centralized Safety Command Centre where users can monitor risk levels, view incidents, and observe emergency escalation workflows.

## ⚠️ 2. Problem Statement

People travelling alone may face emergencies where they cannot immediately access their phones or manually request assistance.

Traditional safety applications often depend on manual SOS activation, which may not always be possible.

SafeSignal addresses this challenge through:
- Continuous analysis of simulated safety readings.
- Detection of abnormal movement and risk patterns.
- Automated risk assessment.
- Priority-based emergency incident handling.
- Structured emergency escalation workflows.

## ✨ 3. Key Features

- **One-Touch SOS:** Provides an interface for initiating an emergency alert.
- **Risk Assessment:** Calculates risk scores based on incoming safety readings.
- **Abnormal Activity Detection:** Identifies unusual patterns using sliding-window analysis.
- **Safety Verification:** Demonstrates a verification stage before escalation.
- **Emergency Command Centre:** Displays incident information and system status.
- **Priority-Based Dispatch:** Organizes incidents according to their urgency.
- **Time-Based Scheduling:** Supports scheduled verification and escalation events.
- **Incident Monitoring:** Displays risk levels and emergency workflow information.

## 💻 4. Technology Stack

| Technology | Purpose |
|---|---|
| React.js | Frontend user interface |
| Vite | Frontend development and build tool |
| JavaScript | Frontend functionality |
| C++ | Core algorithm and backend processing |
| REST API | Communication between frontend and backend |
| HTML5 & CSS3 | Structure and styling |
| Docker | Backend containerization |
| Render | Backend deployment |
| Vercel | Frontend deployment |
| Git & GitHub | Version control and collaboration |

## 🏗️ 5. System Architecture

SafeSignal follows a frontend-backend architecture.

### Frontend
The React-based interface provides:
- Safety dashboard
- Risk monitoring
- SOS functionality
- Incident status
- Emergency command centre

### Backend
The C++ backend handles:
- Sensor reading analysis
- Sliding-window processing
- Risk evaluation
- Incident verification
- Priority-based emergency dispatch

### API Communication
The REST API connects the frontend with the C++ backend and transfers information between the two components.

## 🧠 6. Data Structures and Algorithms Used

DSA plays an important role in the processing and management of safety events.

| DSA Concept | Application in SafeSignal |
|---|---|
| Sliding Window | Analyzes recent sensor readings over a fixed time interval. |
| Queue | Maintains incoming safety events in processing order where required. |
| Priority Queue | Prioritizes emergency incidents according to their urgency. |
| Arrays / Vectors | Store and process sensor readings and event data. |
| Sorting / Priority Ordering | Helps organize incidents based on risk or priority. |
| Time-Based Scheduling | Manages verification and escalation events at scheduled intervals. |

### Sliding Window Algorithm
The sliding-window technique examines a fixed group of recent sensor readings instead of processing the entire historical dataset repeatedly.

This helps identify unusual patterns and changes in movement or activity.

### Priority Queue
A priority queue helps process high-risk incidents before lower-priority incidents, ensuring that more urgent situations receive attention first.

### Time-Based Scheduling
Time-based scheduling manages actions that need to occur after a specified interval, such as safety verification and emergency escalation.

## 🔄 7. Working of the System

1. **Data Input:** Simulated sensor readings and safety events enter the system.
2. **Data Processing:** The C++ backend processes the incoming readings.
3. **Pattern Detection:** Sliding-window analysis examines recent readings for abnormal patterns.
4. **Risk Assessment:** The system evaluates the detected patterns and calculates a risk score.
5. **Safety Verification:** If the risk exceeds a defined threshold, a verification workflow is initiated.
6. **Incident Prioritization:** The priority queue organizes incidents according to urgency.
7. **Emergency Escalation:** The system demonstrates the appropriate emergency response workflow.
8. **Dashboard Update:** The frontend displays updated risk levels and incident statuses.

## ⚙️ 8. Installation and Setup

### Prerequisites

- Node.js and npm
- C++ compiler (supporting C++17 or later)
- CMake
- Git

### Step 1: Clone the Repository

```bash
git clone YOUR_GITHUB_REPOSITORY_URL
cd SafeSignal
```

### Step 2: Install Frontend Dependencies

```bash
npm install
```

### Step 3: Run the Frontend

```bash
npm run dev
```

### Step 4: Build and Run the C++ Backend

Use the project's CMake configuration:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Run the generated backend executable according to your operating system and project configuration.

**Note:** Ensure the backend API is running and the frontend API configuration points to the correct backend URL.

## 🚧 9. Project Limitations

- Sensor readings and GPS information are simulated.
- The project does not currently integrate physical wearable hardware.
- Emergency calls are not connected to actual police, ambulance, or emergency services.
- Location tracking and notification broadcasts are demonstrated through a software-based simulation.
- Real-world deployment would require hardware integration, secure communication, and reliable emergency service connectivity.

## 🚀 10. Future Scope

- Integration with smartwatches and wearable safety devices.
- Real-time GPS tracking.
- Integration with accelerometers and gyroscope sensors.
- Machine learning-based abnormal activity detection.
- Live emergency contact notifications.
- Integration with authorized emergency response services.
- Mobile application development.
- Cloud-based incident storage and analytics.

## 🎓 Project Purpose

SafeSignal demonstrates how Data Structures and Algorithms can be applied to a real-world safety problem through software engineering, real-time data processing concepts, risk assessment, and emergency response management.

**Project Category:** Application

**Thrust Area:** Smart Systems

**Core Focus:** Data Structures and Algorithms (DSA)

---

*SafeSignal is currently an academic software simulation and prototype. It is not a certified emergency response or life-safety system.*
