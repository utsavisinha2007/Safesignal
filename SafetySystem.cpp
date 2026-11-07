#include "SafetySystem.h"

using namespace std;

// Constructor
SafetySystem::SafetySystem() {
    activeRiskEscalated = false;
    activeVerificationEpisode = false;
    autoIncidentCounter = 9000; // Auto-generated incidents use IDs starting at 9001
}

// Destructor
SafetySystem::~SafetySystem() {
    // Member objects clean up automatically
}

// Logs an incoming event and evaluates risk for safety verification
void SafetySystem::logEvent(const Event& event) {
    cout << "\n--- [SafetySystem] Ingesting New Event ---" << endl;
    riskAnalyzer.addEvent(event);

    // Evaluate risk state (Concerning / High Risk when score >= 50)
    if (riskAnalyzer.isRiskHigh()) {
        if (!activeVerificationEpisode) {
            int currentScore = riskAnalyzer.calculateRiskScore();
            string currentLevel = riskAnalyzer.getRiskLevel();

            cout << "  [SafetySystem ALERT] Risk threshold reached (Score: "
                 << currentScore << ", Level: " << currentLevel
                 << ")! Initiating Safety Verification..." << endl;

            int reqID = verificationManager.createVerificationRequest(
                currentScore,
                currentLevel,
                event.getLocation(),
                event.getTimestamp()
            );

            if (reqID != -1) {
                activeVerificationEpisode = true;
                cout << "  [SafetySystem] Safety Verification Request #" << reqID
                     << " created. Awaiting user response before emergency escalation." << endl;
            }
        } else {
            cout << "  [SafetySystem Notice] High-risk state persists (Score: "
                 << riskAnalyzer.calculateRiskScore()
                 << "), but safety verification has already been initiated for this episode." << endl;
        }
    } else {
        // If risk drops below threshold and no active pending verification remains, reset the episode latch
        if (activeVerificationEpisode && !verificationManager.hasActivePendingRequest()) {
            activeVerificationEpisode = false;
            cout << "  [SafetySystem Info] Risk levels have normalized (Score: "
                 << riskAnalyzer.calculateRiskScore() << "). Verification episode reset." << endl;
        }
    }
}

// Reports a manual incident directly into the response manager
void SafetySystem::reportIncident(const Incident& incident) {
    cout << "\n--- [SafetySystem] Registering Manual Incident ---" << endl;
    responseManager.addIncident(incident);
}

// Registers an emergency contact into the response manager
void SafetySystem::registerContact(const Contact& contact) {
    cout << "\n--- [SafetySystem] Registering Contact ---" << endl;
    responseManager.registerContact(contact);
}

// Dispatches the highest-priority emergency waiting in the response queue
void SafetySystem::processNextEmergency() {
    cout << "\n--- [SafetySystem] Processing Next Emergency ---" << endl;
    responseManager.processNextIncident();
}

// Starts a new planned journey through JourneyManager
bool SafetySystem::startJourney(const string& startLoc, const string& dest,
                                const string& eta, const vector<string>& route) {
    cout << "\n--- [SafetySystem] Starting Planned Journey ---" << endl;
    return journeyManager.startJourney(startLoc, dest, eta, route);
}

// Accepts a simulated location update and checks for route deviations.
// If a deviation is detected, the generated Event is automatically passed to logEvent().
void SafetySystem::updateJourneyLocation(const string& currentLocation, const string& timestamp) {
    cout << "\n--- [SafetySystem] Processing Journey Location Update ---" << endl;
    Event deviationEvent;
    bool deviationDetected = journeyManager.updateLocation(currentLocation, timestamp, deviationEvent);

    if (deviationDetected) {
        cout << "  [SafetySystem ALERT] Route deviation reported by JourneyManager! Forwarding to RiskAnalyzer..." << endl;
        logEvent(deviationEvent);
    }
}

// Displays the current journey route progress and status
void SafetySystem::displayJourneyDetails() const {
    journeyManager.displayJourneyDetails();
}

// Ends the active journey and automatically clears its scheduled check-ins
void SafetySystem::endJourney() {
    cout << "\n--- [SafetySystem] Ending Active Journey ---" << endl;
    journeyManager.endJourney();
    checkInManager.clearCheckIns();
}

