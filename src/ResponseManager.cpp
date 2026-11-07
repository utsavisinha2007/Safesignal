#include "../include/ResponseManager.h"
#include <cctype>
#include <iomanip>

using namespace std;

// ---------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------

ResponseManager::ResponseManager(int ackTimeoutMins) {
    acknowledgementTimeoutMinutes = (ackTimeoutMins > 0) ? ackTimeoutMins : 2;
    currentTimeSeconds = -1;
    currentTimeStr = "";
}

ResponseManager::~ResponseManager() {
    // Standard library containers clean up automatically
}

// ---------------------------------------------------------------
// Private Helpers
// ---------------------------------------------------------------

bool ResponseManager::parseTimeOfDay(const string& timeStr, int& outSeconds) const {
    outSeconds = -1;
    if (timeStr.empty()) return false;

    int colonCount = 0;
    for (char c : timeStr) {
        if (c == ':') colonCount++;
        else if (!isdigit(static_cast<unsigned char>(c))) return false;
    }

    if (colonCount != 1 && colonCount != 2) return false;

    size_t firstColon = timeStr.find(':');
    string hourStr = timeStr.substr(0, firstColon);
    if (hourStr.empty() || hourStr.length() > 2) return false;

    string minStr = "";
    string secStr = "0";

    if (colonCount == 1) {
        minStr = timeStr.substr(firstColon + 1);
        if (minStr.empty() || minStr.length() > 2) return false;
    } else {
        size_t secondColon = timeStr.find(':', firstColon + 1);
        minStr = timeStr.substr(firstColon + 1, secondColon - firstColon - 1);
        secStr = timeStr.substr(secondColon + 1);
        if (minStr.empty() || minStr.length() > 2 || secStr.empty() || secStr.length() > 2) return false;
    }

    int hours = stoi(hourStr);
    int minutes = stoi(minStr);
    int seconds = stoi(secStr);

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {
        return false;
    }

    outSeconds = hours * 3600 + minutes * 60 + seconds;
    return true;
}

string ResponseManager::formatTime(int totalSeconds) const {
    int h = totalSeconds / 3600;
    int m = (totalSeconds % 3600) / 60;
    int s = totalSeconds % 60;

    string result = "";
    if (h < 10) result += "0";
    result += to_string(h) + ":";
    if (m < 10) result += "0";
    result += to_string(m) + ":";
    if (s < 10) result += "0";
    result += to_string(s);
    return result;
}

// Dispatches notifications to all contacts of a specific priority tier for an incident
bool ResponseManager::dispatchTier(int incidentID, int tier, const string& timestamp) {
    auto it = incidentRegistry.find(incidentID);
    if (it == incidentRegistry.end()) return false;

    Incident& incident = it->second;
    vector<Contact> tierContacts = getContactsByPriority(tier);

    if (tierContacts.empty()) {
        return false; // No contacts in this tier -> caller should advance to next tier
    }

    string actualTime = timestamp.empty() ? (currentTimeStr.empty() ? incident.getCreationTimestamp() : currentTimeStr) : timestamp;
    int timeSec = 0;
    if (!parseTimeOfDay(actualTime, timeSec)) {
        timeSec = (currentTimeSeconds != -1) ? currentTimeSeconds : 0;
        actualTime = formatTime(timeSec % 86400);
    } else {
        if (currentTimeSeconds != -1 && timeSec < (currentTimeSeconds % 86400)) {
            if ((currentTimeSeconds % 86400) >= 64800 && timeSec <= 21600 &&
                ((timeSec + 86400) - (currentTimeSeconds % 86400) <= 21600)) {
                timeSec = (currentTimeSeconds / 86400 + 1) * 86400 + timeSec;
            } else {
                timeSec = (currentTimeSeconds / 86400) * 86400 + timeSec;
            }
        } else if (currentTimeSeconds != -1) {
            timeSec = (currentTimeSeconds / 86400) * 86400 + timeSec;
        }
    }

    cout << "\n  --- [PRIORITY-BASED EMERGENCY DISPATCH: TIER " << tier << "] ---" << endl;
    cout << "  Broadcasting alert for Incident #" << incident.getIncidentID()
         << " [Severity " << incident.getSeverity() << "/5] at " << incident.getLocation()
         << " (Time: " << actualTime << "):" << endl;

    for (const auto& contact : tierContacts) {
        cout << "  -> Simulated Notification Sent to " << contact.getName()
             << " (" << contact.getRelationship() << ") [Priority " << tier
             << ": " << contact.getPriorityDescription() << "] at "
             << contact.getPhoneNumber() << endl;

        NotificationRecord rec;
        rec.incidentID = incident.getIncidentID();
        rec.contactID = contact.getContactID();
        rec.contactName = contact.getName();
        rec.contactPhone = contact.getPhoneNumber();
        rec.priorityLevel = tier;
        rec.timestamp = actualTime;
        rec.acknowledged = false;
        rec.acknowledgementTimestamp = "";

        incident.addNotificationRecord(rec);
    }

    int deadlineSec = timeSec + (acknowledgementTimeoutMinutes * 60);
    string deadlineStr = formatTime(deadlineSec % 86400);

    incident.setCurrentTier(tier);
    incident.setTierDeadlineSeconds(deadlineSec);
    incident.setTierDeadlineTimestamp(deadlineStr);
    incident.setStatus(INC_AWAITING_ACKNOWLEDGEMENT);

    cout << "  Incident #" << incident.getIncidentID()
         << " awaiting Priority " << tier << " acknowledgement." << endl;
    cout << "  Tier Deadline: " << deadlineStr
         << " (Timeout: " << acknowledgementTimeoutMinutes << " min)." << endl;
    cout << "  ---------------------------------------------------------\n" << endl;

    return true;
}

