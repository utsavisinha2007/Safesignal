#ifndef RESPONSE_MANAGER_H
#define RESPONSE_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Contact.h"
#include "Incident.h"

using namespace std;

// Manages verification requests and priority-based response escalation
class ResponseManager {
private:
    // Dynamic array (Vector) storing emergency contacts
    vector<Contact> contacts;

    // Priority Queue (Max-Heap) managing pending incidents by severity
    priority_queue<Incident> incidentQueue;

public:
    ResponseManager();
    ~ResponseManager();

    // Emergency contact registration & display
    void registerContact(const Contact& contact);
    void displayContacts() const;
    int getContactCount() const;

    // Incident queueing and dispatch
    void addIncident(const Incident& incident);
    void processNextIncident();
    bool hasPendingIncidents() const;
    int getPendingIncidentCount() const;
    void displayPendingIncidents() const;

    // Simulated emergency alerts to registered contacts
    void dispatchAlerts(const Incident& incident);
};

#endif // RESPONSE_MANAGER_H
