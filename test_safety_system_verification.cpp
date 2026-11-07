#include <iostream>
#include <string>
#include <vector>
#include "SafetySystem.h"

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
    cout << "==========================================================" << endl;
    cout << " RUNNING INTEGRATION TESTS FOR SAFETYSYSTEM VERIFICATION  " << endl;
    cout << "==========================================================\n" << endl;

    // ---------------------------------------------------------
    // TEST 1: Risk >= 50 Triggers Verification, NOT Immediate Escalation
    // ---------------------------------------------------------
    cout << "--- Test 1: Risk >= 50 Triggers Verification, NOT Immediate Escalation ---" << endl;
    {
        SafetySystem system;

        // Ingest events that elevate risk >= 50
        // Route Deviation (20) + Missed Check-in (20) + Compound Pattern (25) = Score 65 (Concerning)
        system.logEvent(Event(101, "Route Deviation", "North Path", "14:00:00"));
        assertTest(!system.hasActiveVerification(), "Score is 20: No verification requested yet");

        system.logEvent(Event(102, "Missed Check-in", "Gate 1", "14:04:00"));
        assertTest(system.hasActiveVerification(), "Score is 65: Active verification request created!");
        assertTest(system.getActiveVerificationID() == 5000, "Verification request assigned ID 5000");

        // Verify that NO emergency incident was dispatched or queued in responseManager
        VerificationRequest vreq;
        bool found = system.getVerificationManager().getRequestByID(5000, vreq);
        assertTest(found, "Verification request #5000 found in VerificationManager");
        assertTest(vreq.status == VSTAT_PENDING, "Verification request status is PENDING");
        assertTest(vreq.riskScore == 65, "Verification request captured risk score 65");
        assertTest(vreq.riskLevel == "Concerning", "Verification request captured risk level 'Concerning'");
    }

    // ---------------------------------------------------------
    // TEST 2: Confirming Safety Averts Escalation & Resets Latch
    // ---------------------------------------------------------
    cout << "\n--- Test 2: Confirming Safety ---" << endl;
    {
        SafetySystem system;
        system.logEvent(Event(201, "Route Deviation", "Path A", "10:00:00"));
        system.logEvent(Event(202, "Missed Check-in", "Path B", "10:04:00"));
        assertTest(system.hasActiveVerification(), "Active verification request exists");

        // User confirms safe
        bool confirmed = system.confirmUserSafe();
        assertTest(confirmed, "confirmUserSafe() succeeds for active request");
        assertTest(!system.hasActiveVerification(), "No active verification after user confirms safe");

        VerificationRequest req;
        system.getVerificationManager().getRequestByID(5000, req);
        assertTest(req.status == VSTAT_CONFIRMED_SAFE, "Request status updated to CONFIRMED_SAFE");
        assertTest(system.getVerificationManager().getConfirmedSafeCount() == 1, "Confirmed safe count is 1");

        // Latch is reset: A new independent risk episode can trigger a new verification request
        // First simulate time advancing past the sliding window so previous events expire (10:15:00)
        system.processCurrentTimeUpdate("10:15:00");

        // Now trigger a new high-risk episode
        system.logEvent(Event(203, "SOS", "Library", "10:16:00"));
        system.logEvent(Event(204, "Fall Detected", "Library", "10:17:00"));
        assertTest(system.hasActiveVerification(), "New verification request triggered for subsequent episode");
        assertTest(system.getActiveVerificationID() == 5001, "New request assigned unique ID 5001");
    }

    // ---------------------------------------------------------
    // TEST 3: Requesting Immediate Help
    // ---------------------------------------------------------
    cout << "\n--- Test 3: Requesting Immediate Help ---" << endl;
    {
        SafetySystem system;
        system.logEvent(Event(301, "Route Deviation", "Sector 3", "11:00:00"));
        system.logEvent(Event(302, "Missed Check-in", "Sector 3", "11:04:00"));
        assertTest(system.hasActiveVerification(), "Active verification request exists");

        bool helpReq = system.requestImmediateHelp();
        assertTest(helpReq, "requestImmediateHelp() succeeds");
        assertTest(!system.hasActiveVerification(), "Request is no longer PENDING");

        VerificationRequest req;
        system.getVerificationManager().getRequestByID(5000, req);
        assertTest(req.status == VSTAT_HELP_REQUESTED, "Status is HELP_REQUESTED (preserved for Phase 4.4)");
        assertTest(system.getVerificationManager().getHelpRequestedCount() == 1, "Help requested count is 1");
    }

    // ---------------------------------------------------------
    // TEST 4: Timeout Behavior via processCurrentTimeUpdate
    // ---------------------------------------------------------
    cout << "\n--- Test 4: Verification Timeout via Simulated Time Updates ---" << endl;
    {
        SafetySystem system;
        // Create verification request at 12:00:00 (default 3-minute timeout -> deadline 12:03:00)
        system.logEvent(Event(401, "Route Deviation", "Hall 1", "12:00:00"));
        system.logEvent(Event(402, "Missed Check-in", "Hall 1", "12:04:00"));
        assertTest(system.hasActiveVerification(), "Verification request created at 12:04:00");
        // Deadline is 12:04:00 + 3 min = 12:07:00

        // Advance to 12:06:59 -> still pending
        system.processCurrentTimeUpdate("12:06:59");
        assertTest(system.hasActiveVerification(), "Still pending at 12:06:59 (before deadline)");

        // Advance to 12:07:00 -> timed out!
        system.processCurrentTimeUpdate("12:07:00");
        assertTest(!system.hasActiveVerification(), "No active verification after reaching deadline 12:07:00");

        VerificationRequest req;
        system.getVerificationManager().getRequestByID(5000, req);
        assertTest(req.status == VSTAT_TIMED_OUT, "Request status is TIMED_OUT (preserved for Phase 4.4)");
        assertTest(system.getVerificationManager().getTimedOutCount() == 1, "Timed out count is 1");
    }

    // ---------------------------------------------------------
    // TEST 5: Duplicate Prevention in Same Risk Episode
    // ---------------------------------------------------------
    cout << "\n--- Test 5: Duplicate Verification Prevention ---" << endl;
    {
        SafetySystem system;
        // First high-risk trigger
        system.logEvent(Event(501, "Route Deviation", "Gate A", "15:00:00"));
        system.logEvent(Event(502, "Missed Check-in", "Gate A", "15:04:00"));
        assertTest(system.getVerificationManager().getTotalRequestCount() == 1, "One verification request created");

        // Another event arrives during the same high-risk episode
        system.logEvent(Event(503, "Motion Alert", "Gate A", "15:05:00"));
        assertTest(system.getVerificationManager().getTotalRequestCount() == 1, "No duplicate verification request created during active episode");
        assertTest(system.getVerificationManager().getPendingCount() == 1, "Still 1 pending request");
    }

    // ---------------------------------------------------------
    // TEST 6: Journey & Check-in Integration with Verification
    // ---------------------------------------------------------
    cout << "\n--- Test 6: Journey and Scheduled Check-in Integration ---" << endl;
    {
        SafetySystem system;

        // 1. Start a planned journey
        vector<string> route = { "Checkpoint 1", "Checkpoint 2", "Checkpoint 3" };
        bool journeyStarted = system.startJourney("Start", "Destination", "16:00:00", route);
        assertTest(journeyStarted, "Planned journey started successfully");

        // 2. Schedule a safety check-in during the journey
        int checkInID = system.scheduleCheckIn("15:10:00", "Mid-journey check-in", "Checkpoint 1");
        assertTest(checkInID > 0, "Check-in scheduled during active journey");

        // 3. Submit simulated location update that triggers a route deviation at 15:08:00
        system.updateJourneyLocation("Unexpected Dark Alley", "15:08:00");
        // Deviation event generated and logged, score is 20 (Unusual, < 50)
        assertTest(!system.hasActiveVerification(), "Score is 20: No verification requested yet");

        // 4. Time advances to 15:15:01 (past 15:10:00 + 5 min grace = 15:15:00 deadline)
        // Elapsed from 15:08:00 to 15:15:01 is ~7 minutes (well inside 10m sliding window)
        // Missed check-in detected! Missed event logged at 15:15:01
        // Combined: Route Deviation + Missed Check-in = Score 65 (Concerning) -> triggers Verification!
        system.processCurrentTimeUpdate("15:15:01");
        assertTest(system.hasActiveVerification(), "Check-in miss elevated risk to 65 -> Verification request created!");
        assertTest(system.getActiveVerificationID() == 5000, "Verification request ID is 5000");

        // 5. User confirms safe
        assertTest(system.confirmUserSafe(), "User confirms safe");
        assertTest(!system.hasActiveVerification(), "Verification cleared");

        // 6. Journey ends cleanly
        system.endJourney();
    }

    // ---------------------------------------------------------
    // TEST 7: Regression - Confirming Safety Followed by Another Event while Risk >= 50
    // ---------------------------------------------------------
    cout << "\n--- Test 7: Regression - Event Logged After Confirming Safety While Risk >= 50 ---" << endl;
    {
        SafetySystem system;

        // 1. Ingest events triggering risk score 65 (Concerning)
        system.logEvent(Event(701, "Route Deviation", "Sector 9", "14:00:00"));
        system.logEvent(Event(702, "Missed Check-in", "Sector 9", "14:04:00"));
        assertTest(system.hasActiveVerification(), "Verification request #5000 triggered at risk 65");
        assertTest(system.getVerificationManager().getTotalRequestCount() == 1, "Total verification requests is 1");

        // 2. User confirms safe
        bool safeOk = system.confirmUserSafe();
        assertTest(safeOk, "User confirmed safe for request #5000");
        assertTest(!system.hasActiveVerification(), "No pending verification active after confirmation");

        VerificationRequest req1;
        system.getVerificationManager().getRequestByID(5000, req1);
        assertTest(req1.status == VSTAT_CONFIRMED_SAFE, "Request #5000 status is CONFIRMED_SAFE");

        // 3. Log another event at 14:05:00 while the old events are STILL within the 10m sliding window
        // Underlying risk score remains >= 50!
        system.logEvent(Event(703, "Motion Alert", "Sector 9", "14:05:00"));

        // Critical assertions:
        // Must NOT create a duplicate verification request for this same unresolved episode!
        assertTest(!system.hasActiveVerification(), "No active verification reopened by subsequent event in same episode");
        assertTest(system.getVerificationManager().getTotalRequestCount() == 1, "Total verification request count strictly remains 1");
        assertTest(system.getVerificationManager().getConfirmedSafeCount() == 1, "Confirmed safe count remains 1");

        // 4. Log yet another event at 14:06:00
        system.logEvent(Event(704, "Noise Spike", "Sector 9", "14:06:00"));
        assertTest(!system.hasActiveVerification(), "Still no duplicate verification reopened");
        assertTest(system.getVerificationManager().getTotalRequestCount() == 1, "Total verification request count still 1");

        // 5. Advance time past the 10-minute window (14:18:00) so old events expire and risk normalizes below 50
        system.processCurrentTimeUpdate("14:18:00");
        assertTest(!system.hasActiveVerification(), "No verification pending after risk normalization");

        // 6. Now trigger a genuinely new risk episode at 14:20:00 and 14:24:00
        system.logEvent(Event(705, "Route Deviation", "Sector 10", "14:20:00"));
        system.logEvent(Event(706, "Missed Check-in", "Sector 10", "14:24:00"));

        // Genuinely new episode must trigger a new verification request!
        assertTest(system.hasActiveVerification(), "Genuinely new risk episode successfully triggers new verification request");
        assertTest(system.getActiveVerificationID() == 5001, "New verification request assigned ID 5001");
        assertTest(system.getVerificationManager().getTotalRequestCount() == 2, "Total verification requests is now 2");
    }

    // ---------------------------------------------------------
    // SUMMARY
    // ---------------------------------------------------------
    cout << "\n==========================================================" << endl;
    cout << "SAFETYSYSTEM VERIFICATION SUMMARY: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << endl;
    cout << "==========================================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}