// ---------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------

void ResponseManager::setAcknowledgementTimeoutMinutes(int minutes) {
    if (minutes > 0) {
        acknowledgementTimeoutMinutes = minutes;
        cout << "  [ResponseManager] Acknowledgement timeout configured to "
             << acknowledgementTimeoutMinutes << " minutes." << endl;
    } else {
        cout << "  [ResponseManager Error] Timeout must be positive." << endl;
    }
}

int ResponseManager::getAcknowledgementTimeoutMinutes() const {
    return acknowledgementTimeoutMinutes;
}

// ---------------------------------------------------------------
// Contact Operations
// ---------------------------------------------------------------

void ResponseManager::registerContact(const Contact& contact) {
    contacts.push_back(contact);
    cout << "  [ResponseManager] Registered emergency contact: "
         << contact.getName() << " (" << contact.getRelationship() << ") [Priority "
         << contact.getPriorityLevel() << ": " << contact.getPriorityDescription() << "]" << endl;
}

void ResponseManager::displayContacts() const {
    if (contacts.empty()) {
        cout << "  (No emergency contacts currently registered)" << endl;
        return;
    }

    cout << "  Registered Emergency Contacts (" << contacts.size() << "):" << endl;
    for (size_t i = 0; i < contacts.size(); i++) {
        contacts[i].displayContact();
    }
}

int ResponseManager::getContactCount() const {
    return static_cast<int>(contacts.size());
}

vector<Contact> ResponseManager::getContactsByPriority(int priority) const {
    vector<Contact> filtered;
    for (const auto& c : contacts) {
        if (c.getPriorityLevel() == priority) {
            filtered.push_back(c);
        }
    }
    return filtered;
}

// ---------------------------------------------------------------
// Incident Operations & Priority Dispatch
// ---------------------------------------------------------------

void ResponseManager::addIncident(const Incident& incident) {
    int id = incident.getIncidentID();
    incidentRegistry[id] = incident;

    bool found = false;
    for (int hId : incidentHistoryIDs) {
        if (hId == id) {
            found = true;
            break;
        }
    }
    if (!found) {
        incidentHistoryIDs.push_back(id);
    }

    incidentQueue.push(incident);

    cout << "  [ResponseManager] Incident #" << incident.getIncidentID()
         << " (Severity " << incident.getSeverity() << "/5) queued for priority triage." << endl;
}

