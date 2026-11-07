#include "SafetySystem.h"

using namespace std;

// Constructor
SafetySystem::SafetySystem() {
    activeRiskEscalated = false;
    autoIncidentCounter = 9000; // Auto-generated incidents use IDs starting at 9001
}

// Destructor
SafetySystem::~SafetySystem() {
    // Member objects clean up automatically
}

// Logs an incoming event and evaluates risk for automatic incident escalation
void SafetySystem::logEvent(const Event& event) {
    cout << "\n--- [SafetySystem] Ingesting New Event ---" << endl;
    riskAnalyzer.addEvent(event);

    // Evaluate risk state
    if (riskAnalyzer.isRiskHigh()) {
        if (!activeRiskEscalated) {
            // Risk threshold reached for the first time in this cycle -> trigger automatic escalation
            autoIncidentCounter++;
            int autoSeverity = (riskAnalyzer.getRiskLevel() == "High Risk" || riskAnalyzer.getRiskLevel() == "CRITICAL") ? 5 : 4;
            string autoDescription = "AUTOMATIC ESCALATION: " + riskAnalyzer.getRiskLevel()
                                   + " risk pattern detected (Risk Score: "
                                   + to_string(riskAnalyzer.calculateRiskScore()) + ").";

            Incident autoIncident(autoIncidentCounter, autoDescription, event.getLocation(), autoSeverity);

            cout << "  [ALERT] Risk threshold exceeded! Auto-generating Incident #"
                 << autoIncidentCounter << " (Severity " << autoSeverity << "/5)." << endl;

            responseManager.addIncident(autoIncident);
            activeRiskEscalated = true; // Latch flag to avoid repeated spam while condition persists
        } else {
            cout << "  [NOTICE] High risk persists, but an incident has already been dispatched for this event cycle." << endl;
        }
    } else {
        // If the risk has dropped below the threshold, reset the latch
        if (activeRiskEscalated) {
            activeRiskEscalated = false;
            cout << "  [INFO] Risk levels have normalized. Auto-escalation flag reset." << endl;
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

// Processes simulated current-time update. Advances RiskAnalyzer sliding window and checks for missed check-ins.
void SafetySystem::processCurrentTimeUpdate(const string& currentTime) {
    cout << "\n--- [SafetySystem] Processing Current-Time Check-in Update ---" << endl;
    
    // Advance RiskAnalyzer sliding time window
    riskAnalyzer.updateCurrentTime(currentTime);

    vector<Event> missedEvents = checkInManager.updateCurrentTime(currentTime);

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
    cout << "  Active Risk Escalated:  " << (activeRiskEscalated ? "YES (Alert Sent)" : "NO") << endl;
    cout << "  Pending Emergencies:    " << responseManager.getPendingIncidentCount() << endl;
    cout << "  Active Journey:         " << (journeyManager.isActive() ? "YES" : "NO") << endl;
    cout << "  Scheduled Check-ins:    " << checkInManager.getCheckInCount()
         << " (" << checkInManager.getPendingCount() << " pending, "
         << checkInManager.getMissedCount() << " missed)" << endl;
    cout << "=============================================" << endl;

    // Show details of contacts, recent events, and pending incidents
    responseManager.displayContacts();
    riskAnalyzer.displayRecentEvents();
    responseManager.displayPendingIncidents();
    cout << "=============================================\n" << endl;
}
