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
    cout << " RUNNING UNIT & INTEGRATION TESTS FOR PHASES 4.4 & 4.5    " << endl;
    cout << " (Priority-Based Dispatch & Incident Lifecycle Management)" << endl;
    cout << "==========================================================\n" << endl;

    // ---------------------------------------------------------
    // TEST 1: HELP_REQUESTED Creates Exactly One Incident
    // ---------------------------------------------------------
    cout << "--- Test 1: HELP_REQUESTED Creates Exactly One Incident ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        // Ingest events to elevate risk >= 50
        system.logEvent(Event(101, "Route Deviation", "North Gate", "10:00:00"));
        system.logEvent(Event(102, "Missed Check-in", "North Gate", "10:04:00"));
        assertTest(system.hasActiveVerification(), "Verification request #5000 created");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 0, "No emergency incidents before help requested");

        // User requests immediate help
        bool helpOk = system.requestImmediateHelp();
        assertTest(helpOk, "requestImmediateHelp() succeeds");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 1, "Exactly one emergency incident created upon help request");

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getIncidentID() == 9001, "Incident ID is 9001");
        assertTest(inc.getVerificationRequestID() == 5000, "Incident preserves verification request ID 5000");
        assertTest(inc.getSeverity() == 5, "Emergency incident severity is 5 (Critical)");
        assertTest(inc.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Incident status is AWAITING_ACKNOWLEDGEMENT");

        // Calling requestImmediateHelp again does not create a duplicate incident
        bool dupHelp = system.requestImmediateHelp(5000);
        assertTest(!dupHelp, "Duplicate requestImmediateHelp() rejected");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 1, "Total incident count strictly remains 1");
    }

    // ---------------------------------------------------------
    // TEST 2: TIMED_OUT Creates Exactly One Incident
    // ---------------------------------------------------------
    cout << "\n--- Test 2: TIMED_OUT Creates Exactly One Incident ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        system.logEvent(Event(201, "Route Deviation", "East Wing", "11:00:00"));
        system.logEvent(Event(202, "Missed Check-in", "East Wing", "11:04:00"));
        assertTest(system.hasActiveVerification(), "Verification request created at 11:04:00 (deadline 11:07:00)");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 0, "No incidents before timeout");

        // Advance to 11:07:00 -> times out!
        system.processCurrentTimeUpdate("11:07:00");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 1, "Exactly one incident created upon verification timeout");

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getVerificationRequestID() == 5000, "Incident preserves verification request ID 5000");
        assertTest(inc.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Incident status is AWAITING_ACKNOWLEDGEMENT");

        // Repeating time update at 11:07:00 or later does NOT create a duplicate incident
        system.processCurrentTimeUpdate("11:07:00");
        system.processCurrentTimeUpdate("11:08:00");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 1, "No duplicate incidents on subsequent time updates");
    }

    // ---------------------------------------------------------
    // TEST 3: CONFIRMED_SAFE Creates NO Incident
    // ---------------------------------------------------------
    cout << "\n--- Test 3: CONFIRMED_SAFE Creates No Incident ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        system.logEvent(Event(301, "Route Deviation", "South Path", "12:00:00"));
        system.logEvent(Event(302, "Missed Check-in", "South Path", "12:04:00"));
        assertTest(system.hasActiveVerification(), "Verification request created at 12:04:00");

        // User confirms safety
        bool safeOk = system.confirmUserSafe();
        assertTest(safeOk, "User confirmed safe");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 0, "CONFIRMED_SAFE creates NO emergency incident");

        // Advance time past the original verification deadline
        system.processCurrentTimeUpdate("12:10:00");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 0, "No incident created even after original deadline passes");
    }

    // ---------------------------------------------------------
    // TEST 4: Priority 1 Contacts Notified First
    // ---------------------------------------------------------
    cout << "\n--- Test 4: Priority 1 Contacts Notified First ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice P1", "555-0101", "Parent", 1));
        system.registerContact(Contact(102, "Bob P1", "555-0102", "Spouse", 1));
        system.registerContact(Contact(201, "Charlie P2", "555-0201", "Friend", 2));
        system.registerContact(Contact(301, "Campus Security P3", "555-0301", "Security", 3));

        system.logEvent(Event(401, "SOS", "Quad", "13:00:00"));
        system.logEvent(Event(402, "Fall Detected", "Quad", "13:01:00"));
        system.requestImmediateHelp();

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getCurrentTier() == 1, "Current tier is Priority 1");
        assertTest(inc.getNotificationCount() == 2, "Exactly 2 notifications dispatched (both Priority 1)");

        // Verify that only Priority 1 contacts were notified
        assertTest(inc.isContactNotified(101), "Alice (P1) was notified");
        assertTest(inc.isContactNotified(102), "Bob (P1) was notified");
        assertTest(!inc.isContactNotified(201), "Charlie (P2) was NOT notified initially");
        assertTest(!inc.isContactNotified(301), "Campus Security (P3) was NOT notified initially");
    }

    // ---------------------------------------------------------
    // TEST 5 & 6: Escalation to Tier 2 & Tier 3 on Timeout
    // ---------------------------------------------------------
    cout << "\n--- Test 5 & 6: Priority Tier Escalation on Timeout ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "P1 Contact", "555-0101", "Family", 1));
        system.registerContact(Contact(201, "P2 Contact", "555-0201", "Friend", 2));
        system.registerContact(Contact(301, "P3 Contact", "555-0301", "Security", 3));

        system.logEvent(Event(501, "Route Deviation", "Hall", "14:00:00"));
        system.logEvent(Event(502, "Missed Check-in", "Hall", "14:04:00"));
        system.requestImmediateHelp(); // Dispatch at 14:04:00 (2 min timeout -> Tier 1 deadline 14:06:00)

        // At 14:05:59: still Tier 1
        system.processCurrentTimeUpdate("14:05:59");
        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getCurrentTier() == 1, "Still at Tier 1 before timeout (14:05:59)");
        assertTest(inc.getNotificationCount() == 1, "Only 1 notification sent so far");

        // At 14:06:00: Tier 1 times out -> Escalates to Tier 2!
        system.processCurrentTimeUpdate("14:06:00");
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getCurrentTier() == 2, "Escalated to Tier 2 at 14:06:00");
        assertTest(inc.getNotificationCount() == 2, "Total 2 notifications (P1 + P2)");
        assertTest(inc.isContactNotified(201), "P2 Contact was notified upon Tier 1 timeout");
        assertTest(!inc.isContactNotified(301), "P3 Contact still NOT notified");

        // At 14:08:00: Tier 2 times out -> Escalates to Tier 3!
        system.processCurrentTimeUpdate("14:08:00");
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getCurrentTier() == 3, "Escalated to Tier 3 at 14:08:00");
        assertTest(inc.getNotificationCount() == 3, "Total 3 notifications (P1 + P2 + P3)");
        assertTest(inc.isContactNotified(301), "P3 Contact was notified upon Tier 2 timeout");
    }

    // ---------------------------------------------------------
    // TEST 7: Skipping Empty Tiers
    // ---------------------------------------------------------
    cout << "\n--- Test 7: Skipping Empty Priority Tiers ---" << endl;
    {
        SafetySystem system;
        // Register only Priority 2 and Priority 3 contacts (no Priority 1)
        system.registerContact(Contact(201, "P2 Contact", "555-0201", "Friend", 2));
        system.registerContact(Contact(301, "P3 Contact", "555-0301", "Security", 3));

        system.logEvent(Event(601, "Route Deviation", "Alley", "15:00:00"));
        system.logEvent(Event(602, "Missed Check-in", "Alley", "15:04:00"));
        system.requestImmediateHelp(); // Dispatches at 15:04:00

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getCurrentTier() == 2, "Tier 1 was empty -> automatically started at Tier 2");
        assertTest(inc.isContactNotified(201), "Priority 2 contact immediately notified");
        assertTest(!inc.isContactNotified(301), "Priority 3 contact not yet notified");
    }

    // ---------------------------------------------------------
    // TEST 8 & 9: Acknowledgement Acceptance vs Rejection
    // ---------------------------------------------------------
    cout << "\n--- Test 8 & 9: Acknowledgement Validation ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice P1", "555-0101", "Family", 1));
        system.registerContact(Contact(201, "Charlie P2", "555-0201", "Friend", 2));

        system.logEvent(Event(701, "Route Deviation", "Lot C", "16:00:00"));
        system.logEvent(Event(702, "Missed Check-in", "Lot C", "16:04:00"));
        system.requestImmediateHelp(); // Notifies Alice (P1), Charlie (P2) is unnotified

        // Charlie (P2, unnotified) attempts to acknowledge -> REJECTED!
        bool unnotifiedAck = system.acknowledgeIncident(9001, 201, "16:05:00");
        assertTest(!unnotifiedAck, "Acknowledgement from unnotified contact Charlie (P2) strictly rejected");

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Status remains AWAITING_ACKNOWLEDGEMENT");

        // Alice (P1, notified) acknowledges -> ACCEPTED!
        bool notifiedAck = system.acknowledgeIncident(9001, 101, "16:05:00");
        assertTest(notifiedAck, "Acknowledgement from notified contact Alice (P1) accepted");

        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_ACKNOWLEDGED, "Status updated to ACKNOWLEDGED");
        assertTest(inc.getAcknowledgedByContactID() == 101, "Acknowledged contact ID recorded as 101");
        assertTest(inc.getAcknowledgedByName() == "Alice P1", "Acknowledged contact name recorded");

        // Duplicate acknowledgement rejected
        bool dupAck = system.acknowledgeIncident(9001, 101, "16:06:00");
        assertTest(!dupAck, "Duplicate acknowledgement rejected");
    }

    // ---------------------------------------------------------
    // TEST 10: Acknowledgement Stops Further Escalation
    // ---------------------------------------------------------
    cout << "\n--- Test 10: Acknowledgement Stops Further Escalation ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice P1", "555-0101", "Family", 1));
        system.registerContact(Contact(201, "Charlie P2", "555-0201", "Friend", 2));

        system.logEvent(Event(801, "Route Deviation", "Plaza", "17:00:00"));
        system.logEvent(Event(802, "Missed Check-in", "Plaza", "17:04:00"));
        system.requestImmediateHelp(); // Alice notified at 17:04:00 (Tier 1 deadline 17:06:00)

        // Alice acknowledges at 17:05:00
        system.acknowledgeIncident(9001, 101, "17:05:00");

        // Time advances past deadline to 17:10:00
        system.processCurrentTimeUpdate("17:10:00");

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_ACKNOWLEDGED, "Status remains ACKNOWLEDGED (did not escalate)");
        assertTest(!inc.isContactNotified(201), "Charlie (P2) was NEVER notified because escalation was stopped");
        assertTest(inc.getNotificationCount() == 1, "Only 1 notification sent throughout lifecycle");
    }

    // ---------------------------------------------------------
    // TEST 11 & 12: Incident Resolution and Cancellation
    // ---------------------------------------------------------
    cout << "\n--- Test 11 & 12: Resolution and Cancellation ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        system.logEvent(Event(901, "Route Deviation", "Field", "18:00:00"));
        system.logEvent(Event(902, "Missed Check-in", "Field", "18:04:00"));
        system.requestImmediateHelp();

        // Resolve incident
        bool resOk = system.resolveIncident(9001, "User safely escorted by patrol", "18:15:00");
        assertTest(resOk, "resolveIncident() succeeds");

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_RESOLVED, "Status updated to RESOLVED");
        assertTest(inc.getResolutionNotes() == "User safely escorted by patrol", "Resolution note preserved");
        assertTest(!inc.isActive(), "Incident is no longer active");

        // Duplicate resolution rejected
        bool dupRes = system.resolveIncident(9001, "Second resolution attempt", "18:16:00");
        assertTest(!dupRes, "Duplicate resolveIncident() rejected");

        // Cancellation on resolved incident rejected
        bool cancelOnRes = system.cancelIncident(9001, "Cancel resolved", "18:17:00");
        assertTest(!cancelOnRes, "cancelIncident() rejected on resolved incident");

        // Acknowledgement on resolved incident rejected
        bool ackOnRes = system.acknowledgeIncident(9001, 101, "18:18:00");
        assertTest(!ackOnRes, "acknowledgeIncident() rejected on resolved incident");

        // Test Cancellation on a separate incident
        SafetySystem system2;
        system2.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        system2.logEvent(Event(903, "Route Deviation", "Dorm", "19:00:00"));
        system2.logEvent(Event(904, "Missed Check-in", "Dorm", "19:04:00"));
        system2.requestImmediateHelp();

        bool cancelOk = system2.cancelIncident(9001, "Accidental button press", "19:05:00");
        assertTest(cancelOk, "cancelIncident() succeeds on active incident");

        Incident inc2;
        system2.getResponseManager().getIncidentByID(9001, inc2);
        assertTest(inc2.getStatus() == INC_CANCELLED, "Status updated to CANCELLED");
        assertTest(inc2.getCancellationReason() == "Accidental button press", "Cancellation reason preserved");

        // Duplicate cancellation rejected
        bool dupCancel = system2.cancelIncident(9001, "Try cancel again", "19:06:00");
        assertTest(!dupCancel, "Duplicate cancelIncident() rejected");

        // Resolution on cancelled incident rejected
        bool resOnCancel = system2.resolveIncident(9001, "Try resolve cancelled", "19:07:00");
        assertTest(!resOnCancel, "resolveIncident() rejected on cancelled incident");
    }

    // ---------------------------------------------------------
    // TEST 13: Escalation Exhaustion When Nobody Acknowledges
    // ---------------------------------------------------------
    cout << "\n--- Test 13: Escalation Exhaustion ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "P1 Contact", "555-0101", "Family", 1));
        system.registerContact(Contact(201, "P2 Contact", "555-0201", "Friend", 2));
        system.registerContact(Contact(301, "P3 Contact", "555-0301", "Security", 3));

        system.logEvent(Event(1001, "Route Deviation", "Lab", "20:00:00"));
        system.logEvent(Event(1002, "Missed Check-in", "Lab", "20:04:00"));
        system.requestImmediateHelp(); // 20:04:00 (T1: 20:06, T2: 20:08, T3: 20:10)

        // Advance through all 3 tier timeouts without any contact acknowledging
        system.processCurrentTimeUpdate("20:06:00"); // T1 timeout -> T2
        system.processCurrentTimeUpdate("20:08:00"); // T2 timeout -> T3
        system.processCurrentTimeUpdate("20:10:00"); // T3 timeout -> EXHAUSTED!

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_ESCALATION_EXHAUSTED, "Status is ESCALATION_EXHAUSTED after all tiers time out");
        assertTest(inc.getNotificationCount() == 3, "All 3 tiers were notified before exhaustion");
        assertTest(system.getResponseManager().getEscalationExhaustedCount() == 1, "Escalation exhausted count is 1");
    }

    // ---------------------------------------------------------
    // TEST 14: Notification History Preservation
    // ---------------------------------------------------------
    cout << "\n--- Test 14: Notification History Preservation ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Mom", 1));
        system.registerContact(Contact(201, "Bob", "555-0201", "Coworker", 2));

        system.logEvent(Event(1101, "Route Deviation", "Center", "21:00:00"));
        system.logEvent(Event(1102, "Missed Check-in", "Center", "21:04:00"));
        system.requestImmediateHelp(); // Notifies Alice at 21:04:00

        system.processCurrentTimeUpdate("21:06:00"); // Escalates to Bob at 21:06:00
        system.acknowledgeIncident(9001, 201, "21:07:00"); // Bob acknowledges
        system.resolveIncident(9001, "Resolved safely", "21:10:00"); // Resolved

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_RESOLVED, "Incident is RESOLVED");
        assertTest(inc.getNotificationCount() == 2, "Notification history preserved after resolution (count = 2)");

        const auto& history = inc.getNotificationHistory();
        assertTest(history[0].contactID == 101 && history[0].priorityLevel == 1, "Record 1 is Alice (P1)");
        assertTest(!history[0].acknowledged, "Record 1 was not acknowledged");
        assertTest(history[1].contactID == 201 && history[1].priorityLevel == 2, "Record 2 is Bob (P2)");
        assertTest(history[1].acknowledged, "Record 2 was acknowledged");
        assertTest(history[1].acknowledgementTimestamp == "21:07:00", "Record 2 acknowledgement timestamp preserved");
    }

    // ---------------------------------------------------------
    // TEST 15: Multiple Incidents Processed Independently
    // ---------------------------------------------------------
    cout << "\n--- Test 15: Multiple Incidents Processed Independently ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        // Create Incident 1 via manual report
        Incident manInc(1001, "Manual incident in hallway", "Hallway", 3);
        system.reportIncident(manInc);

        // Create Incident 2 via verification timeout
        system.logEvent(Event(1201, "Route Deviation", "North", "22:00:00"));
        system.logEvent(Event(1202, "Missed Check-in", "North", "22:04:00"));
        system.processCurrentTimeUpdate("22:07:00"); // Times out -> auto incident 9001

        assertTest(system.getResponseManager().getTotalIncidentCount() == 2, "Total incident count is 2");

        // Resolve Incident 1
        system.resolveIncident(1001, "Manual issue resolved", "22:10:00");

        Incident inc1, inc2;
        system.getResponseManager().getIncidentByID(1001, inc1);
        system.getResponseManager().getIncidentByID(9001, inc2);

        assertTest(inc1.getStatus() == INC_RESOLVED, "Incident 1 is RESOLVED");
        assertTest(inc2.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Incident 2 remains AWAITING_ACKNOWLEDGEMENT independently");

        // Acknowledge Incident 2
        system.acknowledgeIncident(9001, 101, "22:11:00");
        system.getResponseManager().getIncidentByID(9001, inc2);
        assertTest(inc2.getStatus() == INC_ACKNOWLEDGED, "Incident 2 is ACKNOWLEDGED independently");
    }

    // ---------------------------------------------------------
    // TEST 16: Journey & Check-in Integration Intact
    // ---------------------------------------------------------
    cout << "\n--- Test 16: Journey and Check-in Behavior Intact ---" << endl;
    {
        SafetySystem system;
        system.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        // Start journey
        vector<string> route = { "Station A", "Station B", "Station C" };
        bool jOk = system.startJourney("Station A", "Station C", "23:00:00", route);
        assertTest(jOk, "Journey started");

        // Schedule check-in
        int chkId = system.scheduleCheckIn("22:30:00", "Midpoint check", "Station B");
        assertTest(chkId > 0, "Check-in scheduled");

        // Route deviation at 22:28:00
        system.updateJourneyLocation("Station B", "22:28:00"); // Expected Station A -> Deviation!

        // Current time reaches check-in deadline + grace (22:35:01) -> Missed check-in!
        // Elapsed from 22:28:00 is ~7 minutes (well inside 10m sliding window)
        // Combined with Route Deviation -> Score 65 -> Verification request created!
        system.processCurrentTimeUpdate("22:35:01");
        assertTest(system.hasActiveVerification(), "Verification request active");

        // Time reaches verification deadline (22:38:01) -> Verification times out -> Emergency incident dispatched!
        system.processCurrentTimeUpdate("22:38:01");
        assertTest(system.getResponseManager().getTotalIncidentCount() == 1, "Emergency incident automatically created on verification timeout");

        Incident inc;
        system.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Incident awaiting Tier 1 acknowledgement");
        assertTest(inc.isContactNotified(101), "Alice (P1) notified");

        // Contact acknowledges
        assertTest(system.acknowledgeIncident(9001, 101, "22:39:00"), "Alice acknowledges emergency");

        // Journey ends
        system.endJourney();
    }

    // ---------------------------------------------------------
    // SUMMARY
    // ---------------------------------------------------------
    cout << "\n==========================================================" << endl;
    cout << "DISPATCH & LIFECYCLE TEST SUMMARY: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << endl;
    cout << "==========================================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}
