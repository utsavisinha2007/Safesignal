#ifndef JOURNEY_MANAGER_H
#define JOURNEY_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include "Event.h"

using namespace std;

// Manages planned journey routes, tracks progress along checkpoints,
// and detects route deviations when unexpected locations are reported.
class JourneyManager {
private:
    string startLocation;
    string destination;
    string expectedArrivalTime;
    vector<string> plannedRoute;     // Ordered list of route checkpoints
    size_t currentCheckpointIndex;   // Index of the next expected checkpoint
    bool isJourneyActive;            // Status flag indicating if journey is currently underway
    int eventCounter;                // Internal counter for generating unique deviation event IDs (starts at 7000)

public:
    JourneyManager();
    ~JourneyManager();

    // Starts a new journey with planned checkpoints
    bool startJourney(const string& startLoc, const string& dest,
                      const string& eta, const vector<string>& route);

    // Accepts a simulated location update and checks for route deviation.
    // Returns true if a deviation is detected and populates outDeviationEvent.
    bool updateLocation(const string& currentLocation, const string& timestamp, Event& outDeviationEvent);

    // Convenience method returning the generated Event directly (eventID > 0 if deviation)
    Event processLocationUpdate(const string& currentLocation, const string& timestamp);

    // Journey status and management
    bool isActive() const;
    void endJourney();
    void displayJourneyDetails() const;

    // Getters for inspection
    string getStartLocation() const;
    string getDestination() const;
    string getExpectedArrivalTime() const;
    vector<string> getPlannedRoute() const;
    size_t getCurrentCheckpointIndex() const;
    string getNextExpectedCheckpoint() const;
};

#endif // JOURNEY_MANAGER_H

