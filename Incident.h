#ifndef INCIDENT_H
#define INCIDENT_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Lifecycle states for an incident (Phase 4.5)
enum IncidentStatus {
    INC_PENDING,                  // Incident created, pending dispatch
    INC_DISPATCHING,              // Broadcast in progress
    INC_AWAITING_ACKNOWLEDGEMENT, // Priority tier notified, awaiting contact acknowledgement
    INC_ACKNOWLEDGED,             // A notified contact has confirmed/acknowledged the incident
    INC_RESOLVED,                 // Incident has been resolved and closed
    INC_CANCELLED,                // Incident was cancelled (e.g. false alarm)
    INC_ESCALATION_EXHAUSTED      // All contact tiers notified without acknowledgement
};

// Notification record tracking simulated dispatch audit trail
struct NotificationRecord {
    int incidentID;
    int contactID;
    string contactName;
    string contactPhone;
    int priorityLevel;
    string timestamp;
    bool acknowledged;
    string acknowledgementTimestamp;
};

class Incident {
private:
    int incidentID;
    string description;
    string location;
    int severity; // Severity scale: 1 (Low) to 5 (Critical)

    // Phase 4.4 & 4.5 enhancements
    IncidentStatus status;
    int verificationRequestID; // -1 if manual incident
    int riskScore;             // -1 if manual incident
    string riskLevel;          // Risk level at creation
    string creationTimestamp;  // Timestamp when incident was created

    // Priority escalation tracking
    int currentTier;               // 0: unnotified, 1: Tier 1, 2: Tier 2, 3: Tier 3
    int tierDeadlineSeconds;       // Simulated deadline in seconds for current tier timeout
    string tierDeadlineTimestamp;  // Formatted deadline timestamp

    // Acknowledgement tracking
    int acknowledgedByContactID;
    string acknowledgedByName;
    int acknowledgedContactPriority;
    string acknowledgementTimestamp;

    // Resolution & cancellation tracking
    string resolutionNotes;
    string resolutionTimestamp;
    string cancellationReason;
    string cancellationTimestamp;

    // Notification audit trail for this incident
    vector<NotificationRecord> notificationHistory;

public:
    // Default constructor
    Incident();

    // Backward-compatible 4-parameter constructor (Phase 1)
    Incident(int id, string desc, string loc, int sev);

    // Full parameterized constructor (Phase 4.4 / 4.5)
    Incident(int id, string desc, string loc, int sev,
             int verifID, int rScore, const string& rLevel, const string& timestamp);

    ~Incident();

    // Standard getters
    int getIncidentID() const;
    string getDescription() const;
    string getLocation() const;
    int getSeverity() const;

    // Status and lifecycle getters
    IncidentStatus getStatus() const;
    string getStatusString() const;
    bool isActive() const; // True if status is not RESOLVED and not CANCELLED
    int getVerificationRequestID() const;
    int getRiskScore() const;
    string getRiskLevel() const;
    string getCreationTimestamp() const;

    // Priority tier tracking getters and setters
    int getCurrentTier() const;
    void setCurrentTier(int tier);
    int getTierDeadlineSeconds() const;
    void setTierDeadlineSeconds(int seconds);
    string getTierDeadlineTimestamp() const;
    void setTierDeadlineTimestamp(const string& deadline);
    void setStatus(IncidentStatus s);

    // Acknowledgement operations
    bool isContactNotified(int contactID) const;
    bool recordAcknowledgement(int contactID, const string& contactName, int priority, const string& timestamp);
    int getAcknowledgedByContactID() const;
    string getAcknowledgedByName() const;
    int getAcknowledgedContactPriority() const;
    string getAcknowledgementTimestamp() const;

    // Resolution and cancellation operations
    bool recordResolution(const string& notes, const string& timestamp);
    bool recordCancellation(const string& reason, const string& timestamp);
    string getResolutionNotes() const;
    string getResolutionTimestamp() const;
    string getCancellationReason() const;
    string getCancellationTimestamp() const;

    // Notification history operations
    void addNotificationRecord(const NotificationRecord& record);
    const vector<NotificationRecord>& getNotificationHistory() const;
    int getNotificationCount() const;

    // Overload '<' operator for Priority Queue ordering (higher severity = higher priority)
    bool operator<(const Incident& other) const;

    // Display functions
    void displayIncident() const;
    void displayDetailedIncident() const;
    void displayNotificationHistory() const;
};

#endif // INCIDENT_H
