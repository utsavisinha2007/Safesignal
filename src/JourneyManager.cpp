#include "../include/JourneyManager.h"

using namespace std;

// Constructor: initializes members and starts eventCounter at 7000
JourneyManager::JourneyManager() {
    startLocation = "N/A";
    destination = "N/A";
    expectedArrivalTime = "N/A";
    currentCheckpointIndex = 0;
    isJourneyActive = false;
    eventCounter = 7000;
}

// Destructor
JourneyManager::~JourneyManager() {
    // Standard library containers clean up automatically
}

// Starts a new journey after validating input parameters
bool JourneyManager::startJourney(const string& startLoc, const string& dest,
                                  const string& eta, const vector<string>& route) {
    if (isJourneyActive) {
        cout << "  [JourneyManager Error] A journey is already active. Please end it before starting a new one." << endl;
        return false;
    }

    if (startLoc.empty() || dest.empty()) {
        cout << "  [JourneyManager Error] Start location and destination cannot be empty." << endl;
        return false;
    }

    if (route.empty()) {
        cout << "  [JourneyManager Error] Planned route must contain at least one checkpoint." << endl;
        return false;
    }

    startLocation = startLoc;
    destination = dest;
    expectedArrivalTime = eta;
    plannedRoute = route;
    currentCheckpointIndex = 0;
    isJourneyActive = true;

    cout << "  [JourneyManager] Journey started from " << startLocation
         << " to " << destination << " (ETA: " << expectedArrivalTime << ")." << endl;
    cout << "  [JourneyManager] Planned checkpoints: " << plannedRoute.size() << endl;
    return true;
}

// Evaluates a simulated location update against the next expected checkpoint.
// If the location deviates from the expected checkpoint, generates an Event and returns true.
bool JourneyManager::updateLocation(const string& currentLocation, const string& timestamp, Event& outDeviationEvent) {
    if (!isJourneyActive) {
        cout << "  [JourneyManager Warning] No active journey to update." << endl;
        return false;
    }

    if (currentLocation.empty()) {
        cout << "  [JourneyManager Error] Location update cannot be empty." << endl;
        return false;
    }

    // Check if the current checkpoint index is valid
    if (currentCheckpointIndex >= plannedRoute.size()) {
        cout << "  [JourneyManager] All checkpoints have already been completed." << endl;
        return false;
    }

    string expected = plannedRoute[currentCheckpointIndex];

    // Case 1: Location matches expected checkpoint
    if (currentLocation == expected) {
        cout << "  [JourneyManager] On Track: Reached Checkpoint " << (currentCheckpointIndex + 1)
             << " (" << currentLocation << ") at " << timestamp << "." << endl;
        currentCheckpointIndex++;

        // If this was the final checkpoint, the journey has successfully reached its destination
        if (currentCheckpointIndex >= plannedRoute.size()) {
            cout << "  [JourneyManager] Destination reached successfully! Completing journey." << endl;
            isJourneyActive = false;
        }
        return false;
    }

    // Case 2: Location does not match expected checkpoint -> Route deviation detected
    eventCounter++;
    outDeviationEvent = Event(eventCounter, "Route Deviation", currentLocation, timestamp);

    cout << "  [JourneyManager ALERT] Route Deviation Detected!" << endl;
    cout << "    Reported Location:   " << currentLocation << endl;
    cout << "    Expected Checkpoint: " << expected << endl;
    cout << "    Generated Event #" << outDeviationEvent.getEventID()
         << " (" << outDeviationEvent.getEventType() << ")" << endl;

    return true;
}

// Convenience method returning the generated Event directly (eventID > 0 if deviation)
Event JourneyManager::processLocationUpdate(const string& currentLocation, const string& timestamp) {
    Event devEvent;
    bool deviated = updateLocation(currentLocation, timestamp, devEvent);
    if (deviated) {
        return devEvent;
    }
    // Return default event (ID 0) indicating normal on-route update
    return Event(0, "On-Route Update", currentLocation, timestamp);
}

// Checks if a journey is currently active
bool JourneyManager::isActive() const {
    return isJourneyActive;
}

// Ends the current journey manually
void JourneyManager::endJourney() {
    if (!isJourneyActive) {
        cout << "  [JourneyManager] No active journey to end." << endl;
        return;
    }

    isJourneyActive = false;
    cout << "  [JourneyManager] Journey ended manually. Route tracking stopped." << endl;
}

// Displays journey details, route progress, and next expected checkpoint
void JourneyManager::displayJourneyDetails() const {
    cout << "\n--- [Journey Details] ---" << endl;
    cout << "  Status:         " << (isJourneyActive ? "ACTIVE" : "INACTIVE") << endl;
    cout << "  Origin:         " << startLocation << endl;
    cout << "  Destination:    " << destination << endl;
    cout << "  Expected ETA:   " << expectedArrivalTime << endl;

    if (plannedRoute.empty()) {
        cout << "  Planned Route:  (No checkpoints planned)" << endl;
    } else {
        cout << "  Route Progress: " << currentCheckpointIndex << " / " << plannedRoute.size() << " checkpoints reached." << endl;
        for (size_t i = 0; i < plannedRoute.size(); i++) {
            cout << "    [" << (i + 1) << "] " << plannedRoute[i];
            if (i < currentCheckpointIndex) {
                cout << " (Passed)";
            } else if (i == currentCheckpointIndex && isJourneyActive) {
                cout << " <-- [NEXT EXPECTED]";
            }
            cout << endl;
        }
    }
    cout << "-------------------------\n" << endl;
}

// Getters
string JourneyManager::getStartLocation() const {
    return startLocation;
}

string JourneyManager::getDestination() const {
    return destination;
}

string JourneyManager::getExpectedArrivalTime() const {
    return expectedArrivalTime;
}

vector<string> JourneyManager::getPlannedRoute() const {
    return plannedRoute;
}

size_t JourneyManager::getCurrentCheckpointIndex() const {
    return currentCheckpointIndex;
}

string JourneyManager::getNextExpectedCheckpoint() const {
    if (isJourneyActive && currentCheckpointIndex < plannedRoute.size()) {
        return plannedRoute[currentCheckpointIndex];
    }
    return "None";
}


