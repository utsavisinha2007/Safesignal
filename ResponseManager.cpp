#include "ResponseManager.h"

using namespace std;

// Constructor
ResponseManager::ResponseManager() {
    // Vector and Priority Queue initialize empty by default
}

// Destructor
ResponseManager::~ResponseManager() {
    // Standard library containers automatically handle their own memory cleanup
}

// Registers a new emergency contact into the dynamic vector
void ResponseManager::registerContact(const Contact& contact) {
    contacts.push_back(contact);
    cout << "  [ResponseManager] Registered emergency contact: "
         << contact.getName() << " (" << contact.getRelationship() << ")" << endl;
}

// Displays all registered emergency contacts stored in the vector
void ResponseManager::displayContacts() const {
    if (contacts.empty()) {
        cout << "  (No emergency contacts currently registered)" << endl;
        return;
    }

    cout << "  Registered Emergency Contacts (" << contacts.size() << "):" << endl;
    for (size_t i = 0; i < contacts.size(); i++) {
        contacts[i].displayContact();
    }
}

// Returns the number of registered contacts
int ResponseManager::getContactCount() const {
    return static_cast<int>(contacts.size());
}

// Adds an incident to the priority queue (Max-Heap based on severity)
void ResponseManager::addIncident(const Incident& incident) {
    incidentQueue.push(incident);
    cout << "  [ResponseManager] Incident #" << incident.getIncidentID()
         << " (Severity " << incident.getSeverity() << "/5) queued for priority triage." << endl;
}

// Checks if there are any pending incidents in the priority queue
bool ResponseManager::hasPendingIncidents() const {
    return !incidentQueue.empty();
}

// Returns the count of pending incidents
int ResponseManager::getPendingIncidentCount() const {
    return static_cast<int>(incidentQueue.size());
}

// Displays all currently pending incidents in order of priority (highest severity first)
void ResponseManager::displayPendingIncidents() const {
    if (incidentQueue.empty()) {
        cout << "  (No pending incidents in the queue)" << endl;
        return;
    }

    cout << "  Pending Incidents in Priority Queue (Highest -> Lowest Severity):" << endl;
    priority_queue<Incident> tempQueue = incidentQueue; // Copy queue for safe inspection
    int rank = 1;
    while (!tempQueue.empty()) {
        cout << "    Priority #" << rank++ << ":" << endl;
        tempQueue.top().displayIncident();
        tempQueue.pop();
    }
}

// Simulates sending emergency alerts to all registered contacts for a given incident.
// Note: This outputs simulated alerts to the console and does not perform real SMS or cellular transmissions.
void ResponseManager::dispatchAlerts(const Incident& incident) {
    cout << "\n  --- [SIMULATED EMERGENCY DISPATCH] ---" << endl;
    cout << "  Broadcasting alert for Incident #" << incident.getIncidentID()
         << " [Severity " << incident.getSeverity() << "/5] at " << incident.getLocation() << endl;

    if (contacts.empty()) {
        cout << "  [WARNING] No emergency contacts are registered to receive this alert!" << endl;
    } else {
        for (size_t i = 0; i < contacts.size(); i++) {
            cout << "  -> Simulated Notification Sent to " << contacts[i].getName()
                 << " (" << contacts[i].getRelationship() << ") at "
                 << contacts[i].getPhoneNumber() << endl;
        }
    }
    cout << "  --------------------------------------\n" << endl;
}

// Processes the highest-priority incident from the priority queue
void ResponseManager::processNextIncident() {
    // Safety check for empty queue
    if (incidentQueue.empty()) {
        cout << "  [ResponseManager] No pending emergencies to process." << endl;
        return;
    }

    // Retrieve the incident with the highest severity
    Incident topIncident = incidentQueue.top();
    incidentQueue.pop();

    cout << "  [ResponseManager] Triaging highest-priority emergency:" << endl;
    topIncident.displayIncident();

    // Dispatch simulated emergency notifications
    dispatchAlerts(topIncident);

    cout << "  [ResponseManager] Incident #" << topIncident.getIncidentID()
         << " has been successfully handled and resolved." << endl;
}

