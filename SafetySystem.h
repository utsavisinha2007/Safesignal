#ifndef SAFETY_SYSTEM_H
#define SAFETY_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include "Contact.h"
#include "Event.h"
#include "Incident.h"
#include "RiskAnalyzer.h"
#include "ResponseManager.h"
#include "JourneyManager.h"
#include "CheckInManager.h"

using namespace std;

class SafetySystem {
private:
    RiskAnalyzer riskAnalyzer;
    ResponseManager responseManager;
    JourneyManager journeyManager;
    CheckInManager checkInManager;
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

    // Journey management operations
    bool startJourney(const string& startLoc, const string& dest,
                      const string& eta, const vector<string>& route);
    void updateJourneyLocation(const string& currentLocation, const string& timestamp);
    void displayJourneyDetails() const;
    void endJourney();

    // Check-in management operations
    int scheduleCheckIn(const string& scheduledTime, const string& description, const string& location);
    bool completeCheckIn(int checkInID);
    void processCurrentTimeUpdate(const string& currentTime);
    void displayScheduledCheckIns() const;
    void displayCheckInStatistics() const;

    // Dedicated display and inspection methods for menu operations
    void displayRiskAnalysis() const;
    void displayRegisteredContacts() const;
    void displayRecentEvents() const;
    void displayPendingEmergencies() const;

    // System overview
    void displaySystemStatus();
};

#endif // SAFETY_SYSTEM_H