bool ResponseManager::dispatchEmergency(const Incident& incident, const string& timestamp) {
    int id = incident.getIncidentID();

    // Check authoritative registry status
    auto it = incidentRegistry.find(id);
    if (it != incidentRegistry.end()) {
        Incident& existing = it->second;
        if (existing.getStatus() == INC_RESOLVED) {
            cout << "  [ResponseManager Error] Incident #" << id
                 << " is already RESOLVED. Cannot dispatch again." << endl;
            return false;
        }
        if (existing.getStatus() == INC_CANCELLED) {
            cout << "  [ResponseManager Error] Incident #" << id
                 << " is already CANCELLED. Cannot dispatch again." << endl;
            return false;
        }
        if (existing.getStatus() == INC_ACKNOWLEDGED) {
            cout << "  [ResponseManager Error] Incident #" << id
                 << " is already ACKNOWLEDGED. Cannot dispatch again." << endl;
            return false;
        }
        if (existing.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT || existing.getStatus() == INC_DISPATCHING) {
            cout << "  [ResponseManager Warning] Incident #" << id
                 << " is already in active emergency dispatch." << endl;
            return false;
        }
    } else {
        incidentRegistry[id] = incident;
        bool found = false;
        for (int hId : incidentHistoryIDs) {
            if (hId == id) {
                found = true;
                break;
            }
        }
        if (!found) {
            incidentHistoryIDs.push_back(id);
        }
    }

    string timeStr = timestamp.empty() ? incident.getCreationTimestamp() : timestamp;
    int parsedSec = 0;
    if (parseTimeOfDay(timeStr, parsedSec)) {
        int effectiveSec = parsedSec;
        if (currentTimeSeconds != -1 && parsedSec < (currentTimeSeconds % 86400)) {
            if ((currentTimeSeconds % 86400) >= 64800 && parsedSec <= 21600 &&
                ((parsedSec + 86400) - (currentTimeSeconds % 86400) <= 21600)) {
                effectiveSec = (currentTimeSeconds / 86400 + 1) * 86400 + parsedSec;
            } else {
                effectiveSec = (currentTimeSeconds / 86400) * 86400 + parsedSec;
            }
        } else if (currentTimeSeconds != -1) {
            effectiveSec = (currentTimeSeconds / 86400) * 86400 + parsedSec;
        }
        if (currentTimeSeconds == -1 || effectiveSec >= currentTimeSeconds) {
            currentTimeSeconds = effectiveSec;
            currentTimeStr = timeStr;
        }
    }

    incidentRegistry[id].setStatus(INC_DISPATCHING);

    // Start with Tier 1
    if (dispatchTier(id, 1, timeStr)) {
        return true;
    }

    // Tier 1 was empty -> immediately escalate to Tier 2
    cout << "  [ResponseManager Notice] No Priority 1 contacts registered. Escalating immediately to Tier 2..." << endl;
    if (dispatchTier(id, 2, timeStr)) {
        return true;
    }

    // Tier 2 was empty -> immediately escalate to Tier 3
    cout << "  [ResponseManager Notice] No Priority 2 contacts registered. Escalating immediately to Tier 3..." << endl;
    if (dispatchTier(id, 3, timeStr)) {
        return true;
    }

    // No contacts registered in any tier
    cout << "  [ResponseManager WARNING] No emergency contacts registered at any tier! Incident #"
         << id << " marked as ESCALATION_EXHAUSTED." << endl;
    incidentRegistry[id].setStatus(INC_ESCALATION_EXHAUSTED);
    return false;
}

void ResponseManager::processNextIncident() {
    while (!incidentQueue.empty()) {
        Incident topIncident = incidentQueue.top();
        incidentQueue.pop();

        int id = topIncident.getIncidentID();
        auto it = incidentRegistry.find(id);
        if (it == incidentRegistry.end()) {
            continue;
        }

        // Check authoritative status from incidentRegistry
        if (it->second.getStatus() != INC_PENDING) {
            // Stale entry: already dispatched, acknowledged, resolved, or cancelled
            continue;
        }

        cout << "  [ResponseManager] Triaging highest-priority emergency:" << endl;
        it->second.displayIncident();

        // Initiate priority-based tiered dispatch on authoritative incident
        dispatchEmergency(it->second, it->second.getCreationTimestamp());
        return;
    }

    cout << "  [ResponseManager] No pending emergencies to process." << endl;
}

bool ResponseManager::hasPendingIncidents() const {
    priority_queue<Incident> tempQueue = incidentQueue;
    while (!tempQueue.empty()) {
        int id = tempQueue.top().getIncidentID();
        auto it = incidentRegistry.find(id);
        if (it != incidentRegistry.end() && it->second.getStatus() == INC_PENDING) {
            return true;
        }
        tempQueue.pop();
    }
    return false;
}

