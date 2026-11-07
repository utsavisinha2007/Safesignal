#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "include/Contact.h"
#include "include/Event.h"
#include "include/Incident.h"
#include "include/SafetySystem.h"

using namespace std;

// Helper function to safely read an integer from standard input
int getIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear remaining newline
            return value;
        } else {
            if (cin.eof()) {
                return 30; // Safely trigger exit on end-of-file
            }
            cout << "  [Error] Invalid numeric input. Please enter an integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Helper function to safely read a line of text
string getStringInput(const string& prompt) {
    string value;
    cout << prompt;
    if (!getline(cin, value)) {
        if (cin.eof()) return "";
    }
    while (value.empty()) {
        if (cin.eof()) return "";
        cout << "  [Error] Input cannot be empty. Please re-enter: ";
        if (!getline(cin, value)) {
            if (cin.eof()) return "";
        }
    }
    return value;
}

int main() {
    // Instantiate the primary SafetySystem coordinator
    SafetySystem safetySystem;

    // Display Welcome Screen
    cout << "==========================================================" << endl;
    cout << "       WELCOME TO SAFESIGNAL - SAFETY MANAGEMENT SYSTEM   " << endl;
    cout << "    A C++ Data Structures & Algorithms Safety Platform    " << endl;
    cout << "==========================================================" << endl;
    cout << "  Features: Queue-based Event Analysis, Priority Queue    " << endl;
    cout << "  Emergency Triage, Vector-based Contact Directory,       " << endl;
    cout << "  Journey Route Deviation Monitoring, Scheduled Safety    " << endl;
    cout << "  Check-in Tracking, and Verification Timeout Management. " << endl;
    cout << "  (Note: Emergency dispatches, location, and time updates " << endl;
    cout << "   are simulated for demonstration and testing)           " << endl;
    cout << "==========================================================\n" << endl;

    int choice = 0;
    bool running = true;

    while (running && !cin.eof()) {
        cout << "================== SAFESIGNAL MAIN MENU ==================" << endl;
        cout << "  --- Core Safety & Incident Management ---" << endl;
        cout << "  1.  Register Emergency Contact" << endl;
        cout << "  2.  Record Safety Event" << endl;
        cout << "  3.  Report Manual Incident" << endl;
        cout << "  4.  View Current Risk Analysis" << endl;
        cout << "  5.  View Registered Contacts" << endl;
        cout << "  6.  View Recent Events" << endl;
        cout << "  7.  View Pending Emergencies" << endl;
        cout << "  8.  Process Next Emergency" << endl;
        cout << "  9.  Display Complete System Status" << endl;
        cout << "  --- Journey & Route Deviation Monitoring ---" << endl;
        cout << "  10. Start a New Journey" << endl;
        cout << "  11. Submit Simulated Location Update" << endl;
        cout << "  12. View Journey Details" << endl;
        cout << "  13. End Current Journey" << endl;
        cout << "  --- Scheduled Safety Check-in Tracking ---" << endl;
        cout << "  14. Schedule a Safety Check-in" << endl;
        cout << "  15. Confirm / Complete a Check-in" << endl;
        cout << "  16. Submit Simulated Current-Time Update" << endl;
        cout << "  17. View Scheduled Check-ins" << endl;
        cout << "  18. View Check-in Statistics" << endl;
        cout << "  --- Safety Verification & Response (Phase 4) ---" << endl;
        cout << "  19. Confirm User Safety (Avert Escalation)" << endl;
        cout << "  20. Request Immediate Help" << endl;
        cout << "  21. View Pending Verification Requests" << endl;
        cout << "  22. View Verification Statistics" << endl;
        cout << "  --- Incident Lifecycle & Priority Dispatch (Phase 4.4 & 4.5) ---" << endl;
        cout << "  23. View Active Emergencies & Incidents" << endl;
        cout << "  24. Acknowledge Incident (by Emergency Contact)" << endl;
        cout << "  25. Resolve Incident" << endl;
        cout << "  26. Cancel Incident (False Alarm)" << endl;
        cout << "  27. View Complete Incident History" << endl;
        cout << "  28. View Dispatch & Notification Audit Trail" << endl;
        cout << "  29. View Incident & Response Statistics" << endl;
        cout << "  --- System ---" << endl;
        cout << "  30. Exit System" << endl;
        cout << "==========================================================" << endl;

        choice = getIntInput("Enter your choice (1-30): ");

        switch (choice) {
            case 1: {
                // Register Emergency Contact
                cout << "\n--- [1] Register New Emergency Contact ---" << endl;
                int id = getIntInput("Enter Contact ID (e.g. 101): ");
                string name = getStringInput("Enter Contact Name: ");
                string phone = getStringInput("Enter Phone Number: ");
                string relation = getStringInput("Enter Relationship (e.g. Family, Friend, Campus Security): ");

                cout << "Select Contact Priority Level:" << endl;
                cout << "  1. Priority 1 (Primary Contact)" << endl;
                cout << "  2. Priority 2 (Secondary Contact)" << endl;
                cout << "  3. Priority 3 (Emergency / Campus Security Contact)" << endl;
                int priority = 0;
                while (priority < 1 || priority > 3) {
                    priority = getIntInput("Enter Priority Level (1-3): ");
                    if (priority < 1 || priority > 3) {
                        cout << "  [Error] Priority level must be 1, 2, or 3. Please try again.\n";
                    }
                }

                Contact newContact(id, name, phone, relation, priority);
                safetySystem.registerContact(newContact);
                cout << "Contact successfully registered.\n" << endl;
                break;
            }

            case 2: {
                // Record Safety Event
                cout << "\n--- [2] Record New Safety Event ---" << endl;
                cout << "Common event types: 'SOS', 'Fall Detected', 'Route Deviation', 'Missed Check-in', 'Motion Alert'" << endl;
                int id = getIntInput("Enter Event ID (e.g. 501): ");
                string type = getStringInput("Enter Event Type: ");
                string loc = getStringInput("Enter Location (e.g. Campus Quad, Main St): ");
                string time = getStringInput("Enter Timestamp (e.g. 14:35:00): ");

                Event newEvent(id, type, loc, time);
                safetySystem.logEvent(newEvent);
                break;
            }

            case 3: {
                // Report Manual Incident
                cout << "\n--- [3] Report Manual Incident ---" << endl;
                int id = getIntInput("Enter Incident ID (e.g. 1001): ");
                string desc = getStringInput("Enter Incident Description: ");
                string loc = getStringInput("Enter Incident Location: ");
                int sev = getIntInput("Enter Severity Level (1 = Minor to 5 = Critical): ");

                Incident newIncident(id, desc, loc, sev);
                safetySystem.reportIncident(newIncident);
                break;
            }

            case 4: {
                // View Current Risk Analysis
                safetySystem.displayRiskAnalysis();
                break;
            }

            case 5: {
                // View Registered Contacts
                safetySystem.displayRegisteredContacts();
                break;
            }

            case 6: {
                // View Recent Events in sliding queue
                safetySystem.displayRecentEvents();
                break;
            }

            case 7: {
                // View Pending Emergencies in priority queue
                safetySystem.displayPendingEmergencies();
                break;
            }

            case 8: {
                // Process Next Emergency
                safetySystem.processNextEmergency();
                break;
            }

            case 9: {
                // Display Complete System Status
                safetySystem.displaySystemStatus();
                break;
            }

            case 10: {
                // Start a New Journey
                cout << "\n--- [10] Start a New Journey ---" << endl;
                string startLoc = getStringInput("Enter Starting Location (e.g. Dormitory A): ");
                string dest = getStringInput("Enter Destination (e.g. Science Library): ");
                string eta = getStringInput("Enter Expected Arrival Time (e.g. 15:45:00): ");

                int numCheckpoints = 0;
                while (numCheckpoints <= 0) {
                    numCheckpoints = getIntInput("How many route checkpoints are planned? (Must be > 0): ");
                    if (numCheckpoints <= 0) {
                        cout << "  [Error] Number of checkpoints must be a positive integer.\n";
                    }
                }

                vector<string> route;
                cout << "Enter the " << numCheckpoints << " planned route checkpoints in order:" << endl;
                for (int i = 0; i < numCheckpoints; i++) {
                    string cp = getStringInput("  Checkpoint #" + to_string(i + 1) + ": ");
                    route.push_back(cp);
                }

                safetySystem.startJourney(startLoc, dest, eta, route);
                break;
            }

            case 11: {
                // Submit Simulated Location Update
                cout << "\n--- [11] Submit Simulated Location Update ---" << endl;
                string currLoc = getStringInput("Enter Simulated Current Location: ");
                string timestamp = getStringInput("Enter Timestamp (e.g. 15:15:00): ");

                safetySystem.updateJourneyLocation(currLoc, timestamp);
                break;
            }

            case 12: {
                // View Journey Details
                safetySystem.displayJourneyDetails();
                break;
            }

            case 13: {
                // End Current Journey
                safetySystem.endJourney();
                break;
            }

            case 14: {
                // Schedule a Safety Check-in
                cout << "\n--- [14] Schedule a Safety Check-in ---" << endl;
                string schedTime = getStringInput("Enter Scheduled Check-in Time (HH:MM or HH:MM:SS): ");
                string desc = getStringInput("Enter Check-in Description/Note: ");
                string loc = getStringInput("Enter Expected Location (or press Enter for 'Unknown'): ");

                int checkInID = safetySystem.scheduleCheckIn(schedTime, desc, loc);
                if (checkInID > 0) {
                    cout << "Check-in #" << checkInID << " successfully scheduled.\n" << endl;
                }
                break;
            }

            case 15: {
                // Confirm / Complete a Check-in
                cout << "\n--- [15] Confirm / Complete a Safety Check-in ---" << endl;
                int checkInID = getIntInput("Enter Check-in ID to confirm: ");
                safetySystem.completeCheckIn(checkInID);
                break;
            }

            case 16: {
                // Submit Simulated Current-Time Update
                cout << "\n--- [16] Submit Simulated Current-Time Update ---" << endl;
                string currTime = getStringInput("Enter Simulated Current Time (HH:MM or HH:MM:SS): ");
                safetySystem.processCurrentTimeUpdate(currTime);
                break;
            }

            case 17: {
                // View Scheduled Check-ins
                safetySystem.displayScheduledCheckIns();
                break;
            }

            case 18: {
                // View Check-in Statistics
                safetySystem.displayCheckInStatistics();
                break;
            }

            case 19: {
                // Confirm User Safety
                cout << "\n--- [19] Confirm User Safety ---" << endl;
                if (!safetySystem.hasActiveVerification()) {
                    cout << "  (No active safety verification requests pending)\n" << endl;
                } else {
                    safetySystem.displayPendingVerifications();
                    int reqID = getIntInput("Enter Request ID to confirm safe (or 0 for active request): ");
                    safetySystem.confirmUserSafe(reqID);
                }
                break;
            }

            case 20: {
                // Request Immediate Help
                cout << "\n--- [20] Request Immediate Help ---" << endl;
                if (!safetySystem.hasActiveVerification()) {
                    cout << "  (No active safety verification requests pending)\n" << endl;
                } else {
                    safetySystem.displayPendingVerifications();
                    int reqID = getIntInput("Enter Request ID to request help (or 0 for active request): ");
                    safetySystem.requestImmediateHelp(reqID);
                }
                break;
            }

            case 21: {
                // View Pending Verification Requests
                cout << "\n--- [21] View Pending Verification Requests ---" << endl;
                safetySystem.displayPendingVerifications();
                break;
            }

            case 22: {
                // View Verification Statistics
                cout << "\n--- [22] View Verification Statistics ---" << endl;
                safetySystem.displayVerificationStatistics();
                break;
            }

            case 23: {
                // View Active Emergencies & Incidents
                cout << "\n--- [23] View Active Emergencies & Incidents ---" << endl;
                safetySystem.displayActiveIncidents();
                break;
            }

            case 24: {
                // Acknowledge Incident (by Emergency Contact)
                cout << "\n--- [24] Acknowledge Incident (Emergency Contact) ---" << endl;
                safetySystem.displayActiveIncidents();
                int incID = getIntInput("Enter Incident ID to acknowledge: ");
                int contactID = getIntInput("Enter Contact ID acknowledging the incident: ");
                string timeStr = getStringInput("Enter Acknowledgement Timestamp (HH:MM or HH:MM:SS): ");
                safetySystem.acknowledgeIncident(incID, contactID, timeStr);
                break;
            }

            case 25: {
                // Resolve Incident
                cout << "\n--- [25] Resolve Incident ---" << endl;
                safetySystem.displayActiveIncidents();
                int incID = getIntInput("Enter Incident ID to resolve: ");
                string notes = getStringInput("Enter Resolution Notes (e.g. User confirmed safe by responder): ");
                string timeStr = getStringInput("Enter Resolution Timestamp (HH:MM or HH:MM:SS): ");
                safetySystem.resolveIncident(incID, notes, timeStr);
                break;
            }

            case 26: {
                // Cancel Incident (False Alarm)
                cout << "\n--- [26] Cancel Incident (False Alarm) ---" << endl;
                safetySystem.displayActiveIncidents();
                int incID = getIntInput("Enter Incident ID to cancel: ");
                string reason = getStringInput("Enter Cancellation Reason (e.g. Accidental distress trigger): ");
                string timeStr = getStringInput("Enter Cancellation Timestamp (HH:MM or HH:MM:SS): ");
                safetySystem.cancelIncident(incID, reason, timeStr);
                break;
            }

            case 27: {
                // View Complete Incident History
                cout << "\n--- [27] View Complete Incident History ---" << endl;
                safetySystem.displayIncidentHistory();
                break;
            }

            case 28: {
                // View Dispatch & Notification Audit Trail
                cout << "\n--- [28] View Dispatch & Notification Audit Trail ---" << endl;
                int incID = getIntInput("Enter Incident ID to view notifications (or 0 for all): ");
                safetySystem.displayNotificationHistory(incID);
                break;
            }

            case 29: {
                // View Incident & Response Statistics
                cout << "\n--- [29] View Incident & Response Statistics ---" << endl;
                safetySystem.displayIncidentStatistics();
                break;
            }

            case 30: {
                // Exit
                cout << "\nThank you for using SafeSignal. Shutting down system safely. Goodbye!" << endl;
                running = false;
                break;
            }

            default: {
                cout << "\n[Error] Invalid selection (" << choice << "). Please choose an option between 1 and 30.\n" << endl;
                break;
            }
        }
    }

    return 0;
}

