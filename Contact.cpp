#include "Contact.h"

using namespace std;

// Default constructor
Contact::Contact() {
    contactID = 0;
    name = "Unknown";
    phoneNumber = "N/A";
    relationship = "General";
    priorityLevel = 1;
}

// Parameterized constructor with validation and backward compatibility
Contact::Contact(int id, string n, string phone, string relation, int priority) {
    contactID = id;
    name = n;
    phoneNumber = phone;
    relationship = relation;
    if (priority >= 1 && priority <= 3) {
        priorityLevel = priority;
    } else {
        cout << "  [Warning] Invalid contact priority: " << priority
             << ". Defaulting to Priority 1 (Primary Contact)." << endl;
        priorityLevel = 1;
    }
}

// Destructor
Contact::~Contact() {
    // No dynamically allocated memory to free
}

// Getter for contact ID
int Contact::getContactID() const {
    return contactID;
}

// Getter for contact name
string Contact::getName() const {
    return name;
}

// Getter for phone number
string Contact::getPhoneNumber() const {
    return phoneNumber;
}

// Getter for relationship
string Contact::getRelationship() const {
    return relationship;
}

// Getter for priority level
int Contact::getPriorityLevel() const {
    return priorityLevel;
}

// Helper returning textual description of priority level
string Contact::getPriorityDescription() const {
    switch (priorityLevel) {
        case 1:
            return "Primary Contact";
        case 2:
            return "Secondary Contact";
        case 3:
            return "Emergency / Campus Security Contact";
        default:
            return "Unknown Priority";
    }
}

// Setter for priority level with validation (1 to 3)
bool Contact::setPriorityLevel(int priority) {
    if (priority >= 1 && priority <= 3) {
        priorityLevel = priority;
        return true;
    }
    cout << "  [Error] Invalid priority level: " << priority
         << ". Must be between 1 and 3 (1=Primary, 2=Secondary, 3=Emergency/Campus Security)." << endl;
    return false;
}

// Displays the contact details clearly in the console including priority number and description
void Contact::displayContact() const {
    cout << "  [ID: " << contactID << "] "
         << name << " (" << relationship << ") - Phone: " << phoneNumber
         << " | Priority " << priorityLevel << " (" << getPriorityDescription() << ")" << endl;
}

