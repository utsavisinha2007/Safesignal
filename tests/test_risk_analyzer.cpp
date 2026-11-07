#include <iostream>
#include <string>
#include <vector>
#include "RiskAnalyzer.h"
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
    cout << "       RUNNING UNIT TESTS FOR RISKANALYZER          " << endl;
    cout << "====================================================\n" << endl;

    // ---------------------------------------------------------
    // TEST 1: Empty Event Window
    // ---------------------------------------------------------
    cout << "--- Test 1: Empty Event Window ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        assertTest(analyzer.getEventCount() == 0, "Event count is 0 in empty window");
        assertTest(analyzer.calculateRiskScore() == 0, "Risk score is 0 in empty window");
        assertTest(analyzer.getRiskLevel() == "Normal", "Risk level is 'Normal' for empty window (0-19)");
        assertTest(!analyzer.isRiskHigh(), "isRiskHigh() is false for empty window");
        assertTest(analyzer.getDetectedPatterns().empty(), "No patterns detected in empty window");
    }

    // ---------------------------------------------------------
    // TEST 2: One Route Deviation
    // ---------------------------------------------------------
    cout << "\n--- Test 2: One Route Deviation ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(101, "Route Deviation", "North Alley", "14:00:00"));

        assertTest(analyzer.getEventCount() == 1, "Event count is 1");
        assertTest(analyzer.calculateRiskScore() == 20, "Single route deviation score is 20 (base severity only)");
        assertTest(analyzer.getRiskLevel() == "Unusual", "Risk level is 'Unusual' (20-49)");
        assertTest(!analyzer.isRiskHigh(), "Single route deviation does not trigger high risk");
        assertTest(analyzer.getDetectedPatterns().empty(), "No danger pattern triggered for single route deviation");
    }

    // ---------------------------------------------------------
    // TEST 3: One Missed Check-in
    // ---------------------------------------------------------
    cout << "\n--- Test 3: One Missed Check-in ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(201, "Missed Check-in", "Library Gate", "14:00:00"));

        assertTest(analyzer.getEventCount() == 1, "Event count is 1");
        assertTest(analyzer.calculateRiskScore() == 20, "Single missed check-in score is 20 (base severity only)");
        assertTest(analyzer.getRiskLevel() == "Unusual", "Risk level is 'Unusual' (20-49)");
        assertTest(!analyzer.isRiskHigh(), "Single missed check-in does not trigger high risk");
        assertTest(analyzer.getDetectedPatterns().empty(), "No danger pattern triggered for single missed check-in");
    }

    // ---------------------------------------------------------
    // TEST 4: Route Deviation Followed by Missed Check-in
    // ---------------------------------------------------------
    cout << "\n--- Test 4: Route Deviation + Missed Check-in Pattern ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(301, "Route Deviation", "North Alley", "14:00:00"));
        analyzer.addEvent(Event(302, "Missed Check-in", "Science Plaza", "14:04:00"));

        assertTest(analyzer.getEventCount() == 2, "Event count is 2");
        // Base: 20 + 20 = 40. Pattern bonus: +25. Total: 65.
        assertTest(analyzer.calculateRiskScore() == 65, "Compound pattern score is 65 (Base 40 + Pattern 25)");
        assertTest(analyzer.getRiskLevel() == "Concerning", "Risk level escalated to 'Concerning' (50-79)");
        assertTest(analyzer.isRiskHigh(), "isRiskHigh() is true for Concerning compound pattern");

        vector<string> patterns = analyzer.getDetectedPatterns();
        bool foundPattern = false;
        for (const auto& p : patterns) {
            if (p == "Route Deviation + Missed Check-in") foundPattern = true;
        }
        assertTest(foundPattern, "Detected pattern 'Route Deviation + Missed Check-in' is recorded");
    }

    // ---------------------------------------------------------
    // TEST 5: Multiple Missed Check-ins
    // ---------------------------------------------------------
    cout << "\n--- Test 5: Multiple Missed Check-ins Pattern ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(401, "Missed Check-in", "Gate 1", "14:00:00"));
        analyzer.addEvent(Event(402, "Missed Check-in", "Gate 2", "14:05:00"));

        assertTest(analyzer.getEventCount() == 2, "Event count is 2");
        // Base: 20 + 20 = 40. Frequency bonus: +5. Pattern bonus: +15. Total: 60.
        assertTest(analyzer.calculateRiskScore() == 60, "Multiple missed check-ins score is 60 (Base 40 + Freq 5 + Pattern 15)");
        assertTest(analyzer.getRiskLevel() == "Concerning", "Risk level is 'Concerning' (50-79)");
        assertTest(analyzer.isRiskHigh(), "isRiskHigh() is true for multiple missed check-ins");

        vector<string> patterns = analyzer.getDetectedPatterns();
        bool foundPattern = false;
        for (const auto& p : patterns) {
            if (p == "Multiple Missed Check-ins") foundPattern = true;
        }
        assertTest(foundPattern, "Detected pattern 'Multiple Missed Check-ins' is recorded");
    }

    // ---------------------------------------------------------
    // TEST 6: Repeated Route Deviations
    // ---------------------------------------------------------
    cout << "\n--- Test 6: Repeated Route Deviations Pattern ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(501, "Route Deviation", "Sector A", "14:00:00"));
        analyzer.addEvent(Event(502, "Route Deviation", "Sector B", "14:06:00"));

        assertTest(analyzer.getEventCount() == 2, "Event count is 2");
        // Base: 20 + 20 = 40. Frequency bonus: +5. Pattern bonus: +15. Total: 60.
        assertTest(analyzer.calculateRiskScore() == 60, "Repeated route deviations score is 60 (Base 40 + Freq 5 + Pattern 15)");
        assertTest(analyzer.getRiskLevel() == "Concerning", "Risk level is 'Concerning'");
        assertTest(analyzer.isRiskHigh(), "isRiskHigh() is true for repeated route deviations");

        vector<string> patterns = analyzer.getDetectedPatterns();
        bool foundPattern = false;
        for (const auto& p : patterns) {
            if (p == "Repeated Route Deviations") foundPattern = true;
        }
        assertTest(foundPattern, "Detected pattern 'Repeated Route Deviations' is recorded");
    }

    // ---------------------------------------------------------
    // TEST 7: Events Outside the Time Window (Pruning)
    // ---------------------------------------------------------
    cout << "\n--- Test 7: Events Outside Time Window ---" << endl;
    {
        RiskAnalyzer analyzer(10); // 10 min window
        analyzer.addEvent(Event(601, "Route Deviation", "Location A", "14:00:00"));
        assertTest(analyzer.getEventCount() == 1, "First event in window");

        // Event 2 arrives 15 minutes later (14:15:00) -> Event 1 must be pruned
        analyzer.addEvent(Event(602, "Motion Alert", "Location B", "14:15:00"));
        assertTest(analyzer.getEventCount() == 1, "Event older than 10 minutes successfully pruned from window");
        assertTest(analyzer.calculateRiskScore() == 10, "Risk score reflects only the active event (Motion Alert = 10)");
        assertTest(analyzer.getRiskLevel() == "Normal", "Risk level dropped back to 'Normal'");
    }

    // ---------------------------------------------------------
    // TEST 8: Midnight Rollover Handling
    // ---------------------------------------------------------
    cout << "\n--- Test 8: Midnight Rollover ---" << endl;
    {
        RiskAnalyzer analyzer(10); // 10 min window
        // Event 1 logged 5 minutes before midnight (23:55:00)
        analyzer.addEvent(Event(701, "Route Deviation", "South Wall", "23:55:00"));

        // Event 2 logged 3 minutes after midnight (00:03:00) -> Elapsed = 8 mins (within 10m window)
        analyzer.addEvent(Event(702, "Missed Check-in", "East Gate", "00:03:00"));

        assertTest(analyzer.getEventCount() == 2, "Both events maintained across midnight rollover (8 mins elapsed <= 10m window)");
        assertTest(analyzer.calculateRiskScore() == 65, "Pattern 'Route Deviation + Missed Check-in' recognized across midnight rollover");

        // Event 3 logged at 00:08:00 -> 13 minutes after Event 1 (23:55:00) -> Event 1 should expire
        analyzer.addEvent(Event(703, "Motion Alert", "East Gate", "00:08:00"));
        assertTest(analyzer.getEventCount() == 2, "Pre-midnight event pruned when elapsed time exceeds 10m window (13 mins elapsed)");
    }

    // ---------------------------------------------------------
    // TEST 9: Invalid Timestamps Rejection
    // ---------------------------------------------------------
    cout << "\n--- Test 9: Invalid Timestamps Handling ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(801, "Motion Alert", "Lobby", "14:00:00"));
        assertTest(analyzer.getEventCount() == 1, "Valid event logged");

        // Invalid timestamps
        analyzer.addEvent(Event(802, "SOS", "Lobby", "25:00:00"));          // Invalid hour 25
        analyzer.addEvent(Event(803, "Fall Detected", "Lobby", "14:75:00")); // Invalid min 75
        analyzer.addEvent(Event(804, "SOS", "Lobby", "14:30abc"));          // Trailing characters
        analyzer.addEvent(Event(805, "SOS", "Lobby", "invalid_time"));      // Non-numeric
        analyzer.addEvent(Event(806, "SOS", "Lobby", ""));                  // Empty

        assertTest(analyzer.getEventCount() == 1, "All 5 invalid timestamp events rejected from window");
        assertTest(analyzer.calculateRiskScore() == 10, "Risk score remains uncorrupted at 10");
    }

    // ---------------------------------------------------------
    // TEST 10: Risk Score Cap at 100
    // ---------------------------------------------------------
    cout << "\n--- Test 10: Risk Score Cap at 100 ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        // Add multiple high-weight events:
        analyzer.addEvent(Event(901, "SOS", "Main Lab", "14:00:00"));            // Weight 40
        analyzer.addEvent(Event(902, "Fall Detected", "Main Lab", "14:01:00"));   // Weight 30
        analyzer.addEvent(Event(903, "Route Deviation", "Main Lab", "14:02:00")); // Weight 20
        analyzer.addEvent(Event(904, "Missed Check-in", "Main Lab", "14:03:00")); // Weight 20
        // Base severity: 40 + 30 + 20 + 20 = 110.
        // Plus Pattern bonuses: Route Deviation + Missed Check-in (+25), Burst >= 3 events (+10) -> Raw total: 145!

        assertTest(analyzer.calculateRiskScore() == 100, "Cumulative score of 145 pts strictly capped at 100");
        assertTest(analyzer.getRiskLevel() == "High Risk", "Risk level is 'High Risk' (80-100)");
        assertTest(analyzer.isRiskHigh(), "isRiskHigh() is true for High Risk tier");
    }

    // ---------------------------------------------------------
    // TEST 11: Old Events Not Contributing to Current Risk
    // ---------------------------------------------------------
    cout << "\n--- Test 11: Old Events Pruned and Inactive ---" << endl;
    {
        RiskAnalyzer analyzer(10); // 10 min window
        // Ingest high threat events at 12:00:00
        analyzer.addEvent(Event(1001, "SOS", "Perimeter", "12:00:00"));
        analyzer.addEvent(Event(1002, "Fall Detected", "Perimeter", "12:02:00"));
        assertTest(analyzer.calculateRiskScore() == 70, "Active threat score is 70");
        assertTest(analyzer.getRiskLevel() == "Concerning", "Threat level is 'Concerning'");

        // Fast forward 25 minutes to 12:25:00 with a minor event
        analyzer.addEvent(Event(1003, "Motion Alert", "Main Building", "12:25:00"));

        assertTest(analyzer.getEventCount() == 1, "Only the newest event remains in active window");
        assertTest(analyzer.calculateRiskScore() == 10, "Old severe events (SOS, Fall) no longer contribute to current risk");
        assertTest(analyzer.getRiskLevel() == "Normal", "Risk level normalized to 'Normal' (10 pts)");
    }

    // ---------------------------------------------------------
    // TEST 12: Time Advancing Without a New Event
    // ---------------------------------------------------------
    cout << "\n--- Test 12: Time Advancing Without a New Event ---" << endl;
    {
        RiskAnalyzer analyzer(10); // 10 min window
        analyzer.addEvent(Event(1201, "Route Deviation", "North Alley", "14:00:00"));
        assertTest(analyzer.getEventCount() == 1, "Event logged at 14:00:00");
        assertTest(analyzer.calculateRiskScore() == 20, "Risk score is 20");

        // Advance time to 14:05:00 without adding any new event
        analyzer.updateCurrentTime("14:05:00");
        assertTest(analyzer.getEventCount() == 1, "Event still in window at 14:05:00 (5 mins elapsed <= 10m)");
        assertTest(analyzer.calculateRiskScore() == 20, "Risk score remains 20 at 14:05:00");

        // Advance time to 14:11:00 without adding any new event -> Event must expire
        analyzer.updateCurrentTime("14:11:00");
        assertTest(analyzer.getEventCount() == 0, "Event expired and removed by time advancement without new event (11 mins elapsed)");
        assertTest(analyzer.calculateRiskScore() == 0, "Risk score drops to 0 after event expiration");
        assertTest(analyzer.getRiskLevel() == "Normal", "Risk level returns to 'Normal'");
    }

    // ---------------------------------------------------------
    // TEST 13: Events Expiring After Multiple Days
    // ---------------------------------------------------------
    cout << "\n--- Test 13: Events Expiring After Multiple Days ---" << endl;
    {
        RiskAnalyzer analyzer(10); // 10 min window
        // Ingest date-aware event on Nov 7
        analyzer.addEvent(Event(1301, "SOS", "Campus Gate", "2026-11-07 14:00:00"));
        assertTest(analyzer.getEventCount() == 1, "Date-aware event logged on 2026-11-07 14:00:00");
        assertTest(analyzer.calculateRiskScore() == 40, "Risk score is 40");

        // Advance time to Nov 9 (2 days later)
        analyzer.updateCurrentTime("2026-11-09 14:00:00");
        assertTest(analyzer.getEventCount() == 0, "Event expired across multi-day gap (2 days elapsed >> 10m)");
        assertTest(analyzer.calculateRiskScore() == 0, "Risk score is 0 after multiple days have passed");
    }

    // ---------------------------------------------------------
    // TEST 14: Chronological Ordering & Out-of-Order Rejection
    // ---------------------------------------------------------
    cout << "\n--- Test 14: Out-of-Order Timestamps Rejection ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(1401, "Route Deviation", "Loc A", "15:00:00"));
        assertTest(analyzer.getEventCount() == 1, "First event at 15:00:00 accepted");

        // Attempt to insert an earlier event at 14:55:00 (out-of-order)
        analyzer.addEvent(Event(1402, "SOS", "Loc B", "14:55:00"));
        assertTest(analyzer.getEventCount() == 1, "Out-of-order event at 14:55:00 rejected from window");
        assertTest(analyzer.calculateRiskScore() == 20, "Risk score unchanged at 20 (SOS was rejected)");

        // Attempt to rewind simulation time backwards
        analyzer.updateCurrentTime("14:50:00");
        assertTest(analyzer.getLatestTimestamp() == "15:00:00", "Simulation timeline cannot be rewound backwards");

        // Normal chronological event at 15:04:00 should be accepted
        analyzer.addEvent(Event(1403, "Missed Check-in", "Loc C", "15:04:00"));
        assertTest(analyzer.getEventCount() == 2, "Chronological event at 15:04:00 accepted");
        assertTest(analyzer.calculateRiskScore() == 65, "Compound pattern formed with valid chronological event");
    }

    // ---------------------------------------------------------
    // TEST 15: Changing the Window Duration
    // ---------------------------------------------------------
    cout << "\n--- Test 15: Changing Window Duration Updates Window Immediately ---" << endl;
    {
        RiskAnalyzer analyzer(10); // Start with 10 min window
        analyzer.addEvent(Event(1501, "Route Deviation", "Loc 1", "16:00:00"));
        analyzer.addEvent(Event(1502, "Missed Check-in", "Loc 2", "16:06:00"));

        assertTest(analyzer.getEventCount() == 2, "Both events inside 10-minute window (6 mins apart)");
        assertTest(analyzer.calculateRiskScore() == 65, "Score is 65 (Compound pattern)");

        // Reduce window to 4 minutes: Event 1 (6 mins older than Event 2) must immediately expire!
        analyzer.setWindowDurationMinutes(4);
        assertTest(analyzer.getWindowDurationMinutes() == 4, "Window duration updated to 4 minutes");
        assertTest(analyzer.getEventCount() == 1, "Event 1 immediately pruned upon reducing window duration to 4m");
        assertTest(analyzer.calculateRiskScore() == 20, "Risk score immediately drops to 20 (Pattern broken by window change)");
        assertTest(analyzer.getRiskLevel() == "Unusual", "Risk level updated to 'Unusual'");
    }

    // ---------------------------------------------------------
    // TEST 16: Risk Score Decreasing After Event Expiration
    // ---------------------------------------------------------
    cout << "\n--- Test 16: Risk Score Decreasing After Expiration ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        // Build up Concerning risk (score 65)
        analyzer.addEvent(Event(1601, "Route Deviation", "North", "17:00:00"));
        analyzer.addEvent(Event(1602, "Missed Check-in", "South", "17:03:00"));
        assertTest(analyzer.calculateRiskScore() == 65, "Initial score is 65 (Concerning)");
        assertTest(analyzer.isRiskHigh(), "isRiskHigh() is true");

        // Advance time to 17:11:00 -> Event 1 (17:00:00) expires (11m elapsed), Event 2 (17:03:00) remains (8m elapsed)
        analyzer.updateCurrentTime("17:11:00");
        assertTest(analyzer.getEventCount() == 1, "Event 1 expired, only Event 2 remains in window");
        assertTest(analyzer.calculateRiskScore() == 20, "Risk score decreased from 65 to 20");
        assertTest(analyzer.getRiskLevel() == "Unusual", "Risk level decreased from Concerning to Unusual");
        assertTest(!analyzer.isRiskHigh(), "isRiskHigh() is now false after score decreased below 50");

        // Advance time to 17:15:00 -> Event 2 expires as well
        analyzer.updateCurrentTime("17:15:00");
        assertTest(analyzer.getEventCount() == 0, "Event 2 expired as well");
        assertTest(analyzer.calculateRiskScore() == 0, "Risk score decreased to 0");
        assertTest(analyzer.getRiskLevel() == "Normal", "Risk level decreased to Normal");
    }

    // ---------------------------------------------------------
    // TEST 17: Mixed Timestamp Formats (Forward and Reverse)
    // ---------------------------------------------------------
    cout << "\n--- Test 17: Mixed Timestamp Formats ---" << endl;
    {
        // Part A: Date-aware first, then time-only
        RiskAnalyzer analyzerA(10);
        analyzerA.addEvent(Event(1701, "Route Deviation", "North Gate", "2026-11-07 14:00:00"));
        assertTest(analyzerA.getEventCount() == 1, "Date-aware event 1 accepted at 2026-11-07 14:00:00");

        analyzerA.updateCurrentTime("14:05:00");
        assertTest(analyzerA.getEventCount() == 1, "Event 1 still active at time-only 14:05:00 (5 mins elapsed)");

        analyzerA.addEvent(Event(1702, "Missed Check-in", "East Corridor", "14:06:00"));
        assertTest(analyzerA.getEventCount() == 2, "Time-only event 2 accepted at 14:06:00 and coexists with date-aware event 1");
        assertTest(analyzerA.calculateRiskScore() == 65, "Compound pattern formed across mixed timestamp formats (Score: 65)");

        // Advance to 14:11:00 -> Event 1 (14:00:00) expires (11m elapsed), Event 2 (14:06:00) remains (5m elapsed)
        analyzerA.updateCurrentTime("14:11:00");
        assertTest(analyzerA.getEventCount() == 1, "Event 1 expired, Event 2 remains active in mixed format timeline");
        assertTest(analyzerA.calculateRiskScore() == 20, "Risk score drops to 20 after event 1 expires in mixed timeline");

        // Part B: Reverse order - Time-only first, then date-aware
        RiskAnalyzer analyzerB(10);
        analyzerB.addEvent(Event(1703, "Route Deviation", "Campus Center", "14:00:00"));
        assertTest(analyzerB.getEventCount() == 1, "Time-only event accepted at 14:00:00");

        analyzerB.updateCurrentTime("2026-11-07 14:05:00");
        assertTest(analyzerB.getEventCount() == 1, "Time-only event preserved when advancing time using date-aware timestamp");

        analyzerB.addEvent(Event(1704, "Missed Check-in", "Library Lawn", "14:06:00"));
        assertTest(analyzerB.getEventCount() == 2, "Second time-only event accepted after date-aware time advancement");
        assertTest(analyzerB.calculateRiskScore() == 65, "Compound pattern correctly evaluated in reverse mixed order");

        analyzerB.updateCurrentTime("2026-11-07 14:11:00");
        assertTest(analyzerB.getEventCount() == 1, "Event 1 expired at date-aware 14:11:00, Event 2 preserved");
    }

    // ---------------------------------------------------------
    // TEST 18: Backward Time Jumps and Zero State Corruption
    // ---------------------------------------------------------
    cout << "\n--- Test 18: Backward Time Jumps & State Corruption Prevention ---" << endl;
    {
        RiskAnalyzer analyzer(10);
        analyzer.addEvent(Event(1801, "Route Deviation", "Sector 7", "15:00:00"));
        assertTest(analyzer.getEventCount() == 1, "Initial event at 15:00:00 accepted");
        assertTest(analyzer.getLatestTimestamp() == "15:00:00", "Latest timestamp is 15:00:00");

        // Attempt backward time jump: 15:00:00 to 02:00:00 (afternoon to early morning without date rollover context)
        analyzer.updateCurrentTime("02:00:00");
        assertTest(analyzer.getLatestTimestamp() == "15:00:00", "Timeline timestamp preserved after rejected backward jump to 02:00:00");

        // Attempt adding an event at 02:00:00
        analyzer.addEvent(Event(1802, "SOS", "Unknown", "02:00:00"));
        assertTest(analyzer.getEventCount() == 1, "Out-of-order event at 02:00:00 rejected; event count remains 1");
        assertTest(analyzer.calculateRiskScore() == 20, "Risk score remains uncorrupted at 20");

        // Now advance cleanly to 15:05:00. If state was corrupted by 02:00:00, this would fail or miscalculate!
        analyzer.updateCurrentTime("15:05:00");
        assertTest(analyzer.getLatestTimestamp() == "15:05:00", "Subsequent chronological advance to 15:05:00 succeeds cleanly");
        assertTest(analyzer.getEventCount() == 1, "Original event at 15:00:00 still active (5 mins elapsed <= 10m)");

        // Add a valid subsequent event at 15:06:00
        analyzer.addEvent(Event(1803, "Missed Check-in", "Sector 8", "15:06:00"));
        assertTest(analyzer.getEventCount() == 2, "Subsequent event at 15:06:00 accepted");
        assertTest(analyzer.calculateRiskScore() == 65, "Risk score accurately reflects compound pattern after rejected backward jumps");
    }

    // ---------------------------------------------------------
    // TEST 19: Missed-Check-in Integration Timing Consistency
    // ---------------------------------------------------------
    cout << "\n--- Test 19: Missed-Check-in Integration Timing Consistency ---" << endl;
    {
        RiskAnalyzer analyzer(10);

        // Advance time to 14:15:00
        analyzer.updateCurrentTime("14:15:00");
        assertTest(analyzer.getLatestTimestamp() == "14:15:00", "Timeline advanced to 14:15:00");

        // Simulate CheckInManager generating a missed event at the EXACT detection time 14:15:00
        // SafetySystem immediately passes this event to RiskAnalyzer. It must NOT be rejected as out-of-order!
        analyzer.addEvent(Event(8000, "Missed Check-in", "Main Gate", "14:15:00"));
        assertTest(analyzer.getEventCount() == 1, "Event timestamp matching current timeline (14:15:00) accepted");

        // Simulate a second missed check-in generated at the same simulated time
        analyzer.addEvent(Event(8001, "Missed Check-in", "Science Hall", "14:15:00"));
        assertTest(analyzer.getEventCount() == 2, "Second concurrent event at exact same timestamp (14:15:00) accepted");
        assertTest(analyzer.calculateRiskScore() == 60, "Multiple missed check-in pattern formed at same timestamp (Score: 60)");

        // Verify that an event with timestamp 1 second earlier (14:14:59) is strictly rejected as out-of-order
        analyzer.addEvent(Event(8002, "Route Deviation", "Old Path", "14:14:59"));
        assertTest(analyzer.getEventCount() == 2, "Event earlier than current timeline (14:14:59 < 14:15:00) rejected");

        // Advance time to 14:26:00 -> both events (11 mins elapsed) should expire
        analyzer.updateCurrentTime("14:26:00");
        assertTest(analyzer.getEventCount() == 0, "Both events expire after 11 minutes (exceeding 10m window)");
        assertTest(analyzer.calculateRiskScore() == 0, "Risk score drops to 0 after expiration");
    }

    // ---------------------------------------------------------
    // SUMMARY
    // ---------------------------------------------------------
    cout << "\n====================================================" << endl;
    cout << "RISKANALYZER TEST SUMMARY: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << endl;
    cout << "====================================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}
