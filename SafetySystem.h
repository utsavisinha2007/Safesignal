#ifndef SAFETY_SYSTEM_H
#define SAFETY_SYSTEM_H

#include <iostream>
#include <string>
#include "Contact.h"
#include "Event.h"
#include "Incident.h"
#include "RiskAnalyzer.h"
#include "ResponseManager.h"

using namespace std;

class SafetySystem {
private:
    RiskAnalyzer riskAnalyzer;
    ResponseManager responseManager;
    bool activeRiskEscalated; // Tracks whether an automatic incident has already been dispatched for the current high-risk state
    int autoIncidentCounter;  // Generates unique IDs for automatically escalated incidents

public:
    SafetySystem();
    ~SafetySystem();

    // Event & risk operations
    void logEvent(const Event& event);

    // Incident reporting & response operations
    void reportIncident(const Incident& incident);
    void processNextEmergency();

    // Contact management operations
    void registerContact(const Contact& contact);

    // System overview
    void displaySystemStatus();
};

#endif // SAFETY_SYSTEM_H
