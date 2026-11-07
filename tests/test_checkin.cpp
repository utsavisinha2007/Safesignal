#include <iostream>
#include <string>
#include <vector>
#include "CheckInManager.h"
#include "Event.h"

using namespace std;

// Global test counters
int testsPassed = 0;
int testsFailed = 0;

// Test assertion helper
void assertTest(bool condition, const string& testName) {
    if (condition) {
        cout << "[PASS] " << testName << endl;
        testsPassed++;
    } else {
        cout << "[FAIL] " << testName << endl;
        testsFailed++;
    }
}

int main() {
    cout << "====================================================" << endl;
    cout << "       RUNNING UNIT TESTS FOR CHECKINMANAGER        " << endl;
    cout << "====================================================\n" << endl;

    // ---------------------------------------------------------
    // TEST SUITE 1: Valid and Invalid Time Inputs
    // ---------------------------------------------------------
    cout << "--- Suite 1: Time Validation ---" << endl;
    {
        CheckInManager manager;

        // Valid times
        int id1 = manager.addCheckIn("14:30", "Valid HH:MM", "Campus Gate");
        assertTest(id1 > 0, "Valid time HH:MM ('14:30') accepted");

        int id2 = manager.addCheckIn("14:30:45", "Valid HH:MM:SS", "Library");
        assertTest(id2 > 0, "Valid time HH:MM:SS ('14:30:45') accepted");

        int id3 = manager.addCheckIn("00:00:00", "Midnight boundary", "Start Point");
        assertTest(id3 > 0, "Valid boundary time ('00:00:00') accepted");

        int id4 = manager.addCheckIn("23:59:59", "End of day boundary", "End Point");
        assertTest(id4 > 0, "Valid boundary time ('23:59:59') accepted");

        // Invalid times
        int idInvalid1 = manager.addCheckIn("24:00", "Invalid hour 24");
        assertTest(idInvalid1 == -1, "Invalid hour '24:00' rejected");

        int idInvalid2 = manager.addCheckIn("12:60", "Invalid minute 60");
        assertTest(idInvalid2 == -1, "Invalid minute '12:60' rejected");

        int idInvalid3 = manager.addCheckIn("12:30:65", "Invalid second 65");
        assertTest(idInvalid3 == -1, "Invalid second '12:30:65' rejected");

        int idInvalid4 = manager.addCheckIn("14:30abc", "Trailing characters");
        assertTest(idInvalid4 == -1, "Trailing characters '14:30abc' rejected");

        int idInvalid5 = manager.addCheckIn("random_text", "Non-numeric string");
        assertTest(idInvalid5 == -1, "Non-numeric string 'random_text' rejected");

        // Invalid simulated update time
        vector<Event> invalidEvents = manager.updateCurrentTime("99:99:99");
        assertTest(invalidEvents.empty(), "Invalid simulated update time '99:99:99' rejected without crash");
    }

    // ---------------------------------------------------------
    // TEST SUITE 2: Grace Period Configuration
    // ---------------------------------------------------------
    cout << "\n--- Suite 2: Grace Period Configuration ---" << endl;
    {
        CheckInManager defaultManager;
        assertTest(defaultManager.getGracePeriod() == 5, "Default grace period is 5 minutes");

        CheckInManager customManager(10);
        assertTest(customManager.getGracePeriod() == 10, "Custom constructor grace period is 10 minutes");

        customManager.setGracePeriod(15);
        assertTest(customManager.getGracePeriod() == 15, "setGracePeriod(15) correctly updates grace period");

        customManager.setGracePeriod(-5);
        assertTest(customManager.getGracePeriod() == 15, "Negative grace period rejected, previous value retained");
    }

    // ---------------------------------------------------------
    // TEST SUITE 3: Grace Period & First Missed Event ID (8000)
    // ---------------------------------------------------------
    cout << "\n--- Suite 3: Grace Period Behavior & First Event ID ---" << endl;
    {
        CheckInManager manager(5); // 5 min grace period
        manager.addCheckIn("10:00:00", "Morning check-in", "Main Quad");

        // Time before scheduled: 09:55:00
        vector<Event> events1 = manager.updateCurrentTime("09:55:00");
        assertTest(events1.empty() && manager.getPendingCount() == 1,
                   "Check-in remains PENDING before scheduled time (09:55:00)");

        // Time at scheduled time: 10:00:00
        vector<Event> events2 = manager.updateCurrentTime("10:00:00");
        assertTest(events2.empty() && manager.getPendingCount() == 1,
                   "Check-in remains PENDING at scheduled time (10:00:00)");

        // Time within grace period: 10:03:00 (deadline is 10:05:00)
        vector<Event> events3 = manager.updateCurrentTime("10:03:00");
        assertTest(events3.empty() && manager.getPendingCount() == 1,
                   "Check-in remains PENDING within grace period (10:03:00)");

        // Time exactly at grace deadline: 10:05:00
        vector<Event> events4 = manager.updateCurrentTime("10:05:00");
        assertTest(events4.empty() && manager.getPendingCount() == 1,
                   "Check-in remains PENDING at exact grace limit (10:05:00)");

        // Time past grace period: 10:05:01
        vector<Event> events5 = manager.updateCurrentTime("10:05:01");
        assertTest(events5.size() == 1, "Check-in becomes MISSED past grace period (10:05:01)");
        assertTest(events5[0].getEventID() == 8000, "First missed-check-in Event receives ID 8000");
        assertTest(events5[0].getEventType() == "Missed Check-in", "Event type is 'Missed Check-in'");
        assertTest(events5[0].getLocation() == "Main Quad", "Event location matches check-in location");
        assertTest(manager.getMissedCount() == 1, "Missed count is now 1");
    }

    // ---------------------------------------------------------
    // TEST SUITE 4: Multiple Overdue Events & Duplicate Prevention
    // ---------------------------------------------------------
    cout << "\n--- Suite 4: Multiple Overdue Events & Duplicate Prevention ---" << endl;
    {
        CheckInManager manager(5); // 5 min grace period
        manager.addCheckIn("11:00:00", "Checkpoint A", "Gate 1");
        manager.addCheckIn("11:10:00", "Checkpoint B", "Gate 2");

        // Current time jumps directly to 11:30:00 (both are overdue)
        vector<Event> missedEvents = manager.updateCurrentTime("11:30:00");
        assertTest(missedEvents.size() == 2, "Multiple overdue check-ins generate multiple events simultaneously (2 events)");

        assertTest(missedEvents[0].getEventID() == 8000, "First event in this manager receives ID 8000");
        assertTest(missedEvents[1].getEventID() == 8001, "Second event in this manager receives ID 8001");

        // Subsequent time update past the deadlines: 11:35:00
        vector<Event> duplicateCheck1 = manager.updateCurrentTime("11:35:00");
        assertTest(duplicateCheck1.empty(), "No duplicate events generated on subsequent time update (11:35:00)");

        // Another later time update: 12:00:00
        vector<Event> duplicateCheck2 = manager.updateCurrentTime("12:00:00");
        assertTest(duplicateCheck2.empty(), "No duplicate events generated on much later update (12:00:00)");
    }

    // ---------------------------------------------------------
    // TEST SUITE 5: On-Time Completion vs Late Confirmation
    // ---------------------------------------------------------
    cout << "\n--- Suite 5: On-Time Completion vs Late Confirmation ---" << endl;
    {
        CheckInManager manager(5);
        int onTimeID = manager.addCheckIn("13:00:00", "Lunch Check-in", "Dining Hall");
        int lateID   = manager.addCheckIn("13:15:00", "Post-Lunch Check-in", "Auditorium");

        // Complete the first check-in ON-TIME (simulated time 12:55:00)
        bool completedOnTime = manager.completeCheckIn(onTimeID);
        assertTest(completedOnTime, "Check-in completed successfully before deadline");

        // Advance time past first check-in deadline (13:10:00)
        vector<Event> events1 = manager.updateCurrentTime("13:10:00");
        assertTest(events1.empty(), "Completed check-in does not trigger missed event after deadline");

        // Advance time past second check-in deadline (13:25:00 -> deadline was 13:20:00)
        vector<Event> events2 = manager.updateCurrentTime("13:25:00");
        assertTest(events2.size() == 1, "Unconfirmed second check-in becomes missed");
        assertTest(manager.getMissedCount() == 1, "Missed count is 1 before late confirmation");

        // Now user performs LATE CONFIRMATION for the second check-in
        bool confirmedLate = manager.completeCheckIn(lateID);
        assertTest(confirmedLate, "completeCheckIn() succeeds for previously missed check-in");
        assertTest(manager.getLateConfirmationCount() == 1, "Late confirmation count is now 1");
        assertTest(manager.getMissedCount() == 0, "Unconfirmed missed count drops to 0 after late confirmation");
        assertTest(manager.getCompletedCount() == 1, "On-time completed count remains 1");
    }

    // ---------------------------------------------------------
    // TEST SUITE 6: Comprehensive Status Counts & Display
    // ---------------------------------------------------------
    cout << "\n--- Suite 6: Status Counts Consistency ---" << endl;
    {
        CheckInManager manager(5);
        // Create 4 check-ins for 4 distinct outcomes:
        int idPending = manager.addCheckIn("18:00:00", "Pending Item", "Loc 1");
        int idOnTime  = manager.addCheckIn("14:00:00", "On-Time Item", "Loc 2");
        int idLate    = manager.addCheckIn("15:00:00", "Late Item", "Loc 3");
        int idMissed  = manager.addCheckIn("16:00:00", "Missed Item", "Loc 4");

        // 1. Complete on-time item immediately
        manager.completeCheckIn(idOnTime);

        // 2. Advance time to 17:00:00 -> idLate (deadline 15:05) and idMissed (deadline 16:05) become missed
        manager.updateCurrentTime("17:00:00");

        // 3. Confirm idLate late
        manager.completeCheckIn(idLate);

        // idPending is scheduled for 18:00:00, so at 17:00:00 it is still PENDING
        assertTest(manager.getCheckInCount() == 4, "Total check-ins count is 4");
        assertTest(manager.getPendingCount() == 1, "Pending count is 1 (id: " + to_string(idPending) + ")");
        assertTest(manager.getCompletedCount() == 1, "On-time completed count is 1 (id: " + to_string(idOnTime) + ")");
        assertTest(manager.getLateConfirmationCount() == 1, "Late confirmation count is 1 (id: " + to_string(idLate) + ")");
        assertTest(manager.getMissedCount() == 1, "Unconfirmed missed count is 1 (id: " + to_string(idMissed) + ")");

        // Sum of parts equals total
        int sumParts = manager.getPendingCount() + manager.getCompletedCount() +
                       manager.getLateConfirmationCount() + manager.getMissedCount();
        assertTest(sumParts == manager.getCheckInCount(), "Sum of status counts (1+1+1+1=4) exactly equals total check-ins");

        // Visual display verification
        cout << "\nVisual display output of all 4 statuses:" << endl;
        manager.displayCheckIns();

        // Clear check-ins
        manager.clearCheckIns();
        assertTest(manager.getCheckInCount() == 0, "clearCheckIns() empties all records");
    }

    // ---------------------------------------------------------
    // SUMMARY
    // ---------------------------------------------------------
    cout << "\n====================================================" << endl;
    cout << "TEST RESULTS SUMMARY: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << endl;
    cout << "====================================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}

