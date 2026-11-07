#include "Incident.h"
#include <iomanip>

using namespace std;

// Default constructor
Incident::Incident() {
    incidentID = 0;
    description = "Unspecified Incident";
    location = "Unknown";
    severity = 1;
    status = INC_PENDING;
    verificationRequestID = -1;
    riskScore = -1;
    riskLevel = "Normal";
    creationTimestamp = "00:00:00";
    currentTier = 0;
    tierDeadlineSeconds = -1;
    tierDeadlineTimestamp = "";
    acknowledgedByContactID = -1;
    acknowledgedByName = "";
    acknowledgedContactPriority = -1;
    acknowledgementTimestamp = "";
    resolutionNotes = "";
    resolutionTimestamp = "";
    cancellationReason = "";
    cancellationTimestamp = "";
}

// Backward-compatible 4-parameter constructor (Phase 1)
Incident::Incident(int id, string desc, string loc, int sev) {
    incidentID = id;
    description = desc;
    location = loc;
    severity = (sev < 1) ? 1 : ((sev > 5) ? 5 : sev);
    status = INC_PENDING;
    verificationRequestID = -1;
    riskScore = -1;
    riskLevel = "Normal";
    creationTimestamp = "00:00:00";
    currentTier = 0;
    tierDeadlineSeconds = -1;
    tierDeadlineTimestamp = "";
    acknowledgedByContactID = -1;
    acknowledgedByName = "";
    acknowledgedContactPriority = -1;
    acknowledgementTimestamp = "";
    resolutionNotes = "";
    resolutionTimestamp = "";
    cancellationReason = "";
    cancellationTimestamp = "";
}

// Full parameterized constructor (Phase 4.4 / 4.5)
Incident::Incident(int id, string desc, string loc, int sev,
                   int verifID, int rScore, const string& rLevel, const string& timestamp) {
    incidentID = id;
    description = desc;
    location = loc;
    severity = (sev < 1) ? 1 : ((sev > 5) ? 5 : sev);
    status = INC_PENDING;
    verificationRequestID = verifID;
    riskScore = rScore;
    riskLevel = rLevel;
    creationTimestamp = timestamp.empty() ? "00:00:00" : timestamp;
    currentTier = 0;
    tierDeadlineSeconds = -1;
    tierDeadlineTimestamp = "";
    acknowledgedByContactID = -1;
    acknowledgedByName = "";
    acknowledgedContactPriority = -1;
    acknowledgementTimestamp = "";
    resolutionNotes = "";
    resolutionTimestamp = "";
    cancellationReason = "";
    cancellationTimestamp = "";
}

Incident::~Incident() {
    // STL vectors clean up automatically
}

int Incident::getIncidentID() const {
    return incidentID;
}

string Incident::getDescription() const {
    return description;
}

string Incident::getLocation() const {
    return location;
}

int Incident::getSeverity() const {
    return severity;
}

IncidentStatus Incident::getStatus() const {
    return status;
}

string Incident::getStatusString() const {
    switch (status) {
        case INC_PENDING:                  return "PENDING";
        case INC_DISPATCHING:              return "DISPATCHING";
        case INC_AWAITING_ACKNOWLEDGEMENT: return "AWAITING_ACKNOWLEDGEMENT";
        case INC_ACKNOWLEDGED:             return "ACKNOWLEDGED";
        case INC_RESOLVED:                 return "RESOLVED";
        case INC_CANCELLED:                return "CANCELLED";
        case INC_ESCALATION_EXHAUSTED:      return "ESCALATION_EXHAUSTED";
        default:                           return "UNKNOWN";
    }
}

bool Incident::isActive() const {
    return status != INC_RESOLVED && status != INC_CANCELLED;
}

int Incident::getVerificationRequestID() const {
    return verificationRequestID;
}

int Incident::getRiskScore() const {
    return riskScore;
}

string Incident::getRiskLevel() const {
    return riskLevel;
}

string Incident::getCreationTimestamp() const {
    return creationTimestamp;
}

int Incident::getCurrentTier() const {
    return currentTier;
}

void Incident::setCurrentTier(int tier) {
    currentTier = tier;
}

int Incident::getTierDeadlineSeconds() const {
    return tierDeadlineSeconds;
}

void Incident::setTierDeadlineSeconds(int seconds) {
    tierDeadlineSeconds = seconds;
}

string Incident::getTierDeadlineTimestamp() const {
    return tierDeadlineTimestamp;
}

void Incident::setTierDeadlineTimestamp(const string& deadline) {
    tierDeadlineTimestamp = deadline;
}

void Incident::setStatus(IncidentStatus s) {
    status = s;
}

bool Incident::isContactNotified(int contactID) const {
    for (const auto& rec : notificationHistory) {
        if (rec.contactID == contactID) {
            return true;
        }
    }
    return false;
}

bool Incident::recordAcknowledgement(int contactID, const string& contactName, int priority, const string& timestamp) {
    if (status == INC_RESOLVED) {
        cout << "  [Incident Error] Incident #" << incidentID << " is already RESOLVED. Cannot acknowledge." << endl;
        return false;
    }
    if (status == INC_CANCELLED) {
        cout << "  [Incident Error] Incident #" << incidentID << " is CANCELLED. Cannot acknowledge." << endl;
        return false;
    }
    if (status == INC_ACKNOWLEDGED) {
        cout << "  [Incident Error] Incident #" << incidentID << " has already been acknowledged by "
             << acknowledgedByName << " (Contact #" << acknowledgedByContactID << ")." << endl;
        return false;
    }
    if (!isContactNotified(contactID)) {
        cout << "  [Incident Error] Contact #" << contactID << " (" << contactName
             << ") has not been notified for Incident #" << incidentID << ". Acknowledgement rejected." << endl;
        return false;
    }

    // Mark the record in notificationHistory as acknowledged
    for (auto& rec : notificationHistory) {
        if (rec.contactID == contactID) {
            rec.acknowledged = true;
            rec.acknowledgementTimestamp = timestamp;
        }
    }

    status = INC_ACKNOWLEDGED;
    acknowledgedByContactID = contactID;
    acknowledgedByName = contactName;
    acknowledgedContactPriority = priority;
    acknowledgementTimestamp = timestamp;
    return true;
}

