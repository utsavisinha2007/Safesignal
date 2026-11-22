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
                return 0; // Safely return 0 on end-of-file
            }
            cout << "  [Error] Invalid numeric input. Please enter an integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Helper function to safely read a line of text
string getStringInput(const string& prompt, bool allowEmpty = false) {
    string value;
    cout << prompt;
    if (!getline(cin, value)) {
        if (cin.eof()) return "";
    }
    while (value.empty() && !allowEmpty) {
        if (cin.eof()) return "";
        cout << "  [Error] Input cannot be empty. Please re-enter: ";
        if (!getline(cin, value)) {
            if (cin.eof()) return "";
        }
    }
    return value;
}

// ============================================================================
// USER MODE (Friendly, Non-Technical Interface)
// ============================================================================

void handleUserJourney(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "               START OR MANAGE A SAFE JOURNEY             " << endl;
    cout << "==========================================================" << endl;
    cout << "  SafeSignal monitors your route checkpoints. If you      " << endl;
    cout << "  deviate, miss a check-in, or trigger distress, alerts   " << endl;
    cout << "  are automatically sent to your emergency contacts.      " << endl;
    cout << "  (Note: GPS tracking and alerts are simulated)           " << endl;
    cout << "==========================================================" << endl;

    if (safetySystem.hasActiveJourney()) {
        cout << "\n[Status] You currently have an active journey in progress!\n" << endl;
        safetySystem.displayJourneyDetails();

        cout << "\nJourney Options:" << endl;
        cout << "  1. Update Current Location (Simulated GPS step)" << endl;
        cout << "  2. End Journey Safely (Arrived at Destination)" << endl;
        cout << "  3. Return to Main Menu" << endl;

        int jChoice = getIntInput("Select an option (1-3): ");
        if (jChoice == 1) {
            cout << "\n--- Update Your Location ---" << endl;
            string loc = getStringInput("Enter your current location (e.g., Campus Library): ");
            string timeStr = getStringInput("Enter current time (HH:MM or HH:MM:SS): ");
            safetySystem.updateJourneyLocation(loc, timeStr);
        } else if (jChoice == 2) {
            cout << "\n--- Ending Active Journey ---" << endl;
            safetySystem.endJourney();
            cout << "  Glad you arrived safely! Your journey and associated check-ins have ended.\n" << endl;
        }
        return;
    }

    // Starting a new journey
    cout << "\n--- Plan a New Safe Journey ---" << endl;
    string startLoc = getStringInput("Where are you starting from? (e.g. Student Union): ");
    string dest = getStringInput("Where is your destination? (e.g. Dormitory B): ");
    string eta = getStringInput("Estimated Arrival Time (HH:MM or HH:MM:SS): ");

    int numCheckpoints = 0;
    while (numCheckpoints <= 0) {
        numCheckpoints = getIntInput("How many route checkpoints/milestones along the way? (e.g. 2): ");
        if (numCheckpoints <= 0) {
            cout << "  [Notice] Please enter at least 1 checkpoint along your path.\n";
        }
    }

    vector<string> route;
    cout << "Enter the " << numCheckpoints << " planned route checkpoints in order:" << endl;
    for (int i = 0; i < numCheckpoints; i++) {
        string cp = getStringInput("  Checkpoint #" + to_string(i + 1) + ": ");
        route.push_back(cp);
    }

    bool started = safetySystem.startJourney(startLoc, dest, eta, route);
    if (started) {
        cout << "\n[Success] Your journey has started! SafeSignal is now protecting your route." << endl;
        cout << "Would you like to schedule an initial safety check-in for this trip? (1 for Yes, 2 for No): ";
        int checkInChoice = getIntInput("");
        if (checkInChoice == 1) {
            string schedTime = getStringInput("Enter check-in time (HH:MM or HH:MM:SS): ");
            string desc = getStringInput("Enter note/purpose (e.g. Midpoint walk check): ");
            string loc = getStringInput("Enter expected location: ", true);
            if (loc.empty()) loc = startLoc;
            int cid = safetySystem.scheduleCheckIn(schedTime, desc, loc);
            if (cid > 0) {
                cout << "  [Success] Safety Check-in #" << cid << " scheduled for " << schedTime << ".\n" << endl;
            }
        }
    }
}

