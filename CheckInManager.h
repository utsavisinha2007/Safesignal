#ifndef CHECK_IN_MANAGER_H
#define CHECK_IN_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include "Event.h"

using namespace std;

// Structure representing an individual scheduled safety check-in record
struct CheckIn {
    int checkInID;
    string scheduledTime; // Simulated scheduled time (HH:MM or HH:MM:SS)
    string description;   // Purpose or notes for this check-in
    string location;      // Expected location associated with this check-in
    bool isCompleted;     // Marked true when user confirms check-in
    bool isMissed;        // Marked true when deadline passes without confirmation
};

// Manages scheduled safety check-ins, enforces grace periods,
// and detects missed check-in deadlines without generating duplicate events.
class CheckInManager {
private:
    vector<CheckIn> checkIns; // STL vector storing scheduled check-ins
    int eventCounter;         // Unique Event ID counter (first event starts at 8000)
    int nextCheckInID;        // Auto-increment ID for check-in records
    int gracePeriodMinutes;   // Configurable grace period before a check-in is declared missed (default: 5 mins)

    // Strict time parser and validator: accepts "HH:MM" and "HH:MM:SS"
    // Validates hours (0-23), minutes (0-59), seconds (0-59)
    // Rejects non-numeric/trailing characters and returns total seconds from midnight
    bool parseTimeToSeconds(const string& timeStr, int& outSeconds) const;

public:
    // Constructor with configurable grace period (defaults to 5 minutes)
    CheckInManager(int gracePeriod = 5);
    ~CheckInManager();

    // Grace period management
    void setGracePeriod(int minutes);
    int getGracePeriod() const;

    // Adds a new scheduled check-in time after strict validation
    int addCheckIn(const string& scheduledTime, const string& description = "Scheduled Check-in",
                   const string& location = "Unknown");

    // Marks a specific scheduled check-in as completed (preserves late confirmation status)
    bool completeCheckIn(int checkInID);

    // Evaluates simulated current time against pending check-ins.
    // Returns a vector of Event objects for any check-ins that just became missed.
    vector<Event> updateCurrentTime(const string& currentTime);

    // Displays all scheduled check-ins and their current statuses
    void displayCheckIns() const;

    // Clears all check-ins (used when a journey ends)
    void clearCheckIns();

    // Inspection getters
    int getCheckInCount() const;          // Total check-ins
    int getPendingCount() const;          // Pending check-ins (unconfirmed, not yet overdue)
    int getCompletedCount() const;        // On-time completed check-ins
    int getLateConfirmationCount() const; // Confirmed after deadline had passed
    int getMissedCount() const;           // Currently overdue and unconfirmed check-ins
};

#endif // CHECK_IN_MANAGER_H
