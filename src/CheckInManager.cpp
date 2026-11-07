#include "../include/CheckInManager.h"
#include <cctype>

using namespace std;

// Constructor: Initializes the check-in list, grace period, and counters
CheckInManager::CheckInManager(int gracePeriod) {
    eventCounter = 8000; // Requirement: First generated missed-check-in Event has ID 8000
    nextCheckInID = 1;
    gracePeriodMinutes = (gracePeriod >= 0) ? gracePeriod : 5;
}

// Destructor
CheckInManager::~CheckInManager() {
    clearCheckIns();
}

// Configures the grace period in minutes
void CheckInManager::setGracePeriod(int minutes) {
    if (minutes >= 0) {
        gracePeriodMinutes = minutes;
        cout << "  [CheckInManager] Grace period configured to " << gracePeriodMinutes << " minutes." << endl;
    } else {
        cout << "  [CheckInManager Error] Grace period cannot be negative." << endl;
    }
}

// Returns the current grace period in minutes
int CheckInManager::getGracePeriod() const {
    return gracePeriodMinutes;
}

// Strict time parser and validator:
// - Accepts only "HH:MM" or "HH:MM:SS"
// - Verifies every character is a digit or ':'
// - Validates hours (0-23), minutes (0-59), seconds (0-59)
// - Rejects trailing/leading spaces or non-numeric characters
// - Calculates exact seconds from midnight (no lexicographical comparisons)
bool CheckInManager::parseTimeToSeconds(const string& timeStr, int& outSeconds) const {
    outSeconds = -1;

    if (timeStr.empty()) {
        return false;
    }

    // Check for invalid characters and count colons
    int colonCount = 0;
    for (char c : timeStr) {
        if (c == ':') {
            colonCount++;
        } else if (!isdigit(static_cast<unsigned char>(c))) {
            return false; // Rejects letters, symbols, spaces, and trailing characters
        }
    }

    // Must have exactly 1 colon (HH:MM) or 2 colons (HH:MM:SS)
    if (colonCount != 1 && colonCount != 2) {
        return false;
    }

    size_t firstColon = timeStr.find(':');
    string hourStr = timeStr.substr(0, firstColon);
    if (hourStr.empty() || hourStr.length() > 2) {
        return false;
    }

    string minStr = "";
    string secStr = "0";

    if (colonCount == 1) {
        // HH:MM format
        minStr = timeStr.substr(firstColon + 1);
        if (minStr.empty() || minStr.length() > 2) {
            return false;
        }
    } else {
        // HH:MM:SS format
        size_t secondColon = timeStr.find(':', firstColon + 1);
        minStr = timeStr.substr(firstColon + 1, secondColon - firstColon - 1);
        secStr = timeStr.substr(secondColon + 1);
        if (minStr.empty() || minStr.length() > 2 || secStr.empty() || secStr.length() > 2) {
            return false;
        }
    }

    int hours = stoi(hourStr);
    int minutes = stoi(minStr);
    int seconds = stoi(secStr);

    // Range checks
    if (hours < 0 || hours > 23) {
        return false;
    }
    if (minutes < 0 || minutes > 59) {
        return false;
    }
    if (seconds < 0 || seconds > 59) {
        return false;
    }

    outSeconds = hours * 3600 + minutes * 60 + seconds;
    return true;
}

// Adds a new scheduled check-in time after strict time validation
int CheckInManager::addCheckIn(const string& scheduledTime, const string& description,
                               const string& location) {
    int parsedSeconds = 0;
    if (!parseTimeToSeconds(scheduledTime, parsedSeconds)) {
        cout << "  [CheckInManager Error] Invalid scheduled time: '" << scheduledTime
             << "'. Must be valid HH:MM or HH:MM:SS (hours: 0-23, mins: 0-59, secs: 0-59)." << endl;
        return -1;
    }

    CheckIn newCheckIn;
    newCheckIn.checkInID = nextCheckInID++;
    newCheckIn.scheduledTime = scheduledTime;
    newCheckIn.description = description.empty() ? "Scheduled Check-in" : description;
    newCheckIn.location = location.empty() ? "Unknown" : location;
    newCheckIn.isCompleted = false;
    newCheckIn.isMissed = false;

    checkIns.push_back(newCheckIn);

    cout << "  [CheckInManager] Added Check-in #" << newCheckIn.checkInID
         << " at " << newCheckIn.scheduledTime << " (" << newCheckIn.description << ")." << endl;

    return newCheckIn.checkInID;
}

// Marks a specific scheduled check-in as completed.
// If it was already missed, preserves the missed flag and logs it as LATE CONFIRMATION.
bool CheckInManager::completeCheckIn(int checkInID) {
    for (size_t i = 0; i < checkIns.size(); i++) {
        if (checkIns[i].checkInID == checkInID) {
            if (checkIns[i].isCompleted) {
                if (checkIns[i].isMissed) {
                    cout << "  [CheckInManager] Check-in #" << checkInID
                         << " is already recorded as LATE CONFIRMATION." << endl;
                } else {
                    cout << "  [CheckInManager] Check-in #" << checkInID
                         << " is already marked as COMPLETED." << endl;
                }
                return true;
            }

            checkIns[i].isCompleted = true;

            if (checkIns[i].isMissed) {
                cout << "  [CheckInManager] Notice: Check-in #" << checkInID
                     << " was previously MISSED. Recorded as LATE CONFIRMATION." << endl;
            } else {
                cout << "  [CheckInManager] Check-in #" << checkInID
                     << " successfully marked as COMPLETED (On-time)." << endl;
            }
            return true;
        }
    }

    cout << "  [CheckInManager Error] Check-in ID #" << checkInID << " was not found." << endl;
    return false;
}