void handleUserContacts(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "                  MY EMERGENCY CONTACTS                   " << endl;
    cout << "==========================================================" << endl;
    cout << "  Your contacts are alerted based on priority tiers:      " << endl;
    cout << "  - Priority 1 (Primary): Family / Roommate (Notified 1st)" << endl;
    cout << "  - Priority 2 (Secondary): Trusted Friends (Notified 2nd)" << endl;
    cout << "  - Priority 3 (Security): Campus Police / Emergency Desk " << endl;
    cout << "==========================================================" << endl;

    safetySystem.displayRegisteredContacts();

    cout << "\nContact Options:" << endl;
    cout << "  1. Add a New Emergency Contact" << endl;
    cout << "  2. Return to Main Menu" << endl;

    int choice = getIntInput("Select an option (1-2): ");
    if (choice == 1) {
        cout << "\n--- Add New Emergency Contact ---" << endl;
        int id = getIntInput("Enter Contact ID (e.g. 101): ");
        string name = getStringInput("Enter Contact Name: ");
        string phone = getStringInput("Enter Phone Number: ");
        string relation = getStringInput("Enter Relationship (e.g. Mother, Roommate, Campus Police): ");

        cout << "Select Priority Level:" << endl;
        cout << "  1. Primary Contact (Notified immediately upon emergency)" << endl;
        cout << "  2. Secondary Contact (Notified if primary does not acknowledge)" << endl;
        cout << "  3. Emergency Services / Campus Security" << endl;
        int priority = 0;
        while (priority < 1 || priority > 3) {
            priority = getIntInput("Enter Priority (1-3): ");
            if (priority < 1 || priority > 3) {
                cout << "  [Notice] Priority must be 1, 2, or 3.\n";
            }
        }

        Contact newContact(id, name, phone, relation, priority);
        safetySystem.registerContact(newContact);
        cout << "\n[Success] Emergency contact \"" << name << "\" successfully added!\n" << endl;
    }
}

void handleUserCheckIns(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "                     SAFETY CHECK-INS                     " << endl;
    cout << "==========================================================" << endl;
    cout << "  Check-ins ensure someone knows you are safe at set      " << endl;
    cout << "  times. If a check-in is missed (plus 5-min grace period)" << endl;
    cout << "  an automated safety verification is triggered.          " << endl;
    cout << "==========================================================" << endl;

    safetySystem.displayScheduledCheckIns();

    cout << "\nCheck-in Options:" << endl;
    cout << "  1. Confirm / Complete a Check-in (\"I'm Safe!\")" << endl;
    cout << "  2. Schedule a New Check-in (During Active Journey)" << endl;
    cout << "  3. Advance Simulated Clock (Simulate Time Passing)" << endl;
    cout << "  4. View Check-in Statistics" << endl;
    cout << "  5. Return to Main Menu" << endl;

    int choice = getIntInput("Select an option (1-5): ");
    switch (choice) {
        case 1: {
            cout << "\n--- Confirm Safety Check-in ---" << endl;
            int cid = getIntInput("Enter Check-in ID to confirm: ");
            bool ok = safetySystem.completeCheckIn(cid);
            if (ok) {
                cout << "  [Success] Check-in #" << cid << " confirmed! Your contacts know you are safe.\n" << endl;
            }
            break;
        }
        case 2: {
            if (!safetySystem.hasActiveJourney()) {
                cout << "\n[Notice] Check-ins can only be scheduled during an active journey." << endl;
                cout << "Please start a journey first from Option 1 in the Main Menu.\n" << endl;
                break;
            }
            cout << "\n--- Schedule a Safety Check-in ---" << endl;
            string schedTime = getStringInput("Enter Check-in Time (HH:MM or HH:MM:SS): ");
            string desc = getStringInput("Enter Note / Purpose (e.g. Crossing Park): ");
            string loc = getStringInput("Enter Expected Location (or leave blank): ", true);
            if (loc.empty()) loc = "Current Route";
            int cid = safetySystem.scheduleCheckIn(schedTime, desc, loc);
            if (cid > 0) {
                cout << "  [Success] Check-in #" << cid << " scheduled for " << schedTime << ".\n" << endl;
            }
            break;
        }
        case 3: {
            cout << "\n--- Advance Simulated Time ---" << endl;
            cout << "Simulating time lets you test deadline triggers and grace periods.\n";
            string currTime = getStringInput("Enter Simulated Current Time (HH:MM or HH:MM:SS): ");
            safetySystem.processCurrentTimeUpdate(currTime);
            break;
        }
        case 4: {
            safetySystem.displayCheckInStatistics();
            break;
        }
        default:
            break;
    }
}

