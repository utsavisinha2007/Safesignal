
#ifndef CONTACT_H
#define CONTACT_H

#include <string>

class Contact {
private:
    int contactID;
    std::string name;
    std::string phoneNumber;
    std::string relationship;

public:
    Contact();

    Contact(int id, std::string n,
            std::string phone, std::string relation);

    ~Contact();

    int getContactID() const;
    std::string getName() const;
    std::string getPhoneNumber() const;
    std::string getRelationship() const;
};

#endif