int ResponseManager::getPendingIncidentCount() const {
    int count = 0;
    priority_queue<Incident> tempQueue = incidentQueue;
    while (!tempQueue.empty()) {
        int id = tempQueue.top().getIncidentID();
        auto it = incidentRegistry.find(id);
        if (it != incidentRegistry.end() && it->second.getStatus() == INC_PENDING) {
            count++;
        }
        tempQueue.pop();
    }
    return count;
}

void ResponseManager::displayPendingIncidents() const {
    priority_queue<Incident> tempQueue = incidentQueue;
    int rank = 1;
    bool foundAny = false;
    while (!tempQueue.empty()) {
        int id = tempQueue.top().getIncidentID();
        auto it = incidentRegistry.find(id);
        if (it != incidentRegistry.end() && it->second.getStatus() == INC_PENDING) {
            if (!foundAny) {
                cout << "  Pending Incidents in Priority Queue (Highest -> Lowest Severity):" << endl;
                foundAny = true;
            }
            cout << "    Priority #" << rank++ << ":" << endl;
            it->second.displayIncident();
        }
        tempQueue.pop();
    }
    if (!foundAny) {
        cout << "  (No pending incidents in the queue)" << endl;
    }
}

// Phase 1 compatibility method
void ResponseManager::dispatchAlerts(const Incident& incident) {
    dispatchEmergency(incident, incident.getCreationTimestamp());
}

// ---------------------------------------------------------------
// Priority Escalation via Simulated Time
// ---------------------------------------------------------------

vector<int> ResponseManager::updateCurrentTime(const string& currentTime) {
    vector<int> escalatedIDs;

    int newSec = -1;
    if (!parseTimeOfDay(currentTime, newSec)) {
        cout << "  [ResponseManager Error] Invalid simulated time: '" << currentTime << "'." << endl;
        return escalatedIDs;
    }

    int effectiveNewSec = newSec;
    if (currentTimeSeconds != -1 && newSec < (currentTimeSeconds % 86400)) {
        // Legitimate forward midnight rollover:
        if ((currentTimeSeconds % 86400) >= 64800 && newSec <= 21600 &&
            ((newSec + 86400) - (currentTimeSeconds % 86400) <= 21600)) {
            effectiveNewSec = (currentTimeSeconds / 86400 + 1) * 86400 + newSec;
        } else {
            cout << "  [ResponseManager Warning] Cannot rewind simulated time from '"
                 << currentTimeStr << "' to '" << currentTime << "'." << endl;
            return escalatedIDs;
        }
    } else if (currentTimeSeconds != -1) {
        effectiveNewSec = (currentTimeSeconds / 86400) * 86400 + newSec;
    }

    currentTimeSeconds = effectiveNewSec;
    currentTimeStr = currentTime;

    // Check all active incidents awaiting acknowledgement
    for (auto& pair : incidentRegistry) {
        Incident& inc = pair.second;
        if (inc.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT) {
            if (currentTimeSeconds >= inc.getTierDeadlineSeconds()) {
                int currentTier = inc.getCurrentTier();
                cout << "\n  [ResponseManager TIMEOUT] Incident #" << inc.getIncidentID()
                     << " Tier " << currentTier << " acknowledgement timeout expired at "
                     << currentTime << "!" << endl;

                int nextTier = currentTier + 1;
                bool escalated = false;

                while (nextTier <= 3) {
                    if (dispatchTier(inc.getIncidentID(), nextTier, currentTime)) {
                        escalated = true;
                        escalatedIDs.push_back(inc.getIncidentID());
                        break;
                    } else {
                        cout << "  [ResponseManager Notice] No Priority " << nextTier
                             << " contacts registered. Escalating to next tier..." << endl;
                        nextTier++;
                    }
                }

                if (!escalated) {
                    cout << "  [ResponseManager ALERT] Incident #" << inc.getIncidentID()
                         << " has EXHAUSTED ALL ESCALATION TIERS with no acknowledgement!" << endl;
                    cout << "  Status marked as ESCALATION_EXHAUSTED." << endl;
                    inc.setStatus(INC_ESCALATION_EXHAUSTED);
                    escalatedIDs.push_back(inc.getIncidentID());
                }
            }
        }
    }

    return escalatedIDs;
}