// Schedules a safety check-in (only allowed during an active journey)
int SafetySystem::scheduleCheckIn(const string& scheduledTime, const string& description,
                                  const string& location) {
    if (!journeyManager.isActive()) {
        cout << "  [SafetySystem Error] No active journey! Check-ins can only be scheduled during an active journey." << endl;
        return -1;
    }

    cout << "\n--- [SafetySystem] Scheduling Safety Check-in ---" << endl;
    return checkInManager.addCheckIn(scheduledTime, description, location);
}

// Confirms and completes a scheduled check-in by ID
bool SafetySystem::completeCheckIn(int checkInID) {
    cout << "\n--- [SafetySystem] Confirming Check-in ---" << endl;
    return checkInManager.completeCheckIn(checkInID);
}

// Processes simulated current-time update.
// Advances RiskAnalyzer sliding window, checks verification request deadlines,
// and evaluates scheduled check-ins for missed events.
void SafetySystem::processCurrentTimeUpdate(const string& currentTime) {
    cout << "\n--- [SafetySystem] Processing Current-Time Check-in & Verification Update ---" << endl;
    
    // 1. Advance RiskAnalyzer sliding time window
    riskAnalyzer.updateCurrentTime(currentTime);

    // If risk normalized below threshold after sliding window advancement, reset the episode latch
    if (!riskAnalyzer.isRiskHigh() && activeVerificationEpisode && !verificationManager.hasActivePendingRequest()) {
        activeVerificationEpisode = false;
        cout << "  [SafetySystem Info] Risk levels have normalized below threshold (Score: "
             << riskAnalyzer.calculateRiskScore() << "). Active verification episode reset." << endl;
    }

    // 2. Evaluate active verification request timeouts against simulated current time
    // When a request times out, create and dispatch an emergency incident (Phase 4.4)
    vector<int> timedOutIDs = verificationManager.updateCurrentTime(currentTime);
    if (!timedOutIDs.empty()) {
        for (int tid : timedOutIDs) {
            cout << "  [SafetySystem TIMEOUT ALERT] Verification Request #" << tid
                 << " has TIMED OUT at " << currentTime << "! Initiating Automatic Emergency Dispatch..." << endl;

            if (verificationToIncidentMap.find(tid) == verificationToIncidentMap.end()) {
                VerificationRequest req;
                if (verificationManager.getRequestByID(tid, req)) {
                    autoIncidentCounter++;
                    int incID = autoIncidentCounter;
                    string desc = "EMERGENCY ESCALATION: Verification timeout - no user response (Verification Request #"
                                + to_string(tid) + ", Risk: " + to_string(req.riskScore)
                                + " [" + req.riskLevel + "], Deadline: " + req.deadlineTimestamp + ").";

                    Incident inc(incID, desc, req.triggerLocation, 5, tid, req.riskScore, req.riskLevel, currentTime);
                    responseManager.dispatchEmergency(inc, currentTime);
                    verificationToIncidentMap[tid] = incID;
                    activeRiskEscalated = true;
                }
            }
        }
    }

    // 3. Evaluate priority escalation timeouts in ResponseManager (Phase 4.4)
    responseManager.updateCurrentTime(currentTime);

    // 4. Evaluate scheduled check-ins for missed check-ins
    vector<Event> missedEvents = checkInManager.updateCurrentTime(currentTime);

    // 5. Ingest any missed check-in events into SafetySystem
    if (!missedEvents.empty()) {
        cout << "  [SafetySystem ALERT] " << missedEvents.size()
             << " missed check-in event(s) detected! Forwarding to RiskAnalyzer..." << endl;
        for (const auto& ev : missedEvents) {
            logEvent(ev);
        }
    } else {
        cout << "  [SafetySystem] Time updated to " << currentTime << ". No missed check-ins." << endl;
    }
}

// Confirms the user is safe, closing the active or specified verification request
bool SafetySystem::confirmUserSafe(int requestID) {
    cout << "\n--- [SafetySystem] Confirming User Safety ---" << endl;
    int targetID = requestID;
    if (targetID <= 0) {
        targetID = verificationManager.getActiveRequestID();
        if (targetID <= 0) {
            cout << "  [SafetySystem Error] No active pending verification request to confirm." << endl;
            return false;
        }
    }

    bool success = verificationManager.confirmSafe(targetID);
    if (success) {
        // Only reset the episode latch if the underlying risk score has actually normalized below threshold.
        // If the risk score remains >= 50, keep activeVerificationEpisode true to prevent subsequent events
        // from immediately reopening duplicate verification requests for the same unresolved episode.
        if (!riskAnalyzer.isRiskHigh()) {
            activeVerificationEpisode = false;
            cout << "  [SafetySystem] User confirmed safe for Request #" << targetID << "." << endl;
            cout << "  Emergency escalation averted. Risk normalized; episode latch reset." << endl;
        } else {
            cout << "  [SafetySystem] User confirmed safe for Request #" << targetID << "." << endl;
            cout << "  Emergency escalation averted. Episode remains marked as addressed while risk score ("
                 << riskAnalyzer.calculateRiskScore() << ") remains elevated." << endl;
        }
    }
    return success;
}