void handleUserSafetyStatus(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "                    MY SAFETY STATUS                      " << endl;
    cout << "==========================================================" << endl;

    // Check if safety verification is pending
    if (safetySystem.hasActiveVerification()) {
        cout << "\n  ********************************************************" << endl;
        cout << "  🚨 ACTION NEEDED: SAFETY VERIFICATION REQUEST PENDING!  " << endl;
        cout << "  ********************************************************" << endl;
        cout << "  SafeSignal detected an unusual safety situation and is   " << endl;
        cout << "  checking on your safety. Please respond below!           " << endl;
        cout << "  ********************************************************\n" << endl;

        safetySystem.displayPendingVerifications();

        cout << "\nVerification Response:" << endl;
        cout << "  1. I Am Safe (False alarm / Cancel alert)" << endl;
        cout << "  2. I Need Help! (Trigger immediate emergency dispatch)" << endl;
        cout << "  3. Return to Menu" << endl;

        int vChoice = getIntInput("Select your response (1-3): ");
        if (vChoice == 1) {
            safetySystem.confirmUserSafe();
        } else if (vChoice == 2) {
            safetySystem.requestImmediateHelp();
        }
        return;
    }

    // Normal safety overview
    int riskScore = safetySystem.getRiskAnalyzer().calculateRiskScore();
    string riskLevel = safetySystem.getRiskAnalyzer().getRiskLevel();

    cout << "\nCurrent Safety Indicator:" << endl;
    if (riskScore == 0) {
        cout << "  [🟢 ALL CLEAR] SafeSignal is actively monitoring. No safety risks detected." << endl;
    } else if (riskScore < 20) {
        cout << "  [🟢 NORMAL] Risk Score: " << riskScore << "/100 (" << riskLevel << "). Everything appears normal." << endl;
    } else if (riskScore < 50) {
        cout << "  [🟡 CAUTION] Risk Score: " << riskScore << "/100 (" << riskLevel << "). Minor safety events recorded." << endl;
    } else {
        cout << "  [🔴 ELEVATED RISK] Risk Score: " << riskScore << "/100 (" << riskLevel << "). Safety alert threshold reached!" << endl;
    }

    cout << "\nSystem Summary:" << endl;
    cout << "  Active Journey:      " << (safetySystem.hasActiveJourney() ? "UNDERWAY" : "None") << endl;
    cout << "  Emergency Contacts:  " << safetySystem.getResponseManager().getContactCount() << " registered" << endl;
    cout << "  Active Incidents:    " << safetySystem.getResponseManager().getActiveIncidentCount() << " in progress" << endl;

    if (safetySystem.getResponseManager().hasActiveIncidents()) {
        cout << "\nActive Emergency Details:" << endl;
        safetySystem.displayActiveIncidents();
    }

    cout << "==========================================================\n" << endl;
}

