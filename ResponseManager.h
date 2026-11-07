#ifndef RESPONSE_MANAGER_H
#define RESPONSE_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include "Contact.h"
#include "Incident.h"

using namespace std;

// Manages verification requests, priority-based emergency dispatch,
// and complete incident lifecycle tracking (Phases 1, 4.1, 4.4, 4.5).
class ResponseManager {
private:
    // Dynamic array (Vector) storing emergency contacts
    vector<Contact> contacts;

    // Priority Queue (Max-Heap) managing pending incidents by severity (Phase 1)
    priority_queue<Incident> incidentQueue;

    // Master incident registry for O(1) lookup, tracking active and historical incidents (Phase 4.5)
    unordered_map<int, Incident> incidentRegistry;

    // Ordered list of incident IDs to preserve insertion/creation order
    vector<int> incidentHistoryIDs;

    // Configurable acknowledgement timeout in minutes (default: 2 minutes)
    int acknowledgementTimeoutMinutes;

    // Simulated timeline tracking
    int currentTimeSeconds;
    string currentTimeStr;

    // Helper: Strict time-of-day parser (HH:MM or HH:MM:SS)
    bool parseTimeOfDay(const string& timeStr, int& outSeconds) const;

    // Helper: Formats seconds-of-day to HH:MM:SS
    string formatTime(int totalSeconds) const;

    // Helper: Dispatches notifications to contacts of a specific priority tier for an incident
    bool dispatchTier(int incidentID, int tier, const string& timestamp);

public:
    ResponseManager(int ackTimeoutMins = 2);
    ~ResponseManager();

    // Configuration
    void setAcknowledgementTimeoutMinutes(int minutes);
    int getAcknowledgementTimeoutMinutes() const;

    // Emergency contact registration & display (Phase 1 / 4.1)
    void registerContact(const Contact& contact);
    void displayContacts() const;
    int getContactCount() const;
    vector<Contact> getContactsByPriority(int priority) const;

    // Incident creation & dispatch (Phase 1 / 4.4)
    void addIncident(const Incident& incident);
    bool dispatchEmergency(const Incident& incident, const string& timestamp = "");
    void processNextIncident(); // Triages highest-priority from incidentQueue
    bool hasPendingIncidents() const;
    int getPendingIncidentCount() const;
    void displayPendingIncidents() const;

    // Simulated emergency alerts to registered contacts (Phase 1 compatibility)
    void dispatchAlerts(const Incident& incident);

    // Priority escalation & simulated time advancement (Phase 4.4)
    vector<int> updateCurrentTime(const string& currentTime);

    // Incident lifecycle management operations (Phase 4.5)
    bool acknowledgeIncident(int incidentID, int contactID, const string& timestamp);
    bool resolveIncident(int incidentID, const string& resolutionNotes, const string& timestamp);
    bool cancelIncident(int incidentID, const string& cancellationReason, const string& timestamp);

    // Queries & status inspections
    bool getIncidentByID(int incidentID, Incident& outIncident) const;
    bool hasActiveIncidents() const;
    vector<Incident> getActiveIncidents() const;
    vector<Incident> getAllIncidents() const;

    // Display operations
    void displayActiveIncidents() const;
    void displayIncidentHistory() const;
    void displayNotificationHistory(int incidentID = -1) const;
    void displayIncidentStatistics() const;

    // Statistics getters
    int getTotalIncidentCount() const;
    int getActiveIncidentCount() const;
    int getAcknowledgedIncidentCount() const;
    int getResolvedIncidentCount() const;
    int getCancelledIncidentCount() const;
    int getEscalationExhaustedCount() const;
    int getTotalNotificationCount() const;
};

#endif // RESPONSE_MANAGER_H