// Evaluates simulated current time against pending check-ins.
// Generates an Event (starting at 8000) for check-ins whose scheduled time + grace period has elapsed.
// Prevents duplicate event generation.
vector<Event> CheckInManager::updateCurrentTime(const string& currentTime) {
    vector<Event> newlyMissedEvents;

    int currentSeconds = 0;
    if (!parseTimeToSeconds(currentTime, currentSeconds)) {
        cout << "  [CheckInManager Error] Invalid simulated current time: '" << currentTime
             << "'. Must be valid HH:MM or HH:MM:SS (hours: 0-23, mins: 0-59, secs: 0-59)." << endl;
        return newlyMissedEvents;
    }

    int gracePeriodSeconds = gracePeriodMinutes * 60;

    for (size_t i = 0; i < checkIns.size(); i++) {
        // Evaluate only pending check-ins that haven't been completed or already flagged
        if (!checkIns[i].isCompleted && !checkIns[i].isMissed) {
            int scheduledSeconds = 0;
            parseTimeToSeconds(checkIns[i].scheduledTime, scheduledSeconds);

            // Deadline is scheduled time plus the configured grace period
            int deadlineSeconds = scheduledSeconds + gracePeriodSeconds;

            if (currentSeconds > deadlineSeconds) {
                checkIns[i].isMissed = true; // Flag as missed so duplicate events are never generated

                // Use post-increment: first event receives ID 8000, next receives 8001, etc.
                int assignedEventID = eventCounter++;

                string loc = checkIns[i].location.empty() ? "Unknown" : checkIns[i].location;
                Event missedEvent(assignedEventID, "Missed Check-in", loc, currentTime);

                cout << "  [CheckInManager ALERT] Missed Safety Check-in Detected!" << endl;
                cout << "    Check-in ID:    #" << checkIns[i].checkInID
                     << " (" << checkIns[i].description << ")" << endl;
                cout << "    Scheduled Time: " << checkIns[i].scheduledTime << endl;
                cout << "    Grace Deadline: +" << gracePeriodMinutes << " min (" << (deadlineSeconds / 3600) << ":"
                     << ((deadlineSeconds % 3600) / 60) << ")" << endl;
                cout << "    Current Time:   " << currentTime << endl;
                cout << "    Location:       " << loc << endl;
                cout << "    Generated Event #" << missedEvent.getEventID()
                     << " (" << missedEvent.getEventType() << ")" << endl;

                newlyMissedEvents.push_back(missedEvent);
            }
        }
    }

    return newlyMissedEvents;
}

// Displays all scheduled check-ins and their current statuses:
// - PENDING: Not yet completed and deadline has not passed
// - COMPLETED: Confirmed on time before deadline passed
// - MISSED: Deadline + grace period elapsed without confirmation
// - LATE CONFIRMATION: Confirmed after deadline had already passed
void CheckInManager::displayCheckIns() const {
    cout << "\n--- [Scheduled Safety Check-ins] ---" << endl;
    cout << "  Configured Grace Period: " << gracePeriodMinutes << " minutes" << endl;

    if (checkIns.empty()) {
        cout << "  (No scheduled check-ins recorded)" << endl;
        cout << "-------------------------------------\n" << endl;
        return;
    }

    for (size_t i = 0; i < checkIns.size(); i++) {
        string statusStr = "PENDING";
        if (checkIns[i].isCompleted && !checkIns[i].isMissed) {
            statusStr = "COMPLETED";
        } else if (checkIns[i].isCompleted && checkIns[i].isMissed) {
            statusStr = "LATE CONFIRMATION";
        } else if (!checkIns[i].isCompleted && checkIns[i].isMissed) {
            statusStr = "MISSED";
        }

        cout << "  Check-in #" << checkIns[i].checkInID
             << " | Time: " << checkIns[i].scheduledTime
             << " | Location: " << checkIns[i].location
             << " | Note: " << checkIns[i].description
             << " | Status: " << statusStr << endl;
    }
    cout << "-------------------------------------\n" << endl;
}

// Clears all check-ins (used when a journey ends)
void CheckInManager::clearCheckIns() {
    checkIns.clear();
}

// Returns total number of registered check-in records
int CheckInManager::getCheckInCount() const {
    return static_cast<int>(checkIns.size());
}

// Returns count of pending check-ins
int CheckInManager::getPendingCount() const {
    int count = 0;
    for (size_t i = 0; i < checkIns.size(); i++) {
        if (!checkIns[i].isCompleted && !checkIns[i].isMissed) {
            count++;
        }
    }
    return count;
}

// Returns count of on-time completed check-ins
int CheckInManager::getCompletedCount() const {
    int count = 0;
    for (size_t i = 0; i < checkIns.size(); i++) {
        if (checkIns[i].isCompleted && !checkIns[i].isMissed) {
            count++;
        }
    }
    return count;
}

// Returns count of check-ins confirmed late (after missed alert)
int CheckInManager::getLateConfirmationCount() const {
    int count = 0;
    for (size_t i = 0; i < checkIns.size(); i++) {
        if (checkIns[i].isCompleted && checkIns[i].isMissed) {
            count++;
        }
    }
    return count;
}

// Returns count of currently missed, unconfirmed check-ins
int CheckInManager::getMissedCount() const {
    int count = 0;
    for (size_t i = 0; i < checkIns.size(); i++) {
        if (!checkIns[i].isCompleted && checkIns[i].isMissed) {
            count++;
        }
    }
    return count;
}