void handleUserEmergencyHelp(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "               🚨 REQUEST EMERGENCY HELP 🚨               " << endl;
    cout << "==========================================================" << endl;
    cout << "  This will immediately trigger an emergency alert to     " << endl;
    cout << "  your designated emergency contacts and campus security  " << endl;
    cout << "  based on priority tiers!                                " << endl;
    cout << "  (Simulated dispatch for testing and student safety demo)" << endl;
    cout << "==========================================================" << endl;

    cout << "\nAre you sure you want to request immediate emergency assistance?" << endl;
    cout << "  1. YES — SEND EMERGENCY HELP NOW" << endl;
    cout << "  2. No — Cancel and return to menu" << endl;

    int choice = getIntInput("Enter choice (1 or 2): ");
    if (choice == 1) {
        cout << "\n[ALERT] Activating One-Touch Emergency Dispatch..." << endl;
        if (safetySystem.hasActiveVerification()) {
            safetySystem.requestImmediateHelp();
        } else {
            // Log a direct SOS event which immediately escalates to emergency dispatch
            string loc = "Current Location";
            string timeStr = "12:00:00";
            if (safetySystem.hasActiveJourney()) {
                loc = safetySystem.getJourneyManager().getNextExpectedCheckpoint();
                if (loc.empty()) loc = safetySystem.getJourneyManager().getDestination();
            }
            Event sosEvent(9999, "SOS", loc, timeStr);
            safetySystem.logEvent(sosEvent);

            // If a verification request was created, immediately request help for it
            if (safetySystem.hasActiveVerification()) {
                safetySystem.requestImmediateHelp();
            }
        }
        cout << "\n[Notice] Emergency alert has been dispatched to your priority contacts!\n" << endl;
    } else {
        cout << "\nEmergency request cancelled. Returning to main menu.\n" << endl;
    }
}

void handleUserActivityHistory(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "                 ACTIVITY & SAFETY HISTORY                " << endl;
    cout << "==========================================================" << endl;
    cout << "  1. View Recent Safety Events (Sliding Window Log)" << endl;
    cout << "  2. View Safety Check-in Statistics & Status" << endl;
    cout << "  3. View Verification Request Statistics" << endl;
    cout << "  4. View Emergency Incidents & Audit Trail" << endl;
    cout << "  5. Return to Main Menu" << endl;

    int choice = getIntInput("Select an option (1-5): ");
    switch (choice) {
        case 1:
            safetySystem.displayRecentEvents();
            break;
        case 2:
            safetySystem.displayCheckInStatistics();
            break;
        case 3:
            safetySystem.displayVerificationStatistics();
            break;
        case 4:
            safetySystem.displayIncidentHistory();
            safetySystem.displayIncidentStatistics();
            break;
        default:
            break;
    }
}

void handleUserSettings(SafetySystem& safetySystem) {
    cout << "\n==========================================================" << endl;
    cout << "                SETTINGS & SYSTEM CONTROLS                " << endl;
    cout << "==========================================================" << endl;
    cout << "  Active Safety Parameters:" << endl;
    cout << "  - Safety Check-in Grace Period: 5 minutes" << endl;
    cout << "  - Safety Verification Timeout:  3 minutes" << endl;
    cout << "  - Contact Ack Timeout (Tiered): 2 minutes" << endl;
    cout << "  - Risk Analysis Time Window:   10 minutes" << endl;
    cout << "==========================================================" << endl;

    cout << "\nControls:" << endl;
    cout << "  1. Advance Simulated Clock (Simulate Time Passing)" << endl;
    cout << "  2. Simulate Safety Sensor Event (Fall, Impact, Motion)" << endl;
    cout << "  3. View Complete Technical System Status" << endl;
    cout << "  4. Return to Main Menu" << endl;

    int choice = getIntInput("Select an option (1-4): ");
    switch (choice) {
        case 1: {
            string currTime = getStringInput("Enter Simulated Current Time (HH:MM or HH:MM:SS): ");
            safetySystem.processCurrentTimeUpdate(currTime);
            break;
        }
        case 2: {
            cout << "\nSelect sensor event to simulate:" << endl;
            cout << "  1. Fall Detected (High Severity)" << endl;
            cout << "  2. Impact Detected (High Severity)" << endl;
            cout << "  3. Motion Alert (Minor Event)" << endl;
            cout << "  4. Custom Event" << endl;
            int sChoice = getIntInput("Enter choice (1-4): ");
            string type = "Fall Detected";
            if (sChoice == 2) type = "Impact";
            else if (sChoice == 3) type = "Motion Alert";
            else if (sChoice == 4) type = getStringInput("Enter event type: ");

            string loc = getStringInput("Enter location: ");
            string timeStr = getStringInput("Enter timestamp (HH:MM or HH:MM:SS): ");
            int id = getIntInput("Enter Event ID (e.g. 601): ");
            Event ev(id, type, loc, timeStr);
            safetySystem.logEvent(ev);
            break;
        }
        case 3:
            safetySystem.displaySystemStatus();
            break;
        default:
            break;
    }
}