int Incident::getAcknowledgedByContactID() const {
    return acknowledgedByContactID;
}

string Incident::getAcknowledgedByName() const {
    return acknowledgedByName;
}

int Incident::getAcknowledgedContactPriority() const {
    return acknowledgedContactPriority;
}

string Incident::getAcknowledgementTimestamp() const {
    return acknowledgementTimestamp;
}

bool Incident::recordResolution(const string& notes, const string& timestamp) {
    if (status == INC_RESOLVED) {
        cout << "  [Incident Error] Incident #" << incidentID << " is already RESOLVED." << endl;
        return false;
    }
    if (status == INC_CANCELLED) {
        cout << "  [Incident Error] Cannot resolve cancelled Incident #" << incidentID << "." << endl;
        return false;
    }

    status = INC_RESOLVED;
    resolutionNotes = notes.empty() ? "Resolved by operator" : notes;
    resolutionTimestamp = timestamp;
    return true;
}

bool Incident::recordCancellation(const string& reason, const string& timestamp) {
    if (status == INC_CANCELLED) {
        cout << "  [Incident Error] Incident #" << incidentID << " is already CANCELLED." << endl;
        return false;
    }
    if (status == INC_RESOLVED) {
        cout << "  [Incident Error] Cannot cancel already resolved Incident #" << incidentID << "." << endl;
        return false;
    }

    status = INC_CANCELLED;
    cancellationReason = reason.empty() ? "False alarm" : reason;
    cancellationTimestamp = timestamp;
    return true;
}

string Incident::getResolutionNotes() const {
    return resolutionNotes;
}

string Incident::getResolutionTimestamp() const {
    return resolutionTimestamp;
}

string Incident::getCancellationReason() const {
    return cancellationReason;
}

string Incident::getCancellationTimestamp() const {
    return cancellationTimestamp;
}

void Incident::addNotificationRecord(const NotificationRecord& record) {
    notificationHistory.push_back(record);
}

const vector<NotificationRecord>& Incident::getNotificationHistory() const {
    return notificationHistory;
}

int Incident::getNotificationCount() const {
    return static_cast<int>(notificationHistory.size());
}

// Overload '<' operator for Priority Queue ordering (higher severity = higher priority)
bool Incident::operator<(const Incident& other) const {
    return this->severity < other.severity;
}

void Incident::displayIncident() const {
    cout << "  [Incident #" << incidentID << "] Severity: Level " << severity << " / 5 | Status: "
         << getStatusString() << endl;
    cout << "    Description: " << description << endl;
    cout << "    Location:    " << location << endl;
}

void Incident::displayDetailedIncident() const {
    cout << "  [Incident #" << incidentID << "] Severity: Level " << severity
         << " / 5 | Status: " << getStatusString() << endl;
    cout << "    Description: " << description << endl;
    cout << "    Location:    " << location << endl;
    cout << "    Created:     " << creationTimestamp;
    if (verificationRequestID > 0) {
        cout << " | Verification Request #" << verificationRequestID
             << " (Risk: " << riskScore << " [" << riskLevel << "])";
    }
    cout << endl;

    if (currentTier > 0) {
        cout << "    Current Tier: Priority " << currentTier;
        if (!tierDeadlineTimestamp.empty()) {
            cout << " | Tier Deadline: " << tierDeadlineTimestamp;
        }
        cout << endl;
    }

    if (status == INC_ACKNOWLEDGED) {
        cout << "    Acknowledged by: " << acknowledgedByName << " (ID: "
             << acknowledgedByContactID << ", Priority " << acknowledgedContactPriority
             << ") at " << acknowledgementTimestamp << endl;
    } else if (status == INC_RESOLVED) {
        cout << "    Resolved at: " << resolutionTimestamp << " | Note: " << resolutionNotes << endl;
    } else if (status == INC_CANCELLED) {
        cout << "    Cancelled at: " << cancellationTimestamp << " | Reason: " << cancellationReason << endl;
    }

    cout << "    Total Notifications Sent: " << notificationHistory.size() << endl;
}

void Incident::displayNotificationHistory() const {
    cout << "    Notification Audit Trail for Incident #" << incidentID << " (" << notificationHistory.size() << "):" << endl;
    if (notificationHistory.empty()) {
        cout << "      (No notifications dispatched yet)" << endl;
        return;
    }
    for (size_t i = 0; i < notificationHistory.size(); i++) {
        const auto& rec = notificationHistory[i];
        cout << "      [" << (i + 1) << "] Sent at " << rec.timestamp
             << " -> " << rec.contactName << " (ID: " << rec.contactID
             << ", Priority " << rec.priorityLevel << ", Phone: " << rec.contactPhone << ")"
             << " | Acknowledged: " << (rec.acknowledged ? ("YES at " + rec.acknowledgementTimestamp) : "NO")
             << endl;
    }
}