// User explicitly requests immediate emergency help (Phase 4.4)
bool SafetySystem::requestImmediateHelp(int requestID) {
    cout << "\n--- [SafetySystem] User Requesting Immediate Help ---" << endl;
    int targetID = requestID;
    if (targetID <= 0) {
        targetID = verificationManager.getActiveRequestID();
        if (targetID <= 0) {
            cout << "  [SafetySystem Error] No active pending verification request to request help for." << endl;
            return false;
        }
    }

    bool success = verificationManager.requestHelp(targetID);
    if (success) {
        cout << "  [SafetySystem ALERT] Immediate help registered for Request #" << targetID
             << "! Initiating Priority-Based Emergency Dispatch..." << endl;

        if (verificationToIncidentMap.find(targetID) == verificationToIncidentMap.end()) {
            VerificationRequest req;
            if (verificationManager.getRequestByID(targetID, req)) {
                autoIncidentCounter++;
                int incID = autoIncidentCounter;
                string timeStr = req.resolutionTimestamp.empty() ? req.creationTimestamp : req.resolutionTimestamp;
                string desc = "EMERGENCY ESCALATION: User requested immediate help (Verification Request #"
                            + to_string(targetID) + ", Risk: " + to_string(req.riskScore)
                            + " [" + req.riskLevel + "]).";

                Incident inc(incID, desc, req.triggerLocation, 5, targetID, req.riskScore, req.riskLevel, timeStr);
                responseManager.dispatchEmergency(inc, timeStr);
                verificationToIncidentMap[targetID] = incID;
                activeRiskEscalated = true;
            }
        }
    }
    return success;
}

// Displays all currently pending verification requests
void SafetySystem::displayPendingVerifications() const {
    verificationManager.displayPendingRequests();
}

// Displays verification statistics
void SafetySystem::displayVerificationStatistics() const {
    verificationManager.displayVerificationStatistics();
}

// Checks if there is an active pending verification request
bool SafetySystem::hasActiveVerification() const {
    return verificationManager.hasActivePendingRequest();
}

// Returns the ID of the active pending verification request, or -1 if none
int SafetySystem::getActiveVerificationID() const {
    return verificationManager.getActiveRequestID();
}

// Returns a const reference to the internal VerificationManager
const VerificationManager& SafetySystem::getVerificationManager() const {
    return verificationManager;
}

// ---------------------------------------------------------------
// Incident Lifecycle & Priority Dispatch Operations (Phase 4.4 & 4.5)
// ---------------------------------------------------------------

bool SafetySystem::acknowledgeIncident(int incidentID, int contactID, const string& timestamp) {
    cout << "\n--- [SafetySystem] Recording Contact Incident Acknowledgement ---" << endl;
    return responseManager.acknowledgeIncident(incidentID, contactID, timestamp);
}

bool SafetySystem::resolveIncident(int incidentID, const string& notes, const string& timestamp) {
    cout << "\n--- [SafetySystem] Resolving Incident ---" << endl;
    return responseManager.resolveIncident(incidentID, notes, timestamp);
}

bool SafetySystem::cancelIncident(int incidentID, const string& reason, const string& timestamp) {
    cout << "\n--- [SafetySystem] Cancelling Incident ---" << endl;
    return responseManager.cancelIncident(incidentID, reason, timestamp);
}

void SafetySystem::displayActiveIncidents() const {
    responseManager.displayActiveIncidents();
}

void SafetySystem::displayIncidentHistory() const {
    responseManager.displayIncidentHistory();
}

void SafetySystem::displayNotificationHistory(int incidentID) const {
    responseManager.displayNotificationHistory(incidentID);
}

void SafetySystem::displayIncidentStatistics() const {
    responseManager.displayIncidentStatistics();
}

ResponseManager& SafetySystem::getResponseManager() {
    return responseManager;
}