void runUserMode(SafetySystem& safetySystem) {
    bool inUserMode = true;
    while (inUserMode && !cin.eof()) {
        cout << "\n=================== SAFESIGNAL USER DASHBOARD ===================" << endl;
        cout << "  1. Start a Safe Journey       (Route tracking & checkpoints)" << endl;
        cout << "  2. My Emergency Contacts      (Manage primary & secondary contacts)" << endl;
        cout << "  3. Safety Check-ins           (Confirm check-ins & schedule alerts)" << endl;
        cout << "  4. My Safety Status           (Real-time safety overview & alarms)" << endl;
        cout << "  5. Request Emergency Help     (One-Touch SOS distress trigger)" << endl;
        cout << "  6. Activity History           (Timeline, past events & incidents)" << endl;
        cout << "  7. Settings & Controls        (Clock simulation & sensor testing)" << endl;
        cout << "  8. Exit User Mode             (Return to mode selector / exit)" << endl;
        cout << "=================================================================" << endl;

        int choice = getIntInput("Enter your choice (1-8): ");

        switch (choice) {
            case 1:
                handleUserJourney(safetySystem);
                break;
            case 2:
                handleUserContacts(safetySystem);
                break;
            case 3:
                handleUserCheckIns(safetySystem);
                break;
            case 4:
                handleUserSafetyStatus(safetySystem);
                break;
            case 5:
                handleUserEmergencyHelp(safetySystem);
                break;
            case 6:
                handleUserActivityHistory(safetySystem);
                break;
            case 7:
                handleUserSettings(safetySystem);
                break;
            case 8:
                cout << "\nExiting User Mode..." << endl;
                inUserMode = false;
                break;
            default:
                cout << "\n[Notice] Please select an option between 1 and 8.\n";
                break;
        }
    }
}

// ============================================================================
// DEMONSTRATION MODE (Full 30-Option Technical System Interface)
// ============================================================================

void runDemonstrationMode(SafetySystem& safetySystem) {
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
                string loc = getStringInput("Enter Expected Location (or press Enter for 'Unknown'): ", true);
                if (loc.empty()) loc = "Unknown";

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
                cout << "\nExiting Demonstration Mode..." << endl;
                running = false;
                break;
            }

            default: {
                cout << "\n[Error] Invalid selection (" << choice << "). Please choose an option between 1 and 30.\n" << endl;
                break;
            }
        }
    }
}

// ============================================================================
// MAIN ENTRY POINT (Startup Mode Selector)
// ============================================================================

int main() {
    // Instantiate the primary SafetySystem coordinator (shared across all modes)
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

    bool appRunning = true;
    while (appRunning && !cin.eof()) {
        cout << "=================== SAFESIGNAL STARTUP MENU ===================" << endl;
        cout << "  Please select an operation mode:" << endl;
        cout << "  1. User Mode (Default - Intuitive, Friendly Interface)" << endl;
        cout << "  2. Demonstration Mode (Complete Technical 30-Option System)" << endl;
        cout << "  3. Exit Application" << endl;
        cout << "===============================================================" << endl;

        int modeChoice = getIntInput("Enter mode selection (1-3) [Default: 1]: ");

        if (modeChoice == 1) {
            runUserMode(safetySystem);
        } else if (modeChoice == 2) {
            runDemonstrationMode(safetySystem);
        } else if (modeChoice == 3 || cin.eof()) {
            cout << "\nThank you for using SafeSignal. Shutting down system safely. Goodbye!" << endl;
            appRunning = false;
        } else {
            cout << "\n[Notice] Please select 1 for User Mode, 2 for Demonstration Mode, or 3 to Exit.\n" << endl;
        }
    }

    return 0;
}
