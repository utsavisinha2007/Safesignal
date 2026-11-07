#ifndef VERIFICATION_MANAGER_H
#define VERIFICATION_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Event.h"

using namespace std;

// Status values for a safety verification request lifecycle
enum VerificationStatus {
    VSTAT_PENDING,         // Awaiting user response
    VSTAT_CONFIRMED_SAFE,  // User confirmed they are safe (false alarm)
    VSTAT_HELP_REQUESTED,  // User explicitly requested emergency help
    VSTAT_TIMED_OUT        // No response within the timeout deadline
};

// A safety verification request triggered when risk reaches the Concerning threshold.
// Tracks the verification lifecycle from creation through resolution or timeout.
struct VerificationRequest {
    int requestID;              // Unique verification request ID
    int riskScore;              // Risk score at the time of creation
    string riskLevel;           // Risk level description at the time of creation
    string triggerLocation;     // Location associated with the triggering event
    string creationTimestamp;   // Simulated timestamp when the request was created
    string deadlineTimestamp;   // Simulated timestamp when the request times out
    int creationSeconds;        // Internal: creation time in seconds-of-day for comparison
    int deadlineSeconds;        // Internal: deadline time in seconds-of-day for comparison
    VerificationStatus status;  // Current lifecycle status
    string resolutionTimestamp; // Simulated timestamp when the request was resolved (empty if pending)
};

// Manages safety verification requests triggered by concerning risk patterns.
// Uses simulated time only. No real clocks, threads, or background timers.
//
// Data structures used:
//   - unordered_map<int, VerificationRequest>: Hash table storing all requests keyed by requestID.
//     Provides O(1) average-case lookup, insertion, and update by ID.
//     Preserves completed requests for history and statistics.
//   - vector<int> pendingRequestIDs: Ordered list of IDs of currently-pending requests.
//     Enables efficient iteration over only the active subset without scanning the full map.
//     Maintained in insertion order for deterministic timeout evaluation.
class VerificationManager {
private:
    unordered_map<int, VerificationRequest> requestMap; // All requests (active + history)
    vector<int> pendingRequestIDs;                      // IDs of currently PENDING requests
    int nextRequestID;          // Auto-incrementing unique ID counter (starts at 5000)
    int timeoutMinutes;         // Configurable verification timeout in minutes (default: 3)
    int currentTimeSeconds;     // Latest simulated time-of-day in seconds (-1 if unset)
    string currentTimeStr;      // String representation of latest simulated time

    // Strict time-of-day parser: accepts HH:MM and HH:MM:SS formats
    // Validates hours (0-23), minutes (0-59), seconds (0-59)
    bool parseTimeOfDay(const string& timeStr, int& outSeconds) const;

    // Formats seconds-of-day back into HH:MM:SS string
    string formatTime(int totalSeconds) const;

    // Returns a human-readable status string for a VerificationStatus enum value
    string statusToString(VerificationStatus status) const;

    // Removes a request ID from the pendingRequestIDs vector
    void removePendingID(int requestID);

public:
    // Constructor with configurable timeout in minutes (default: 3 minutes)
    VerificationManager(int timeoutMins = 3);
    ~VerificationManager();

    // Configuration
    void setTimeoutMinutes(int minutes);
    int getTimeoutMinutes() const;

    // Core verification operations
    // Creates a new verification request. Returns the assigned request ID.
    // If there is already an active (PENDING) request, rejects the duplicate and returns -1.
    int createVerificationRequest(int riskScore, const string& riskLevel,
                                  const string& location, const string& timestamp);

    // User confirms they are safe. Returns true on success, false if request is not PENDING.
    bool confirmSafe(int requestID);

    // User requests emergency help. Returns true on success, false if request is not PENDING.
    bool requestHelp(int requestID);

    // Advances simulated time and evaluates pending requests for timeout.
    // Returns a vector of request IDs that timed out during this update.
    vector<int> updateCurrentTime(const string& currentTime);

    // Query operations
    vector<VerificationRequest> getPendingRequests() const;
    bool getRequestByID(int requestID, VerificationRequest& outRequest) const;
    bool hasActivePendingRequest() const;
    int getActiveRequestID() const; // Returns the ID of the active PENDING request, or -1 if none

    // Display operations
    void displayPendingRequests() const;
    void displayVerificationStatistics() const;
    void displayRequestHistory() const;

    // Statistics
    int getTotalRequestCount() const;
    int getPendingCount() const;
    int getConfirmedSafeCount() const;
    int getHelpRequestedCount() const;
    int getTimedOutCount() const;
};

#endif // VERIFICATION_MANAGER_H

