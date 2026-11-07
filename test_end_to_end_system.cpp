#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "Contact.h"
#include "Event.h"
#include "Incident.h"
#include "RiskAnalyzer.h"
#include "ResponseManager.h"
#include "SafetySystem.h"
#include "JourneyManager.h"
#include "CheckInManager.h"
#include "VerificationManager.h"

using namespace std;

static int totalTestsPassed = 0;
static int totalTestsFailed = 0;

void assertTest(bool condition, const string& testName) {
    if (condition) {
        cout << "[PASS] " << testName << endl;
        totalTestsPassed++;
    } else {
        cout << "[FAIL] " << testName << endl;
        totalTestsFailed++;
    }
}

int main() {
    cout << "==========================================================" << endl;
    cout << "  RUNNING END-TO-END SYSTEM INTEGRATION TESTS (PHASE 4.6) " << endl;
    cout << "==========================================================" << endl;

    // -------------------------------------------------------------
    // SCENARIO 1: Normal journey with successful check-ins
    // -------------------------------------------------------------
    cout << "\n--- Scenario 1: Normal Journey with Successful Check-ins ---" << endl;
    {
        SafetySystem sys;
        vector<string> route = {"Campus Gate", "Library", "Science Hall", "Dormitory"};
        assertTest(sys.startJourney("Campus Gate", "Dormitory", "14:45:00", route), "Journey started successfully");

        assertTest(sys.scheduleCheckIn("14:15:00", "At library", "Library") > 0, "Check-in 1 scheduled");
        assertTest(sys.scheduleCheckIn("14:30:00", "At science hall", "Science Hall") > 0, "Check-in 2 scheduled");

        // Advance along route
        sys.updateJourneyLocation("Campus Gate", "14:00:00");
        sys.updateJourneyLocation("Library", "14:14:00");

        // Confirm check-in 1 on time
        assertTest(sys.completeCheckIn(1), "Check-in 1 completed on time");

        sys.updateJourneyLocation("Science Hall", "14:28:00");
        assertTest(sys.completeCheckIn(2), "Check-in 2 completed on time");

        sys.updateJourneyLocation("Dormitory", "14:40:00");

        // System state remains completely safe and normal
        assertTest(sys.getRiskAnalyzer().calculateRiskScore() == 0, "Risk score remains 0 throughout normal journey");
        assertTest(!sys.hasActiveVerification(), "No verification triggered during normal journey");
        assertTest(!sys.getResponseManager().hasActiveIncidents(), "No incidents triggered during normal journey");

        sys.endJourney();
    }

    // -------------------------------------------------------------
    // SCENARIO 2: Route deviation without a missed check-in
    // -------------------------------------------------------------
    cout << "\n--- Scenario 2: Route Deviation without Missed Check-in ---" << endl;
    {
        SafetySystem sys;
        vector<string> route = {"Start", "Checkpoint 1", "Checkpoint 2", "Destination"};
        sys.startJourney("Start", "Destination", "15:30:00", route);

        // Deviation to an unexpected location
        sys.updateJourneyLocation("Unknown Alley", "15:05:00");

        assertTest(sys.getRiskAnalyzer().getEventCount() == 1, "Route deviation logged as 1 event");
        assertTest(sys.getRiskAnalyzer().calculateRiskScore() == 20, "Single route deviation score is 20 (Unusual)");
        assertTest(!sys.getRiskAnalyzer().isRiskHigh(), "Single route deviation does NOT exceed threshold");
        assertTest(!sys.hasActiveVerification(), "No verification request for isolated route deviation");

        sys.endJourney();
    }

    // -------------------------------------------------------------
    // SCENARIO 3: Missed check-in without route deviation
    // -------------------------------------------------------------
    cout << "\n--- Scenario 3: Missed Check-in without Route Deviation ---" << endl;
    {
        SafetySystem sys;
        vector<string> route3 = {"Home", "Library", "Park"};
        sys.startJourney("Home", "Park", "17:00:00", route3);
        sys.scheduleCheckIn("16:00:00", "Afternoon study", "Library");

        // Grace period is 5 minutes -> Missed check-in generates at 16:05:01
        sys.processCurrentTimeUpdate("16:05:01");

        assertTest(sys.getRiskAnalyzer().getEventCount() == 1, "Missed check-in logged as 1 event");
        assertTest(sys.getRiskAnalyzer().calculateRiskScore() == 20, "Single missed check-in score is 20 (Unusual)");
        assertTest(!sys.getRiskAnalyzer().isRiskHigh(), "Single missed check-in does not exceed threshold");
        assertTest(!sys.hasActiveVerification(), "No verification request for isolated missed check-in");
        sys.endJourney();
    }

    // -------------------------------------------------------------
    // SCENARIO 4: Route deviation combined with missed check-in
    // SCENARIO 5: Risk score crossing verification threshold (>= 50)
    // -------------------------------------------------------------
    cout << "\n--- Scenario 4 & 5: Compound Risk Crossing Verification Threshold ---" << endl;
    {
        SafetySystem sys;
        vector<string> route = {"Point A", "Point B", "Point C"};
        sys.startJourney("Point A", "Point C", "17:30:00", route);
        sys.scheduleCheckIn("17:10:00", "Point B", "Halfway");

        // Route deviation at 17:08:00
        sys.updateJourneyLocation("Off Route Woods", "17:08:00");
        assertTest(sys.getRiskAnalyzer().calculateRiskScore() == 20, "Risk score 20 after deviation");

        // Check-in missed at 17:15:01 (within 10m sliding window)
        sys.processCurrentTimeUpdate("17:15:01");

        assertTest(sys.getRiskAnalyzer().getEventCount() == 2, "Both events active in 10-minute window");
        assertTest(sys.getRiskAnalyzer().calculateRiskScore() == 65, "Compound risk score is 65 (Base 40 + Pattern 25)");
        assertTest(sys.getRiskAnalyzer().isRiskHigh(), "Risk state is HIGH");
        assertTest(sys.hasActiveVerification(), "Safety verification request automatically created");
        assertTest(sys.getActiveVerificationID() == 5000, "Verification request assigned ID 5000");

        // Ensure no emergency incident has been dispatched prematurely
        assertTest(sys.getResponseManager().getActiveIncidentCount() == 0, "No emergency dispatched prematurely while awaiting verification");

        sys.endJourney();
    }

    // -------------------------------------------------------------
    // SCENARIO 6: User confirms safety
    // -------------------------------------------------------------
    cout << "\n--- Scenario 6: User Confirms Safety (Averting Escalation) ---" << endl;
    {
        SafetySystem sys;
        sys.logEvent(Event(1, "Route Deviation", "Dark Path", "18:00:00"));
        sys.logEvent(Event(2, "Missed Check-in", "Dark Path", "18:04:00"));

        assertTest(sys.hasActiveVerification(), "Verification active at score 65");

        assertTest(sys.confirmUserSafe(), "confirmUserSafe() returns true");
        assertTest(!sys.hasActiveVerification(), "No active verification after safety confirmation");

        VerificationRequest req;
        assertTest(sys.getVerificationManager().getRequestByID(5000, req), "Verification request #5000 retrieved");
        assertTest(req.status == VSTAT_CONFIRMED_SAFE, "Request status marked CONFIRMED_SAFE");

        // Confirm safety preserves history and prevents emergency dispatch
        assertTest(sys.getResponseManager().getTotalIncidentCount() == 0, "Zero emergency incidents created");
    }

    // -------------------------------------------------------------
    // SCENARIO 7: User requests immediate help
    // SCENARIO 10: Priority 1 notification
    // -------------------------------------------------------------
    cout << "\n--- Scenario 7 & 10: User Requests Help -> Priority 1 Notification ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));
        sys.registerContact(Contact(103, "Security", "555-0911", "Security", 3));

        sys.logEvent(Event(1, "Route Deviation", "North Quad", "19:00:00"));
        sys.logEvent(Event(2, "Missed Check-in", "North Quad", "19:03:00"));

        assertTest(sys.hasActiveVerification(), "Verification active");

        // User requests immediate help
        assertTest(sys.requestImmediateHelp(), "requestImmediateHelp() succeeds");
        assertTest(!sys.hasActiveVerification(), "Verification request closed");

        // Exactly one incident created
        assertTest(sys.getResponseManager().getActiveIncidentCount() == 1, "Exactly one active incident created");

        Incident inc;
        assertTest(sys.getResponseManager().getIncidentByID(9001, inc), "Incident #9001 created in ResponseManager");
        assertTest(inc.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Incident status is AWAITING_ACKNOWLEDGEMENT");
        assertTest(inc.getCurrentTier() == 1, "Current tier is Priority 1");
        assertTest(inc.getNotificationCount() == 1, "Only 1 notification sent initially");

        const auto& notes = inc.getNotificationHistory();
        assertTest(notes[0].contactID == 101, "Notification sent to Alice (Priority 1)");
        assertTest(notes[0].priorityLevel == 1, "Notification record level is 1");
        assertTest(!notes[0].acknowledged, "Notification not yet acknowledged");
    }

    // -------------------------------------------------------------
    // SCENARIO 8: Verification timeout triggers automatic dispatch
    // SCENARIO 9: Duplicate verification prevention
    // -------------------------------------------------------------
    cout << "\n--- Scenario 8 & 9: Verification Timeout & Duplicate Prevention ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        sys.logEvent(Event(1, "Route Deviation", "South Alley", "20:00:00"));
        sys.logEvent(Event(2, "Missed Check-in", "South Alley", "20:03:00"));

        assertTest(sys.hasActiveVerification(), "Verification request active");
        int reqID = sys.getActiveVerificationID();

        // Additional event while verification is pending should NOT create duplicate request
        sys.logEvent(Event(3, "Motion Alert", "South Alley", "20:04:00"));
        assertTest(sys.getVerificationManager().getTotalRequestCount() == 1, "Duplicate verification request prevented");
        assertTest(sys.getActiveVerificationID() == reqID, "Active verification request ID unchanged");

        // 3-minute timeout -> deadline is 20:06:00
        sys.processCurrentTimeUpdate("20:05:59");
        assertTest(sys.hasActiveVerification(), "Still pending 1s before deadline");
        assertTest(sys.getResponseManager().getActiveIncidentCount() == 0, "No emergency before timeout");

        // At exact deadline (20:06:00), timeout triggers automatic escalation
        sys.processCurrentTimeUpdate("20:06:00");
        assertTest(!sys.hasActiveVerification(), "Verification resolved via timeout");

        VerificationRequest req;
        sys.getVerificationManager().getRequestByID(reqID, req);
        assertTest(req.status == VSTAT_TIMED_OUT, "Verification request status is TIMED_OUT");

        assertTest(sys.getResponseManager().getActiveIncidentCount() == 1, "Incident automatically created on timeout");
        Incident inc;
        sys.getResponseManager().getIncidentByID(9001, inc);
        assertTest(inc.getVerificationRequestID() == reqID, "Incident preserves verification request ID");
    }

    // -------------------------------------------------------------
    // SCENARIO 11: Escalation to Priority 2
    // SCENARIO 12: Escalation to Priority 3
    // -------------------------------------------------------------
    cout << "\n--- Scenario 11 & 12: Tier Escalation (P1 -> P2 -> P3) ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));
        sys.registerContact(Contact(103, "Security", "555-0911", "Security", 3));

        // Create an incident at 21:00:00
        Incident inc(9001, "Late Night Distress", "Old Gym", 5, -1, 70, "Concerning", "21:00:00");
        sys.getResponseManager().dispatchEmergency(inc, "21:00:00");

        // Tier 1 dispatched with 2-min timeout -> deadline 21:02:00
        Incident checkInc;
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getCurrentTier() == 1, "Initially at Tier 1");
        assertTest(checkInc.getNotificationCount() == 1, "1 contact notified (Alice)");

        // 1 second before deadline -> no escalation
        sys.processCurrentTimeUpdate("21:01:59");
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getCurrentTier() == 1, "Still at Tier 1 before deadline");

        // At 21:02:00 -> Escalates to Tier 2
        sys.processCurrentTimeUpdate("21:02:00");
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getCurrentTier() == 2, "Escalated to Tier 2 at 21:02:00");
        assertTest(checkInc.getNotificationCount() == 2, "2 contacts notified total (Alice + Bob)");
        assertTest(checkInc.getNotificationHistory()[1].contactID == 102, "Second notification went to Bob (P2)");

        // At 21:04:00 (2 min after Tier 2) -> Escalates to Tier 3
        sys.processCurrentTimeUpdate("21:04:00");
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getCurrentTier() == 3, "Escalated to Tier 3 at 21:04:00");
        assertTest(checkInc.getNotificationCount() == 3, "3 contacts notified total (Alice + Bob + Security)");
        assertTest(checkInc.getNotificationHistory()[2].contactID == 103, "Third notification went to Security (P3)");
    }

    // -------------------------------------------------------------
    // SCENARIO 13: Contact acknowledgement halts escalation
    // -------------------------------------------------------------
    cout << "\n--- Scenario 13: Contact Acknowledgement Halts Escalation ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));

        Incident inc(9001, "Distress", "Path C", 5, -1, 65, "Concerning", "21:10:00");
        sys.getResponseManager().dispatchEmergency(inc, "21:10:00");

        // Alice (101) acknowledges at 21:11:00
        assertTest(sys.acknowledgeIncident(9001, 101, "21:11:00"), "Alice acknowledges incident #9001");

        Incident checkInc;
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getStatus() == INC_ACKNOWLEDGED, "Status is ACKNOWLEDGED");
        assertTest(checkInc.getAcknowledgedByContactID() == 101, "Acknowledged by Alice (101)");
        assertTest(checkInc.getAcknowledgementTimestamp() == "21:11:00", "Acknowledgement timestamp recorded");

        // Time passes deadline (21:12:00) -> Must NOT escalate to Tier 2
        sys.processCurrentTimeUpdate("21:13:00");
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getCurrentTier() == 1, "Tier remains 1; escalation halted");
        assertTest(checkInc.getNotificationCount() == 1, "No further notifications sent after acknowledgement");
    }

    // -------------------------------------------------------------
    // SCENARIO 14: Incident resolution
    // SCENARIO 15: Incident cancellation
    // -------------------------------------------------------------
    cout << "\n--- Scenario 14 & 15: Incident Resolution & Cancellation ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        Incident inc1(9001, "Incident 1", "Loc A", 5, -1, 60, "Concerning", "10:00:00");
        Incident inc2(9002, "Incident 2", "Loc B", 5, -1, 60, "Concerning", "10:00:00");
        sys.getResponseManager().dispatchEmergency(inc1, "10:00:00");
        sys.getResponseManager().dispatchEmergency(inc2, "10:00:00");

        // Resolve Incident 1
        assertTest(sys.resolveIncident(9001, "User safely escorted", "10:15:00"), "Resolve Incident 1 succeeds");
        Incident c1;
        sys.getResponseManager().getIncidentByID(9001, c1);
        assertTest(c1.getStatus() == INC_RESOLVED, "Incident 1 status is RESOLVED");
        assertTest(!c1.isActive(), "Incident 1 isActive() is false");
        assertTest(c1.getResolutionNotes() == "User safely escorted", "Resolution notes preserved");

        // Cancel Incident 2 (False Alarm)
        assertTest(sys.cancelIncident(9002, "Accidental panic button press", "10:16:00"), "Cancel Incident 2 succeeds");
        Incident c2;
        sys.getResponseManager().getIncidentByID(9002, c2);
        assertTest(c2.getStatus() == INC_CANCELLED, "Incident 2 status is CANCELLED");
        assertTest(!c2.isActive(), "Incident 2 isActive() is false");
        assertTest(c2.getCancellationReason() == "Accidental panic button press", "Cancellation reason preserved");

        // Disallow re-resolving or re-cancelling
        assertTest(!sys.resolveIncident(9001, "Repeat resolution", "10:20:00"), "Cannot re-resolve resolved incident");
        assertTest(!sys.cancelIncident(9001, "Cannot cancel resolved", "10:20:00"), "Cannot cancel resolved incident");
        assertTest(!sys.resolveIncident(9002, "Cannot resolve cancelled", "10:20:00"), "Cannot resolve cancelled incident");
        assertTest(!sys.cancelIncident(9002, "Repeat cancel", "10:20:00"), "Cannot re-cancel cancelled incident");
    }

    // -------------------------------------------------------------
    // SCENARIO 16: Escalation exhaustion
    // -------------------------------------------------------------
    cout << "\n--- Scenario 16: Escalation Exhaustion ---" << endl;
    {
        SafetySystem sys;
        // Only Tier 1 contact registered
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));

        Incident inc(9001, "Unacknowledged Alert", "Isolated Sector", 5, -1, 75, "Concerning", "11:00:00");
        sys.getResponseManager().dispatchEmergency(inc, "11:00:00");

        // Alice doesn't answer -> 11:02:00 timeout tries Tier 2 (empty) -> tries Tier 3 (empty) -> EXHAUSTED
        sys.processCurrentTimeUpdate("11:02:00");

        Incident checkInc;
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getStatus() == INC_ESCALATION_EXHAUSTED, "Status is ESCALATION_EXHAUSTED when no further contacts remain");
        assertTest(checkInc.isActive(), "Escalation exhausted incident remains active for emergency visibility");
    }

    // -------------------------------------------------------------
    // SCENARIO 17: Multiple incidents running independently
    // -------------------------------------------------------------
    cout << "\n--- Scenario 17: Multiple Incidents Running Independently ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));
        sys.registerContact(Contact(103, "Security", "555-0911", "Security", 3));

        // Incident A created at 12:00:00 (Deadline 12:02:00)
        Incident incA(1001, "Incident A", "North", 4, -1, 60, "Concerning", "12:00:00");
        sys.getResponseManager().dispatchEmergency(incA, "12:00:00");

        // Incident B created at 12:01:00 (Deadline 12:03:00)
        Incident incB(1002, "Incident B", "South", 5, -1, 80, "High Risk", "12:01:00");
        sys.getResponseManager().dispatchEmergency(incB, "12:01:00");

        // At 12:02:00: Incident A reaches deadline and escalates to Tier 2; Incident B still at Tier 1 (deadline 12:03:00)
        sys.processCurrentTimeUpdate("12:02:00");
        Incident a1, b1;
        sys.getResponseManager().getIncidentByID(1001, a1);
        sys.getResponseManager().getIncidentByID(1002, b1);
        assertTest(a1.getCurrentTier() == 2, "Incident A escalated to Tier 2 at 12:02:00");
        assertTest(b1.getCurrentTier() == 1, "Incident B remains at Tier 1 (its deadline is 12:03:00)");

        // Alice acknowledges Incident B only
        assertTest(sys.acknowledgeIncident(1002, 101, "12:02:30"), "Alice acknowledges Incident B");
        sys.getResponseManager().getIncidentByID(1002, b1);
        assertTest(b1.getStatus() == INC_ACKNOWLEDGED, "Incident B status is ACKNOWLEDGED");

        // Advance to 12:04:00: Incident A escalates to Tier 3; Incident B does NOT escalate because it's acknowledged
        sys.processCurrentTimeUpdate("12:04:00");
        sys.getResponseManager().getIncidentByID(1001, a1);
        sys.getResponseManager().getIncidentByID(1002, b1);
        assertTest(a1.getCurrentTier() == 3, "Incident A escalated to Tier 3");
        assertTest(b1.getCurrentTier() == 1, "Incident B remained at Tier 1 (not escalated)");
        assertTest(b1.getNotificationCount() == 1, "Incident B notification count remained 1");
    }

    // -------------------------------------------------------------
    // SCENARIO 18: Invalid user input and invalid IDs
    // -------------------------------------------------------------
    cout << "\n--- Scenario 18: Invalid User Input & Invalid IDs ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        Incident inc(9001, "Test Alert", "Loc", 5, -1, 60, "Concerning", "13:00:00");
        sys.getResponseManager().dispatchEmergency(inc, "13:00:00");

        // Non-existent incident ID acknowledgement
        assertTest(!sys.acknowledgeIncident(9999, 101, "13:01:00"), "Acknowledging non-existent incident rejected");

        // Non-existent contact ID acknowledgement
        assertTest(!sys.acknowledgeIncident(9001, 9999, "13:01:00"), "Acknowledgement by unregistered contact rejected");

        // Unnotified contact acknowledgement (Bob has not been notified yet)
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));
        assertTest(!sys.acknowledgeIncident(9001, 102, "13:01:00"), "Acknowledgement by unnotified contact rejected");

        // Non-existent incident resolution
        assertTest(!sys.resolveIncident(9999, "Notes", "13:05:00"), "Resolving non-existent incident rejected");

        // Non-existent incident cancellation
        assertTest(!sys.cancelIncident(9999, "Reason", "13:05:00"), "Cancelling non-existent incident rejected");

        // Invalid verification request ID
        assertTest(!sys.confirmUserSafe(9999), "Confirming non-existent verification request rejected");
        assertTest(!sys.requestImmediateHelp(9999), "Requesting help for non-existent verification request rejected");
    }

    // -------------------------------------------------------------
    // SCENARIO 19: Repeated simulated time updates & midnight rollover
    // -------------------------------------------------------------
    cout << "\n--- Scenario 19: Repeated Time Updates & Midnight Rollover ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Night Owl", "555-0999", "Security", 1));

        // Dispatch incident at 23:59:00 -> Tier 1 deadline 00:01:00
        Incident inc(9001, "Midnight Alert", "West Wing", 5, -1, 65, "Concerning", "23:59:00");
        sys.getResponseManager().dispatchEmergency(inc, "23:59:00");

        // Repeated identical timestamp updates
        sys.processCurrentTimeUpdate("23:59:00");
        sys.processCurrentTimeUpdate("23:59:00");
        Incident checkInc;
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getCurrentTier() == 1, "Repeated identical time updates cause no duplicate state changes");

        // Midnight rollover across 00:00:00 to 00:01:00 -> Deadline met!
        sys.processCurrentTimeUpdate("00:01:00");
        sys.getResponseManager().getIncidentByID(9001, checkInc);
        assertTest(checkInc.getStatus() == INC_ESCALATION_EXHAUSTED || checkInc.getCurrentTier() > 1,
                   "Simulated time correctly advances across midnight rollover");

        // Backward time jump rejected
        sys.processCurrentTimeUpdate("23:00:00");
        // State remains intact
        assertTest(sys.getResponseManager().getTotalIncidentCount() == 1, "Total incident count intact after rejected backward jump");
    }

    // -------------------------------------------------------------
    // SCENARIO 20: Preservation of complete incident and notification history
    // -------------------------------------------------------------
    cout << "\n--- Scenario 20: Preservation of Complete History ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));

        Incident inc1(101, "Incident One", "Library", 4, 5001, 60, "Concerning", "08:00:00");
        Incident inc2(102, "Incident Two", "Cafeteria", 5, 5002, 80, "High Risk", "08:05:00");
        sys.getResponseManager().dispatchEmergency(inc1, "08:00:00");
        sys.getResponseManager().dispatchEmergency(inc2, "08:05:00");

        // Acknowledge inc1, resolve it
        sys.acknowledgeIncident(101, 101, "08:01:30");
        sys.resolveIncident(101, "Checked and clear", "08:10:00");

        // Cancel inc2
        sys.cancelIncident(102, "User called back - mistaken alert", "08:06:00");

        // Verify history
        vector<Incident> allIncidents = sys.getResponseManager().getAllIncidents();
        assertTest(allIncidents.size() == 2, "All incidents preserved in history vector");

        Incident h1, h2;
        sys.getResponseManager().getIncidentByID(101, h1);
        sys.getResponseManager().getIncidentByID(102, h2);

        assertTest(h1.getStatus() == INC_RESOLVED, "Incident 1 status preserved as RESOLVED");
        assertTest(h1.getAcknowledgedByName() == "Alice", "Incident 1 acknowledgement audit trail preserved");
        assertTest(h1.getResolutionNotes() == "Checked and clear", "Incident 1 resolution notes preserved");

        assertTest(h2.getStatus() == INC_CANCELLED, "Incident 2 status preserved as CANCELLED");
        assertTest(h2.getCancellationReason() == "User called back - mistaken alert", "Incident 2 cancellation reason preserved");

        assertTest(sys.getResponseManager().getTotalNotificationCount() >= 2, "All notification records preserved in audit trail");
    }

    // -------------------------------------------------------------
    // SCENARIO 21: Legacy "Process Next Emergency" Menu Compatibility
    // -------------------------------------------------------------
    cout << "\n--- Scenario 21: Legacy Process Next Emergency Compatibility ---" << endl;
    {
        SafetySystem sys;
        sys.registerContact(Contact(101, "Alice", "555-0101", "Family", 1));
        sys.registerContact(Contact(102, "Bob", "555-0102", "Friend", 2));

        // Report manual incident (Option 3 in menu) -> enters incidentQueue with INC_PENDING
        Incident manualInc(777, "Manual Emergency Report", "Main Gate", 4);
        sys.reportIncident(manualInc);

        assertTest(sys.getResponseManager().hasPendingIncidents(), "Manual incident present in pending queue");
        assertTest(sys.getResponseManager().getPendingIncidentCount() == 1, "Pending incident count is 1");

        // Call processNextEmergency() (Option 8 in menu)
        sys.processNextEmergency();

        // Must initiate priority dispatch, NOT bypass acknowledgement or lifecycle!
        Incident triaged;
        assertTest(sys.getResponseManager().getIncidentByID(777, triaged), "Incident #777 found in registry");
        assertTest(triaged.getStatus() == INC_AWAITING_ACKNOWLEDGEMENT, "Triaged incident status is AWAITING_ACKNOWLEDGEMENT");
        assertTest(triaged.getCurrentTier() == 1, "Triaged incident entered Priority 1 dispatch");
        assertTest(triaged.getNotificationCount() == 1, "Alice notified at Priority 1");
        assertTest(!sys.getResponseManager().hasPendingIncidents(), "Queue has 0 pending items after triage");

        // Verify it can be acknowledged and resolved through standard lifecycle
        assertTest(sys.acknowledgeIncident(777, 101, "09:01:00"), "Triaged manual incident acknowledged through lifecycle");
        assertTest(sys.resolveIncident(777, "Resolved by responder", "09:05:00"), "Triaged manual incident resolved through lifecycle");
    }

    cout << "\n==========================================================" << endl;
    cout << "END-TO-END TEST SUMMARY: " << totalTestsPassed << " PASSED, "
         << totalTestsFailed << " FAILED" << endl;
    cout << "==========================================================" << endl;

    return (totalTestsFailed == 0) ? 0 : 1;
}