// ---------------------------------------------------------------
// Incident Lifecycle Management (Phase 4.5)
// ---------------------------------------------------------------

bool ResponseManager::acknowledgeIncident(int incidentID, int contactID, const string& timestamp) {
    auto it = incidentRegistry.find(incidentID);
    if (it == incidentRegistry.end()) {
        cout << "  [ResponseManager Error] Incident #" << incidentID << " not found." << endl;
        return false;
    }

    Incident& incident = it->second;

    // Look up contact details
    string contactName = "Unknown Contact";
    int priority = 1;
    bool contactFound = false;
    for (const auto& c : contacts) {
        if (c.getContactID() == contactID) {
            contactName = c.getName();
            priority = c.getPriorityLevel();
            contactFound = true;
            break;
        }
    }

    if (!contactFound) {
        cout << "  [ResponseManager Error] Contact #" << contactID << " is not a registered contact." << endl;
        return false;
    }

    string actualTime = timestamp.empty() ? (currentTimeStr.empty() ? "00:00:00" : currentTimeStr) : timestamp;
    bool success = incident.recordAcknowledgement(contactID, contactName, priority, actualTime);
    if (success) {
        cout << "  [ResponseManager] Incident #" << incidentID << " successfully ACKNOWLEDGED by "
             << contactName << " (Contact #" << contactID << ", Priority " << priority
             << ") at " << actualTime << "." << endl;
        cout << "  Priority escalation has been halted for this incident." << endl;
    }
    return success;
}

bool ResponseManager::resolveIncident(int incidentID, const string& resolutionNotes, const string& timestamp) {
    auto it = incidentRegistry.find(incidentID);
    if (it == incidentRegistry.end()) {
        cout << "  [ResponseManager Error] Incident #" << incidentID << " not found." << endl;
        return false;
    }

    Incident& incident = it->second;
    string actualTime = timestamp.empty() ? (currentTimeStr.empty() ? "00:00:00" : currentTimeStr) : timestamp;

    bool success = incident.recordResolution(resolutionNotes, actualTime);
    if (success) {
        cout << "  [ResponseManager] Incident #" << incidentID << " has been successfully RESOLVED at "
             << actualTime << "." << endl;
        cout << "  Resolution Note: " << incident.getResolutionNotes() << endl;
    }
    return success;
}

bool ResponseManager::cancelIncident(int incidentID, const string& cancellationReason, const string& timestamp) {
    auto it = incidentRegistry.find(incidentID);
    if (it == incidentRegistry.end()) {
        cout << "  [ResponseManager Error] Incident #" << incidentID << " not found." << endl;
        return false;
    }

    Incident& incident = it->second;
    string actualTime = timestamp.empty() ? (currentTimeStr.empty() ? "00:00:00" : currentTimeStr) : timestamp;

    bool success = incident.recordCancellation(cancellationReason, actualTime);
    if (success) {
        cout << "  [ResponseManager] Incident #" << incidentID << " has been CANCELLED at "
             << actualTime << " (Reason: " << incident.getCancellationReason() << ")." << endl;
    }
    return success;
}

// ---------------------------------------------------------------
// Queries & Status Inspections
// ---------------------------------------------------------------

bool ResponseManager::getIncidentByID(int incidentID, Incident& outIncident) const {
    auto it = incidentRegistry.find(incidentID);
    if (it != incidentRegistry.end()) {
        outIncident = it->second;
        return true;
    }
    return false;
}

bool ResponseManager::hasActiveIncidents() const {
    for (const auto& pair : incidentRegistry) {
        if (pair.second.isActive()) return true;
    }
    return false;
}

vector<Incident> ResponseManager::getActiveIncidents() const {
    vector<Incident> active;
    for (int id : incidentHistoryIDs) {
        auto it = incidentRegistry.find(id);
        if (it != incidentRegistry.end() && it->second.isActive()) {
            active.push_back(it->second);
        }
    }
    return active;
}

vector<Incident> ResponseManager::getAllIncidents() const {
    vector<Incident> all;
    for (int id : incidentHistoryIDs) {
        auto it = incidentRegistry.find(id);
        if (it != incidentRegistry.end()) {
            all.push_back(it->second);
        }
    }
    return all;
}