const ResponseManager& SafetySystem::getResponseManager() const {
    return responseManager;
}

// Displays all scheduled check-ins
void SafetySystem::displayScheduledCheckIns() const {
    checkInManager.displayCheckIns();
}

// Displays check-in statistics
void SafetySystem::displayCheckInStatistics() const {
    cout << "\n--- [Safety Check-in Statistics] ---" << endl;
    cout << "  Total Scheduled:     " << checkInManager.getCheckInCount() << endl;
    cout << "  Pending Check-ins:   " << checkInManager.getPendingCount() << endl;
    cout << "  Completed (On-time): " << checkInManager.getCompletedCount() << endl;
    cout << "  Late Confirmations:  " << checkInManager.getLateConfirmationCount() << endl;
    cout << "  Unconfirmed Missed:  " << checkInManager.getMissedCount() << endl;
    cout << "  Grace Period:        " << checkInManager.getGracePeriod() << " minutes" << endl;
    cout << "------------------------------------\n" << endl;
}

// Returns a const reference to the internal RiskAnalyzer for testing/inspection
const RiskAnalyzer& SafetySystem::getRiskAnalyzer() const {
    return riskAnalyzer;
}

// Displays detailed risk analysis diagnostics
void SafetySystem::displayRiskAnalysis() const {
    cout << "\n--- [Current Risk Analysis] ---" << endl;
    cout << "  Recent Events Monitored: " << riskAnalyzer.getEventCount() << endl;
    cout << "  Calculated Risk Score:   " << riskAnalyzer.calculateRiskScore() << endl;
    cout << "  Risk Category:           " << riskAnalyzer.getRiskLevel() << endl;
    cout << "  Threshold Alert State:   " << (riskAnalyzer.isRiskHigh() ? "ACTIVE (HIGH RISK)" : "NORMAL") << endl;
    cout << "--------------------------------\n" << endl;
}

// Forwards request to display registered contacts
void SafetySystem::displayRegisteredContacts() const {
    cout << "\n--- [Registered Emergency Contacts] ---" << endl;
    responseManager.displayContacts();
    cout << "---------------------------------------\n" << endl;
}

// Forwards request to display recent events in the sliding queue
void SafetySystem::displayRecentEvents() const {
    cout << "\n--- [Recent Safety Events] ---" << endl;
    riskAnalyzer.displayRecentEvents();
    cout << "------------------------------\n" << endl;
}

// Forwards request to display pending incidents in the priority queue
void SafetySystem::displayPendingEmergencies() const {
    cout << "\n--- [Pending Emergency Incidents] ---" << endl;
    responseManager.displayPendingIncidents();
    cout << "------------------------------------\n" << endl;
}

// Displays overall system status, diagnostics, and counts
void SafetySystem::displaySystemStatus() {
    cout << "\n=============================================" << endl;
    cout << "        SAFESIGNAL SYSTEM STATUS             " << endl;
    cout << "=============================================" << endl;
    cout << "  Registered Contacts:    " << responseManager.getContactCount() << endl;
    cout << "  Recent Events in Queue: " << riskAnalyzer.getEventCount() << endl;
    cout << "  Current Risk Score:     " << riskAnalyzer.calculateRiskScore()
         << " (" << riskAnalyzer.getRiskLevel() << ")" << endl;
    cout << "  High-Risk State:        " << (riskAnalyzer.isRiskHigh() ? "YES [TRIGGERED]" : "NO [NORMAL]") << endl;
    cout << "  Active Verification:    " << (verificationManager.hasActivePendingRequest() ? "YES (Pending User Response)" : "NO") << endl;
    cout << "  Pending Emergencies:    " << responseManager.getPendingIncidentCount() << endl;
    cout << "  Active Journey:         " << (journeyManager.isActive() ? "YES" : "NO") << endl;
    cout << "  Scheduled Check-ins:    " << checkInManager.getCheckInCount()
         << " (" << checkInManager.getPendingCount() << " pending, "
         << checkInManager.getMissedCount() << " missed)" << endl;
    cout << "=============================================" << endl;

    // Show details of contacts, recent events, pending verifications, and pending incidents
    responseManager.displayContacts();
    riskAnalyzer.displayRecentEvents();
    if (verificationManager.hasActivePendingRequest()) {
        verificationManager.displayPendingRequests();
    }
    responseManager.displayPendingIncidents();
    cout << "=============================================\n" << endl;
}
