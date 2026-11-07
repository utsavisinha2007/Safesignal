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
            int autoSeverity = (riskAnalyzer.getRiskLevel() == "CRITICAL") ? 5 : 4;
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
    cout << "=============================================" << endl;

    // Show details of contacts, recent events, and pending incidents
    responseManager.displayContacts();
    riskAnalyzer.displayRecentEvents();
    responseManager.displayPendingIncidents();
    cout << "=============================================\n" << endl;
}

