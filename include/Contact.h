#ifndef CONTACT_H
#define CONTACT_H

#include <iostream>
#include <string>

using namespace std;

class Contact {
private:
    int contactID;
    string name;
    string phoneNumber;
    string relationship;
    int priorityLevel; // Priority 1 (Primary), Priority 2 (Secondary), Priority 3 (Emergency/Campus Security)

public:
    Contact();
    // Parameterized constructor: default priority = 1 preserves backward compatibility
    Contact(int id, string n, string phone, string relation, int priority = 1);
    ~Contact();

    // Getter functions
    int getContactID() const;
    string getName() const;
    string getPhoneNumber() const;
    string getRelationship() const;
    int getPriorityLevel() const;
    string getPriorityDescription() const;

    // Setter function with validation (1-3)
    bool setPriorityLevel(int priority);

    // Display function
    void displayContact() const;
};

#endif // CONTACT_H
