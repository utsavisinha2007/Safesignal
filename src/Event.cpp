#include "../include/Event.h"

using namespace std;

// Default constructor
Event::Event() {
    eventID = 0;
    eventType = "General";
    location = "Unknown";
    timestamp = "00:00:00";
}

// Parameterized constructor
Event::Event(int id, string type, string loc, string time) {
    eventID = id;
    eventType = type;
    location = loc;
    timestamp = time;
}

// Destructor
Event::~Event() {
    // No dynamic memory to release
}

// Getter for event ID
int Event::getEventID() const {
    return eventID;
}

// Getter for event type
string Event::getEventType() const {
    return eventType;
}

// Getter for location
string Event::getLocation() const {
    return location;
}

// Getter for timestamp
string Event::getTimestamp() const {
    return timestamp;
}

// Evaluates and returns a risk severity weight based on the event type.
// Specific dangerous triggers receive high weights, while unknown/minor events receive a low default weight.
int Event::getSeverityWeight() const {
    if (eventType == "SOS" || eventType == "Panic Button") {
        return 40; // Critical direct distress signal
    } else if (eventType == "Fall Detected" || eventType == "Impact") {
        return 30; // Severe physical incident signal
    } else if (eventType == "Route Deviation" || eventType == "Geofence Exit" || eventType == "Missed Check-in") {
        return 20; // Moderate safety anomaly / missed scheduled check-in
    } else if (eventType == "Motion Alert" || eventType == "Noise Spike") {
        return 10; // Low-level environment trigger
    } else {
        return 5;  // Default weight for unrecognized or standard events
    }
}

// Neatly prints the event details to the console
void Event::displayEvent() const {
    cout << "  [Event #" << eventID << "] Type: " << eventType
         << " | Location: " << location
         << " | Time: " << timestamp
         << " | Weight: " << getSeverityWeight() << endl;
}

