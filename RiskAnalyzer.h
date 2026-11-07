#ifndef RISK_ANALYZER_H
#define RISK_ANALYZER_H

#include <iostream>
#include <string>
#include <queue>
#include "Event.h"

using namespace std;

// Analyzes events within sliding time windows to detect patterns and calculate risk
class RiskAnalyzer {
private:
    // Queue storing incoming chronological events
    queue<Event> eventQueue;
    int windowCapacity;

public:
    RiskAnalyzer(int capacity = 5);
    ~RiskAnalyzer();

    // Event collection
    void addEvent(const Event& event);

    // Risk calculation logic
    int calculateRiskScore() const;
    string getRiskLevel() const;
    bool isRiskHigh() const;

    // Inspection & status
    void displayRecentEvents() const;
    int getEventCount() const;
    void clearEvents();
};

#endif // RISK_ANALYZER_H
