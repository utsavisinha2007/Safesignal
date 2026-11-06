
#ifndef EVENT_H
#define EVENT_H

#include <string>

class Event {
private:
    int eventID;
    std::string eventType;
    std::string location;
    std::string timestamp;

public:
    Event();

    Event(int id, std::string type,
          std::string loc, std::string time);

    ~Event();

    int getEventID() const;
    std::string getEventType() const;
    std::string getLocation() const;
    std::string getTimestamp() const;
};

#endif