// ---------------------------------------------------------------
// Display Functions
// ---------------------------------------------------------------

void ResponseManager::displayActiveIncidents() const {
    cout << "\n--- [Active Emergencies & Incidents] ---" << endl;
    vector<Incident> active = getActiveIncidents();
    if (active.empty()) {
        cout << "  (No active incidents currently requiring response)" << endl;
    } else {
        for (const auto& inc : active) {
            inc.displayDetailedIncident();
            cout << endl;
        }
    }
    cout << "----------------------------------------" << endl;
}

void ResponseManager::displayIncidentHistory() const {
    cout << "\n--- [Complete Incident History] ---" << endl;
    if (incidentRegistry.empty()) {
        cout << "  (No incidents recorded in system history)" << endl;
    } else {
        for (int id : incidentHistoryIDs) {
            auto it = incidentRegistry.find(id);
            if (it != incidentRegistry.end()) {
                it->second.displayDetailedIncident();
                cout << endl;
            }
        }
    }
    cout << "-----------------------------------" << endl;
}

void ResponseManager::displayNotificationHistory(int incidentID) const {
    cout << "\n--- [Emergency Notification Audit Trail] ---" << endl;
    if (incidentID > 0) {
        auto it = incidentRegistry.find(incidentID);
        if (it != incidentRegistry.end()) {
            it->second.displayNotificationHistory();
        } else {
            cout << "  Incident #" << incidentID << " not found." << endl;
        }
    } else {
        if (incidentRegistry.empty()) {
            cout << "  (No incidents recorded)" << endl;
        } else {
            for (int id : incidentHistoryIDs) {
                auto it = incidentRegistry.find(id);
                if (it != incidentRegistry.end()) {
                    it->second.displayNotificationHistory();
                    cout << endl;
                }
            }
        }
    }
    cout << "--------------------------------------------" << endl;
}

void ResponseManager::displayIncidentStatistics() const {
    cout << "\n--- [Emergency Response & Incident Statistics] ---" << endl;
    cout << "  Total Incidents Recorded:     " << getTotalIncidentCount() << endl;
    cout << "  Active Incidents:             " << getActiveIncidentCount() << endl;
    cout << "  Acknowledged Incidents:       " << getAcknowledgedIncidentCount() << endl;
    cout << "  Resolved Incidents:           " << getResolvedIncidentCount() << endl;
    cout << "  Cancelled Incidents:          " << getCancelledIncidentCount() << endl;
    cout << "  Escalation Exhausted:         " << getEscalationExhaustedCount() << endl;
    cout << "  Total Notifications Sent:     " << getTotalNotificationCount() << endl;
    cout << "  Acknowledgement Timeout:      " << acknowledgementTimeoutMinutes << " minutes" << endl;
    cout << "--------------------------------------------------" << endl;
}

// ---------------------------------------------------------------
// Statistics Getters
// ---------------------------------------------------------------

int ResponseManager::getTotalIncidentCount() const {
    return static_cast<int>(incidentRegistry.size());
}

int ResponseManager::getActiveIncidentCount() const {
    int count = 0;
    for (const auto& pair : incidentRegistry) {
        if (pair.second.isActive()) count++;
    }
    return count;
}

int ResponseManager::getAcknowledgedIncidentCount() const {
    int count = 0;
    for (const auto& pair : incidentRegistry) {
        if (pair.second.getStatus() == INC_ACKNOWLEDGED) count++;
    }
    return count;
}

int ResponseManager::getResolvedIncidentCount() const {
    int count = 0;
    for (const auto& pair : incidentRegistry) {
        if (pair.second.getStatus() == INC_RESOLVED) count++;
    }
    return count;
}

int ResponseManager::getCancelledIncidentCount() const {
    int count = 0;
    for (const auto& pair : incidentRegistry) {
        if (pair.second.getStatus() == INC_CANCELLED) count++;
    }
    return count;
}

int ResponseManager::getEscalationExhaustedCount() const {
    int count = 0;
    for (const auto& pair : incidentRegistry) {
        if (pair.second.getStatus() == INC_ESCALATION_EXHAUSTED) count++;
    }
    return count;
}

int ResponseManager::getTotalNotificationCount() const {
    int total = 0;
    for (const auto& pair : incidentRegistry) {
        total += pair.second.getNotificationCount();
    }
    return total;
}

