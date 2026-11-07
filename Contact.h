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

public:
    Contact();
    Contact(int id, string n, string phone, string relation);
    ~Contact();

    // Getter functions
    int getContactID() const;
    string getName() const;
    string getPhoneNumber() const;
    string getRelationship() const;

    // Display function
    void displayContact() const;
};

#endif // CONTACT_H
