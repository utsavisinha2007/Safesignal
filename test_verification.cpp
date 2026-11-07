#include <iostream>
#include <string>
#include <vector>
#include "VerificationManager.h"

using namespace std;

int testsPassed = 0;
int testsFailed = 0;

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
    cout << "    RUNNING UNIT TESTS FOR VERIFICATIONMANAGER      " << endl;
    cout << "====================================================\n" << endl;

    // ---------------------------------------------------------
    // TEST SUITE 1: Creating a Request & Unique IDs
    // ---------------------------------------------------------
    cout << "--- Suite 1: Creating Requests & Unique IDs ---" << endl;
    {
        VerificationManager vm;
        int id1 = vm.createVerificationRequest(65, "Concerning", "North Gate", "14:00:00");
        assertTest(id1 == 5000, "First request gets ID 5000");
        assertTest(vm.getTotalRequestCount() == 1, "Total request count is 1");
        assertTest(vm.getPendingCount() == 1, "Pending count is 1");
        assertTest(vm.hasActivePendingRequest(), "Has active pending request");

        VerificationRequest req;
        bool found = vm.getRequestByID(5000, req);
        assertTest(found, "Request #5000 found by ID");
        assertTest(req.riskScore == 65, "Risk score is 65");
        assertTest(req.riskLevel == "Concerning", "Risk level is 'Concerning'");
        assertTest(req.triggerLocation == "North Gate", "Location is 'North Gate'");
        assertTest(req.creationTimestamp == "14:00:00", "Creation timestamp is 14:00:00");
        assertTest(req.status == VSTAT_PENDING, "Status is PENDING");
    }

    // ---------------------------------------------------------
    // TEST SUITE 2: Default Timeout (3 minutes)
    // ---------------------------------------------------------
    cout << "\n--- Suite 2: Default Timeout (3 minutes) ---" << endl;
    {
        VerificationManager vm;
        assertTest(vm.getTimeoutMinutes() == 3, "Default timeout is 3 minutes");

        int id = vm.createVerificationRequest(55, "Concerning", "East Wing", "10:00:00");
        assertTest(id >= 0, "Request created successfully");

        VerificationRequest req;
        vm.getRequestByID(id, req);
        assertTest(req.deadlineTimestamp == "10:03:00", "Deadline is 10:03:00 (creation + 3 min)");

        // Advance time to 10:02:59 -> still pending
        vector<int> timedOut = vm.updateCurrentTime("10:02:59");
        assertTest(timedOut.empty(), "No timeout at 10:02:59 (1 second before deadline)");
        assertTest(vm.getPendingCount() == 1, "Still pending at 10:02:59");
    }

    // ---------------------------------------------------------
    // TEST SUITE 3: Custom Timeout
    // ---------------------------------------------------------
    cout << "\n--- Suite 3: Custom Timeout ---" << endl;
    {
        VerificationManager vm(5); // 5 minute timeout
        assertTest(vm.getTimeoutMinutes() == 5, "Custom timeout is 5 minutes");

        int id = vm.createVerificationRequest(70, "Concerning", "Library", "12:00:00");
        VerificationRequest req;
        vm.getRequestByID(id, req);
        assertTest(req.deadlineTimestamp == "12:05:00", "Deadline is 12:05:00 (creation + 5 min)");

        // Test setTimeoutMinutes
        vm.confirmSafe(id); // Close the request first
        vm.setTimeoutMinutes(10);
        assertTest(vm.getTimeoutMinutes() == 10, "Timeout updated to 10 minutes");

        int id2 = vm.createVerificationRequest(60, "Concerning", "Lab", "12:10:00");
        VerificationRequest req2;
        vm.getRequestByID(id2, req2);
        assertTest(req2.deadlineTimestamp == "12:20:00", "New deadline uses updated 10-min timeout");
    }

    // ---------------------------------------------------------
    // TEST SUITE 4: Confirm Safe
    // ---------------------------------------------------------
    cout << "\n--- Suite 4: Confirming Safety ---" << endl;
    {
        VerificationManager vm;
        int id = vm.createVerificationRequest(55, "Concerning", "Quad", "15:00:00");

        bool result = vm.confirmSafe(id);
        assertTest(result, "confirmSafe() returns true for PENDING request");

        VerificationRequest req;
        vm.getRequestByID(id, req);
        assertTest(req.status == VSTAT_CONFIRMED_SAFE, "Status changed to CONFIRMED_SAFE");
        assertTest(vm.getPendingCount() == 0, "Pending count dropped to 0");
        assertTest(vm.getConfirmedSafeCount() == 1, "Confirmed safe count is 1");
        assertTest(!vm.hasActivePendingRequest(), "No active pending request after confirmation");
        assertTest(vm.getTotalRequestCount() == 1, "Request preserved in history (not deleted)");
    }

    // ---------------------------------------------------------
    // TEST SUITE 5: Request Help
    // ---------------------------------------------------------
    cout << "\n--- Suite 5: Requesting Help ---" << endl;
    {
        VerificationManager vm;
        int id = vm.createVerificationRequest(80, "High Risk", "Parking Lot", "16:00:00");

        bool result = vm.requestHelp(id);
        assertTest(result, "requestHelp() returns true for PENDING request");

        VerificationRequest req;
        vm.getRequestByID(id, req);
        assertTest(req.status == VSTAT_HELP_REQUESTED, "Status changed to HELP_REQUESTED");
        assertTest(vm.getPendingCount() == 0, "Pending count dropped to 0");
        assertTest(vm.getHelpRequestedCount() == 1, "Help requested count is 1");
        assertTest(vm.getTotalRequestCount() == 1, "Request preserved in history");
    }

    // ---------------------------------------------------------
    // TEST SUITE 6: Timeout Before, At, and After Deadline
    // ---------------------------------------------------------
    cout << "\n--- Suite 6: Timeout Timing Precision ---" << endl;
    {
        // Before deadline: no timeout
        VerificationManager vm1;
        int id1 = vm1.createVerificationRequest(60, "Concerning", "Hall A", "09:00:00");
        vector<int> to1 = vm1.updateCurrentTime("09:02:59");
        assertTest(to1.empty(), "No timeout 1 second before deadline (09:02:59 < 09:03:00)");
        assertTest(vm1.getPendingCount() == 1, "Request still pending before deadline");
        (void)id1; // suppress unused warning

        // Exactly at deadline: timeout
        VerificationManager vm2;
        int id2 = vm2.createVerificationRequest(60, "Concerning", "Hall B", "09:00:00");
        vector<int> to2 = vm2.updateCurrentTime("09:03:00");
        assertTest(to2.size() == 1, "Timeout occurs exactly at deadline (09:03:00)");
        assertTest(to2[0] == id2, "Timed-out ID matches the request");
        assertTest(vm2.getPendingCount() == 0, "No pending requests after timeout");
        assertTest(vm2.getTimedOutCount() == 1, "Timed out count is 1");

        VerificationRequest req2;
        vm2.getRequestByID(id2, req2);
        assertTest(req2.status == VSTAT_TIMED_OUT, "Status is TIMED_OUT");
        assertTest(req2.resolutionTimestamp == "09:03:00", "Resolution timestamp is deadline time");

        // After deadline: timeout
        VerificationManager vm3;
        int id3 = vm3.createVerificationRequest(60, "Concerning", "Hall C", "09:00:00");
        vector<int> to3 = vm3.updateCurrentTime("09:10:00");
        assertTest(to3.size() == 1, "Timeout occurs after deadline (09:10:00 > 09:03:00)");
        assertTest(to3[0] == id3, "Timed-out ID matches");
        assertTest(vm3.getTimedOutCount() == 1, "Timed out count is 1");
    }

    // ---------------------------------------------------------
    // TEST SUITE 7: Duplicate Completion Rejection
    // ---------------------------------------------------------
    cout << "\n--- Suite 7: Duplicate Completion Rejection ---" << endl;
    {
        VerificationManager vm;
        int id = vm.createVerificationRequest(55, "Concerning", "Cafe", "11:00:00");

        // Confirm safe
        vm.confirmSafe(id);
        assertTest(vm.getConfirmedSafeCount() == 1, "Confirmed safe once");

        // Try to confirm again
        bool dupConfirm = vm.confirmSafe(id);
        assertTest(!dupConfirm, "Duplicate confirmSafe() rejected on CONFIRMED_SAFE request");
        assertTest(vm.getConfirmedSafeCount() == 1, "Confirmed safe count still 1");

        // Try to request help on confirmed request
        bool helpOnConfirmed = vm.requestHelp(id);
        assertTest(!helpOnConfirmed, "requestHelp() rejected on CONFIRMED_SAFE request");
        assertTest(vm.getHelpRequestedCount() == 0, "Help requested count remains 0");
    }
    {
        // Reject actions on timed-out request
        VerificationManager vm;
        int id = vm.createVerificationRequest(55, "Concerning", "Lab", "13:00:00");
        vm.updateCurrentTime("13:03:00"); // timeout

        bool confirmOnTimeout = vm.confirmSafe(id);
        assertTest(!confirmOnTimeout, "confirmSafe() rejected on TIMED_OUT request");

        bool helpOnTimeout = vm.requestHelp(id);
        assertTest(!helpOnTimeout, "requestHelp() rejected on TIMED_OUT request");
    }
    {
        // Reject actions on help-requested request
        VerificationManager vm;
        int id = vm.createVerificationRequest(80, "High Risk", "Lot B", "14:00:00");
        vm.requestHelp(id);

        bool dupHelp = vm.requestHelp(id);
        assertTest(!dupHelp, "Duplicate requestHelp() rejected on HELP_REQUESTED request");

        bool confirmOnHelp = vm.confirmSafe(id);
        assertTest(!confirmOnHelp, "confirmSafe() rejected on HELP_REQUESTED request");
    }

    // ---------------------------------------------------------
    // TEST SUITE 8: Invalid Request IDs
    // ---------------------------------------------------------
    cout << "\n--- Suite 8: Invalid Request IDs ---" << endl;
    {
        VerificationManager vm;

        bool r1 = vm.confirmSafe(9999);
        assertTest(!r1, "confirmSafe(9999) returns false for nonexistent ID");

        bool r2 = vm.requestHelp(9999);
        assertTest(!r2, "requestHelp(9999) returns false for nonexistent ID");

        VerificationRequest req;
        bool r3 = vm.getRequestByID(9999, req);
        assertTest(!r3, "getRequestByID(9999) returns false");

        assertTest(vm.getActiveRequestID() == -1, "getActiveRequestID() is -1 with no requests");
    }

    // ---------------------------------------------------------
    // TEST SUITE 9: Invalid Timestamps
    // ---------------------------------------------------------
    cout << "\n--- Suite 9: Invalid Timestamps ---" << endl;
    {
        VerificationManager vm;

        // Invalid creation timestamps
        int r1 = vm.createVerificationRequest(60, "Concerning", "X", "25:00:00");
        assertTest(r1 == -1, "Creation with invalid time '25:00:00' rejected");

        int r2 = vm.createVerificationRequest(60, "Concerning", "X", "abc");
        assertTest(r2 == -1, "Creation with non-numeric time 'abc' rejected");

        int r3 = vm.createVerificationRequest(60, "Concerning", "X", "");
        assertTest(r3 == -1, "Creation with empty timestamp rejected");

        int r4 = vm.createVerificationRequest(60, "Concerning", "X", "14:60:00");
        assertTest(r4 == -1, "Creation with invalid minutes '14:60:00' rejected");

        assertTest(vm.getTotalRequestCount() == 0, "No requests created from invalid timestamps");

        // Invalid updateCurrentTime
        vm.createVerificationRequest(60, "Concerning", "Y", "10:00:00");
        vector<int> to = vm.updateCurrentTime("99:99:99");
        assertTest(to.empty(), "updateCurrentTime with invalid '99:99:99' returns empty");
        assertTest(vm.getPendingCount() == 1, "Pending request preserved after invalid time update");
    }

    // ---------------------------------------------------------
    // TEST SUITE 10: Backward Time Rejection
    // ---------------------------------------------------------
    cout << "\n--- Suite 10: Backward Time Rejection ---" << endl;
    {
        VerificationManager vm;
        vm.createVerificationRequest(60, "Concerning", "Main", "14:00:00");

        // Advance to 14:01:00
        vm.updateCurrentTime("14:01:00");

        // Try backward jump to 13:59:00
        vector<int> to = vm.updateCurrentTime("13:59:00");
        assertTest(to.empty(), "Backward time jump from 14:01:00 to 13:59:00 rejected");
        assertTest(vm.getPendingCount() == 1, "Pending request preserved after backward time rejection");

        // Verify forward still works
        to = vm.updateCurrentTime("14:03:00");
        assertTest(to.size() == 1, "Forward time to 14:03:00 (deadline) succeeds and triggers timeout");

        // Backward creation timestamp
        VerificationManager vm2;
        vm2.createVerificationRequest(60, "Concerning", "A", "15:00:00");
        vm2.confirmSafe(5000); // close it
        int r = vm2.createVerificationRequest(60, "Concerning", "B", "14:50:00");
        assertTest(r == -1, "Creation with backward timestamp (14:50:00 < 15:00:00) rejected");
    }

    // ---------------------------------------------------------
    // TEST SUITE 11: Duplicate Active Request Prevention
    // ---------------------------------------------------------
    cout << "\n--- Suite 11: Duplicate Active Request Prevention ---" << endl;
    {
        VerificationManager vm;
        int id1 = vm.createVerificationRequest(55, "Concerning", "Plaza", "10:00:00");
        assertTest(id1 == 5000, "First request created with ID 5000");

        // Try to create another while first is pending
        int id2 = vm.createVerificationRequest(70, "Concerning", "Park", "10:01:00");
        assertTest(id2 == -1, "Duplicate request rejected while first is still pending");
        assertTest(vm.getTotalRequestCount() == 1, "Only 1 request in system");

        // Close the first, then create another
        vm.confirmSafe(id1);
        int id3 = vm.createVerificationRequest(65, "Concerning", "Arena", "10:02:00");
        assertTest(id3 == 5001, "New request created after previous was resolved (ID 5001)");
        assertTest(vm.getTotalRequestCount() == 2, "Total requests now 2");
    }

    // ---------------------------------------------------------
    // TEST SUITE 12: Statistics & History Preservation
    // ---------------------------------------------------------
    cout << "\n--- Suite 12: Statistics & History Preservation ---" << endl;
    {
        VerificationManager vm(2); // 2-minute timeout

        // Request 1: confirmed safe
        int id1 = vm.createVerificationRequest(55, "Concerning", "Dorm", "08:00:00");
        vm.confirmSafe(id1);

        // Request 2: help requested
        int id2 = vm.createVerificationRequest(80, "High Risk", "Road", "08:05:00");
        vm.requestHelp(id2);

        // Request 3: timed out
        int id3 = vm.createVerificationRequest(60, "Concerning", "Bridge", "08:10:00");
        vm.updateCurrentTime("08:12:00");

        assertTest(vm.getTotalRequestCount() == 3, "Total request count is 3");
        assertTest(vm.getPendingCount() == 0, "No pending requests");
        assertTest(vm.getConfirmedSafeCount() == 1, "1 confirmed safe");
        assertTest(vm.getHelpRequestedCount() == 1, "1 help requested");
        assertTest(vm.getTimedOutCount() == 1, "1 timed out");

        // All 3 requests still exist in history
        VerificationRequest r1, r2, r3;
        assertTest(vm.getRequestByID(id1, r1), "Request 1 still in history");
        assertTest(vm.getRequestByID(id2, r2), "Request 2 still in history");
        assertTest(vm.getRequestByID(id3, r3), "Request 3 still in history");
        assertTest(r1.status == VSTAT_CONFIRMED_SAFE, "Request 1 status preserved as CONFIRMED_SAFE");
        assertTest(r2.status == VSTAT_HELP_REQUESTED, "Request 2 status preserved as HELP_REQUESTED");
        assertTest(r3.status == VSTAT_TIMED_OUT, "Request 3 status preserved as TIMED_OUT");

        // Display for visual verification
        cout << "\nVisual display output:" << endl;
        vm.displayVerificationStatistics();
        vm.displayRequestHistory();
    }

    // ---------------------------------------------------------
    // TEST SUITE 13: HH:MM Format Support
    // ---------------------------------------------------------
    cout << "\n--- Suite 13: HH:MM Format Support ---" << endl;
    {
        VerificationManager vm(5);
        int id = vm.createVerificationRequest(60, "Concerning", "Gate", "14:30");
        assertTest(id >= 0, "Request created with HH:MM format '14:30'");

        VerificationRequest req;
        vm.getRequestByID(id, req);
        assertTest(req.deadlineTimestamp == "14:35:00", "Deadline correctly computed from HH:MM input");

        vm.updateCurrentTime("14:34");
        assertTest(vm.getPendingCount() == 1, "Still pending at 14:34 (HH:MM format time update)");

        vector<int> to = vm.updateCurrentTime("14:35");
        assertTest(to.size() == 1, "Timeout at 14:35 (HH:MM format)");
    }

    // ---------------------------------------------------------
    // TEST SUITE 14: Multiple Sequential Requests
    // ---------------------------------------------------------
    cout << "\n--- Suite 14: Multiple Sequential Requests ---" << endl;
    {
        VerificationManager vm(2);

        // Create, timeout, create, confirm, create, help
        int id1 = vm.createVerificationRequest(55, "Concerning", "A", "10:00:00");
        vm.updateCurrentTime("10:02:00"); // timeout
        assertTest(vm.getTimedOutCount() == 1, "First request timed out");

        int id2 = vm.createVerificationRequest(60, "Concerning", "B", "10:03:00");
        vm.confirmSafe(id2);
        assertTest(vm.getConfirmedSafeCount() == 1, "Second request confirmed safe");

        int id3 = vm.createVerificationRequest(80, "High Risk", "C", "10:05:00");
        vm.requestHelp(id3);
        assertTest(vm.getHelpRequestedCount() == 1, "Third request help requested");

        assertTest(vm.getTotalRequestCount() == 3, "All 3 requests preserved in history");
        assertTest(id1 == 5000 && id2 == 5001 && id3 == 5002, "Sequential IDs: 5000, 5001, 5002");
    }

    // ---------------------------------------------------------
    // SUMMARY
    // ---------------------------------------------------------
    cout << "\n====================================================" << endl;
    cout << "VERIFICATION TEST SUMMARY: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << endl;
    cout << "====================================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}

