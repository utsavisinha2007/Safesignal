#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <string>

using namespace std;

class Event {
private:
    int eventID;
    string eventType;
    string location;
    string timestamp;

public:
    Event();
    Event(int id, string type, string loc, string time);
    ~Event();

    // Getter functions
    int getEventID() const;
    string getEventType() const;
    string getLocation() const;
    string getTimestamp() const;

    // Helper to evaluate severity weight based on event type
    int getSeverityWeight() const;

    // Display function
    void displayEvent() const;
};

#endif // EVENT_H
