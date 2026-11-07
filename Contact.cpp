#include "Contact.h"

using namespace std;

// Default constructor
Contact::Contact() {
    contactID = 0;
    name = "Unknown";
    phoneNumber = "N/A";
    relationship = "General";
}

// Parameterized constructor
Contact::Contact(int id, string n, string phone, string relation) {
    contactID = id;
    name = n;
    phoneNumber = phone;
    relationship = relation;
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

// Displays the contact details clearly in the console
void Contact::displayContact() const {
    cout << "  [ID: " << contactID << "] "
         << name << " (" << relationship << ") - Phone: " << phoneNumber << endl;
}

