#ifndef INCIDENT_H
#define INCIDENT_H

#include <string>

class Incident {
private:
    int incidentID;
    std::string description;
    std::string location;
    int severity;

public:
    Incident();
    Incident(int id, std::string desc, std::string loc, int sev);
    ~Incident();

    int getIncidentID() const;
    std::string getDescription() const;
    std::string getLocation() const;
    int getSeverity() const;
};

#endif