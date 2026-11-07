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
#include "VerificationManager.h"

using namespace std;

class SafetySystem {
private:
    RiskAnalyzer riskAnalyzer;
    ResponseManager responseManager;
    JourneyManager journeyManager;
    CheckInManager checkInManager;
    VerificationManager verificationManager;
    bool activeRiskEscalated; // Tracks whether an automatic incident has already been dispatched for the current high-risk state
    bool activeVerificationEpisode; // Tracks whether safety verification has been initiated for the active high-risk episode
    int autoIncidentCounter;  // Generates unique IDs for automatically escalated incidents
    unordered_map<int, int> verificationToIncidentMap; // Maps verificationRequestID -> incidentID to prevent duplicate incidents

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

    // Safety verification operations (Phase 4)
    bool confirmUserSafe(int requestID = -1);
    bool requestImmediateHelp(int requestID = -1);
    void displayPendingVerifications() const;
    void displayVerificationStatistics() const;
    bool hasActiveVerification() const;
    int getActiveVerificationID() const;
    const VerificationManager& getVerificationManager() const;

    // Incident lifecycle & priority dispatch operations (Phase 4.4 & 4.5)
    bool acknowledgeIncident(int incidentID, int contactID, const string& timestamp = "");
    bool resolveIncident(int incidentID, const string& notes = "", const string& timestamp = "");
    bool cancelIncident(int incidentID, const string& reason = "", const string& timestamp = "");
    void displayActiveIncidents() const;
    void displayIncidentHistory() const;
    void displayNotificationHistory(int incidentID = -1) const;
    void displayIncidentStatistics() const;
    ResponseManager& getResponseManager();
    const ResponseManager& getResponseManager() const;

    // Accessors for testing and inspection
    const RiskAnalyzer& getRiskAnalyzer() const;

    // Dedicated display and inspection methods for menu operations
    void displayRiskAnalysis() const;
    void displayRegisteredContacts() const;
    void displayRecentEvents() const;
    void displayPendingEmergencies() const;

    // System overview
    void displaySystemStatus();
};

#endif // SAFETY_SYSTEM_H
