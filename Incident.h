#ifndef INCIDENT_H
#define INCIDENT_H

#include <iostream>
#include <string>

using namespace std;

class Incident {
private:
    int incidentID;
    string description;
    string location;
    int severity; // Severity scale: 1 (Low) to 5 (Critical)

public:
    Incident();
    Incident(int id, string desc, string loc, int sev);
    ~Incident();

    // Getter functions
    int getIncidentID() const;
    string getDescription() const;
    string getLocation() const;
    int getSeverity() const;

    // Overload '<' operator for Priority Queue ordering (higher severity = higher priority)
    bool operator<(const Incident& other) const;

    // Display function
    void displayIncident() const;
};

#endif // INCIDENT_H
