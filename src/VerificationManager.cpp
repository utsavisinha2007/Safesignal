#include "../include/VerificationManager.h"
#include <cctype>
#include <iomanip>

using namespace std;

// ---------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------

VerificationManager::VerificationManager(int timeoutMins) {
    nextRequestID = 5000; // First request will be 5000
    timeoutMinutes = (timeoutMins > 0) ? timeoutMins : 3;
    currentTimeSeconds = -1;
    currentTimeStr = "";
}

VerificationManager::~VerificationManager() {
    // STL containers clean up automatically
}

// ---------------------------------------------------------------
// Private Helpers
// ---------------------------------------------------------------

// Strict time-of-day parser supporting HH:MM and HH:MM:SS
// Validates hours (0-23), minutes (0-59), seconds (0-59)
// Rejects trailing characters, non-numeric content, and out-of-range values
bool VerificationManager::parseTimeOfDay(const string& timeStr, int& outSeconds) const {
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

// Formats total seconds-of-day into HH:MM:SS string
string VerificationManager::formatTime(int totalSeconds) const {
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

// Returns human-readable status string
string VerificationManager::statusToString(VerificationStatus status) const {
    switch (status) {
        case VSTAT_PENDING:        return "PENDING";
        case VSTAT_CONFIRMED_SAFE: return "CONFIRMED SAFE";
        case VSTAT_HELP_REQUESTED: return "HELP REQUESTED";
        case VSTAT_TIMED_OUT:      return "TIMED OUT";
        default:                   return "UNKNOWN";
    }
}

// Removes a request ID from the pendingRequestIDs vector
void VerificationManager::removePendingID(int requestID) {
    for (size_t i = 0; i < pendingRequestIDs.size(); i++) {
        if (pendingRequestIDs[i] == requestID) {
            pendingRequestIDs.erase(pendingRequestIDs.begin() + static_cast<long>(i));
            return;
        }
    }
}

// ---------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------

void VerificationManager::setTimeoutMinutes(int minutes) {
    if (minutes > 0) {
        timeoutMinutes = minutes;
        cout << "  [VerificationManager] Verification timeout configured to "
             << timeoutMinutes << " minutes." << endl;
    } else {
        cout << "  [VerificationManager Error] Timeout must be positive." << endl;
    }
}

int VerificationManager::getTimeoutMinutes() const {
    return timeoutMinutes;
}

// ---------------------------------------------------------------
// Core Verification Operations
// ---------------------------------------------------------------

// Creates a new verification request when risk becomes Concerning.
// Rejects duplicate requests if there is already an active PENDING request.
// Validates the creation timestamp. Calculates the deadline as creation + timeout.
int VerificationManager::createVerificationRequest(int riskScore, const string& riskLevel,
                                                    const string& location, const string& timestamp) {
    // Validate the creation timestamp
    int creationSec = -1;
    if (!parseTimeOfDay(timestamp, creationSec)) {
        cout << "  [VerificationManager Error] Invalid creation timestamp: '"
             << timestamp << "'. Request not created." << endl;
        return -1;
    }

    // Handle simulated time progression and midnight rollover
    int effectiveCreationSec = creationSec;
    if (currentTimeSeconds != -1 && creationSec < (currentTimeSeconds % 86400)) {
        if ((currentTimeSeconds % 86400) >= 64800 && creationSec <= 21600 &&
            ((creationSec + 86400) - (currentTimeSeconds % 86400) <= 21600)) {
            effectiveCreationSec = (currentTimeSeconds / 86400 + 1) * 86400 + creationSec;
        } else {
            cout << "  [VerificationManager Error] Creation timestamp '" << timestamp
                 << "' is earlier than current simulated time '" << currentTimeStr
                 << "'. Request not created." << endl;
            return -1;
        }
    } else if (currentTimeSeconds != -1) {
        effectiveCreationSec = (currentTimeSeconds / 86400) * 86400 + creationSec;
    }

    // Prevent duplicate active verification requests
    if (!pendingRequestIDs.empty()) {
        int activeID = pendingRequestIDs.back();
        cout << "  [VerificationManager Warning] An active verification request already exists (Request #"
             << activeID << "). Cannot create a duplicate." << endl;
        return -1;
    }

    // Calculate deadline
    int deadlineSec = effectiveCreationSec + (timeoutMinutes * 60);
    // Handle midnight wraparound for deadline display (but internally track raw seconds)
    string deadlineStr = formatTime(deadlineSec % 86400);

    // Build the request
    VerificationRequest req;
    req.requestID = nextRequestID;
    req.riskScore = riskScore;
    req.riskLevel = riskLevel;
    req.triggerLocation = location;
    req.creationTimestamp = timestamp;
    req.deadlineTimestamp = deadlineStr;
    req.creationSeconds = effectiveCreationSec;
    req.deadlineSeconds = deadlineSec;
    req.status = VSTAT_PENDING;
    req.resolutionTimestamp = "";

    // Store in the hash map and pending list
    requestMap[nextRequestID] = req;
    pendingRequestIDs.push_back(nextRequestID);

    // Update simulated time
    currentTimeSeconds = effectiveCreationSec;
    currentTimeStr = timestamp;

    cout << "  [VerificationManager] Safety Verification Request #" << nextRequestID << " created." << endl;
    cout << "    Risk Score: " << riskScore << " (" << riskLevel << ")" << endl;
    cout << "    Location:   " << location << endl;
    cout << "    Created:    " << timestamp << endl;
    cout << "    Deadline:   " << deadlineStr << " (timeout: " << timeoutMinutes << " min)" << endl;
    cout << "    Status:     PENDING - Awaiting user response." << endl;

    int assignedID = nextRequestID;
    nextRequestID++;
    return assignedID;
}

// User confirms they are safe. Only valid on PENDING requests.
bool VerificationManager::confirmSafe(int requestID) {
    auto it = requestMap.find(requestID);
    if (it == requestMap.end()) {
        cout << "  [VerificationManager Error] Request #" << requestID
             << " not found. Invalid request ID." << endl;
        return false;
    }

    VerificationRequest& req = it->second;

    if (req.status != VSTAT_PENDING) {
        cout << "  [VerificationManager Error] Request #" << requestID
             << " cannot be confirmed safe. Current status: "
             << statusToString(req.status) << "." << endl;
        return false;
    }

    req.status = VSTAT_CONFIRMED_SAFE;
    req.resolutionTimestamp = currentTimeStr.empty() ? req.creationTimestamp : currentTimeStr;

    removePendingID(requestID);

    cout << "  [VerificationManager] Request #" << requestID
         << " resolved: USER CONFIRMED SAFE." << endl;
    cout << "    No emergency contacts will be alerted." << endl;
    return true;
}

// User explicitly requests emergency help. Only valid on PENDING requests.
bool VerificationManager::requestHelp(int requestID) {
    auto it = requestMap.find(requestID);
    if (it == requestMap.end()) {
        cout << "  [VerificationManager Error] Request #" << requestID
             << " not found. Invalid request ID." << endl;
        return false;
    }

    VerificationRequest& req = it->second;

    if (req.status != VSTAT_PENDING) {
        cout << "  [VerificationManager Error] Request #" << requestID
             << " cannot request help. Current status: "
             << statusToString(req.status) << "." << endl;
        return false;
    }

    req.status = VSTAT_HELP_REQUESTED;
    req.resolutionTimestamp = currentTimeStr.empty() ? req.creationTimestamp : currentTimeStr;

    removePendingID(requestID);

    cout << "  [VerificationManager ALERT] Request #" << requestID
         << " resolved: USER REQUESTED EMERGENCY HELP!" << endl;
    cout << "    Emergency escalation should be initiated immediately." << endl;
    return true;
}

// Advances simulated time and checks pending requests for timeout.
// Returns a vector of request IDs that timed out during this update.
vector<int> VerificationManager::updateCurrentTime(const string& currentTime) {
    vector<int> timedOutIDs;

    int newTimeSec = -1;
    if (!parseTimeOfDay(currentTime, newTimeSec)) {
        cout << "  [VerificationManager Error] Invalid simulated time: '"
             << currentTime << "'. Time update rejected." << endl;
        return timedOutIDs;
    }

    // Support forward midnight rollover and reject backward time jumps
    int effectiveNewSec = newTimeSec;
    if (currentTimeSeconds != -1 && newTimeSec < (currentTimeSeconds % 86400)) {
        if ((currentTimeSeconds % 86400) >= 64800 && newTimeSec <= 21600 &&
            ((newTimeSec + 86400) - (currentTimeSeconds % 86400) <= 21600)) {
            effectiveNewSec = (currentTimeSeconds / 86400 + 1) * 86400 + newTimeSec;
        } else {
            cout << "  [VerificationManager Warning] Cannot rewind simulated time from '"
                 << currentTimeStr << "' to '" << currentTime << "'. Update rejected." << endl;
            return timedOutIDs;
        }
    } else if (currentTimeSeconds != -1) {
        effectiveNewSec = (currentTimeSeconds / 86400) * 86400 + newTimeSec;
    }

    // Commit time advancement
    currentTimeSeconds = effectiveNewSec;
    currentTimeStr = currentTime;

    // Evaluate all pending requests for timeout
    // Iterate over a copy of pendingRequestIDs because removePendingID modifies the vector
    vector<int> pendingCopy = pendingRequestIDs;
    for (int id : pendingCopy) {
        auto it = requestMap.find(id);
        if (it != requestMap.end()) {
            VerificationRequest& req = it->second;
            if (req.status == VSTAT_PENDING && currentTimeSeconds >= req.deadlineSeconds) {
                req.status = VSTAT_TIMED_OUT;
                req.resolutionTimestamp = currentTime;
                removePendingID(id);

                cout << "  [VerificationManager TIMEOUT] Request #" << id
                     << " has TIMED OUT! No user response received by deadline "
                     << req.deadlineTimestamp << "." << endl;
                cout << "    Emergency escalation should be initiated automatically." << endl;

                timedOutIDs.push_back(id);
            }
        }
    }

    return timedOutIDs;
}

// ---------------------------------------------------------------
// Query Operations
// ---------------------------------------------------------------

vector<VerificationRequest> VerificationManager::getPendingRequests() const {
    vector<VerificationRequest> result;
    for (int id : pendingRequestIDs) {
        auto it = requestMap.find(id);
        if (it != requestMap.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

bool VerificationManager::getRequestByID(int requestID, VerificationRequest& outRequest) const {
    auto it = requestMap.find(requestID);
    if (it != requestMap.end()) {
        outRequest = it->second;
        return true;
    }
    return false;
}

bool VerificationManager::hasActivePendingRequest() const {
    return !pendingRequestIDs.empty();
}

int VerificationManager::getActiveRequestID() const {
    if (pendingRequestIDs.empty()) return -1;
    return pendingRequestIDs.back();
}

// ---------------------------------------------------------------
// Display Operations
// ---------------------------------------------------------------

void VerificationManager::displayPendingRequests() const {
    cout << "\n--- [Pending Safety Verification Requests] ---" << endl;
    if (pendingRequestIDs.empty()) {
        cout << "  (No pending verification requests)" << endl;
    } else {
        for (int id : pendingRequestIDs) {
            auto it = requestMap.find(id);
            if (it != requestMap.end()) {
                const VerificationRequest& req = it->second;
                cout << "  Request #" << req.requestID
                     << " | Risk: " << req.riskScore << " (" << req.riskLevel << ")"
                     << " | Location: " << req.triggerLocation
                     << " | Created: " << req.creationTimestamp
                     << " | Deadline: " << req.deadlineTimestamp
                     << " | Status: " << statusToString(req.status) << endl;
            }
        }
    }
    cout << "-----------------------------------------------" << endl;
}

void VerificationManager::displayVerificationStatistics() const {
    cout << "\n--- [Verification Statistics] ---" << endl;
    cout << "  Total Requests:       " << getTotalRequestCount() << endl;
    cout << "  Pending:              " << getPendingCount() << endl;
    cout << "  Confirmed Safe:       " << getConfirmedSafeCount() << endl;
    cout << "  Help Requested:       " << getHelpRequestedCount() << endl;
    cout << "  Timed Out:            " << getTimedOutCount() << endl;
    cout << "  Verification Timeout: " << timeoutMinutes << " minutes" << endl;
    cout << "---------------------------------" << endl;
}

void VerificationManager::displayRequestHistory() const {
    cout << "\n--- [Verification Request History] ---" << endl;
    if (requestMap.empty()) {
        cout << "  (No verification requests in history)" << endl;
    } else {
        for (const auto& pair : requestMap) {
            const VerificationRequest& req = pair.second;
            cout << "  Request #" << req.requestID
                 << " | Risk: " << req.riskScore << " (" << req.riskLevel << ")"
                 << " | Location: " << req.triggerLocation
                 << " | Created: " << req.creationTimestamp
                 << " | Deadline: " << req.deadlineTimestamp
                 << " | Status: " << statusToString(req.status);
            if (!req.resolutionTimestamp.empty()) {
                cout << " | Resolved: " << req.resolutionTimestamp;
            }
            cout << endl;
        }
    }
    cout << "--------------------------------------" << endl;
}

// ---------------------------------------------------------------
// Statistics
// ---------------------------------------------------------------

int VerificationManager::getTotalRequestCount() const {
    return static_cast<int>(requestMap.size());
}

int VerificationManager::getPendingCount() const {
    return static_cast<int>(pendingRequestIDs.size());
}

int VerificationManager::getConfirmedSafeCount() const {
    int count = 0;
    for (const auto& pair : requestMap) {
        if (pair.second.status == VSTAT_CONFIRMED_SAFE) count++;
    }
    return count;
}

int VerificationManager::getHelpRequestedCount() const {
    int count = 0;
    for (const auto& pair : requestMap) {
        if (pair.second.status == VSTAT_HELP_REQUESTED) count++;
    }
    return count;
}

int VerificationManager::getTimedOutCount() const {
    int count = 0;
    for (const auto& pair : requestMap) {
        if (pair.second.status == VSTAT_TIMED_OUT) count++;
    }
    return count;
}


