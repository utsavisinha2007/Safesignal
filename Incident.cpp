#include "Incident.h"

using namespace std;

// Default constructor
Incident::Incident() {
    incidentID = 0;
    description = "Unspecified Incident";
    location = "Unknown";
    severity = 1; // Default to lowest severity
}

// Parameterized constructor with validation ensuring severity stays within [1, 5]
Incident::Incident(int id, string desc, string loc, int sev) {
    incidentID = id;
    description = desc;
    location = loc;

    // Validate severity range (1 = Minor, 5 = Critical)
    if (sev < 1) {
        severity = 1;
    } else if (sev > 5) {
        severity = 5;
    } else {
        severity = sev;
    }
}

// Destructor
Incident::~Incident() {
    // No dynamic memory to release
}

// Getter for incident ID
int Incident::getIncidentID() const {
    return incidentID;
}

// Getter for description
string Incident::getDescription() const {
    return description;
}

// Getter for location
string Incident::getLocation() const {
    return location;
}

// Getter for severity
int Incident::getSeverity() const {
    return severity;
}

// Overload '<' operator for Priority Queue ordering.
// In std::priority_queue (Max-Heap), returning true when this->severity < other.severity
// ensures that the Incident with the HIGHEST severity gets highest priority.
bool Incident::operator<(const Incident& other) const {
    return this->severity < other.severity;
}

// Neatly prints incident details to the console
void Incident::displayIncident() const {
    cout << "  [Incident #" << incidentID << "] Severity: Level " << severity << " / 5" << endl;
    cout << "    Description: " << description << endl;
    cout << "    Location:    " << location << endl;
}

