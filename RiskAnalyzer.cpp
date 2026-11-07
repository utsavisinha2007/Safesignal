#include "RiskAnalyzer.h"

using namespace std;

// Constructor: Initializes the analyzer with a fixed-size recent-event capacity
RiskAnalyzer::RiskAnalyzer(int capacity) {
    if (capacity > 0) {
        windowCapacity = capacity;
    } else {
        windowCapacity = 5; // Default fallback capacity
    }
}

// Destructor: Cleans up the queue
RiskAnalyzer::~RiskAnalyzer() {
    clearEvents();
}

// Adds an incoming event to the fixed-size recent-event queue (FIFO).
// If capacity is reached, the oldest event is evicted from the front.
void RiskAnalyzer::addEvent(const Event& event) {
    if (eventQueue.size() >= static_cast<size_t>(windowCapacity)) {
        Event evicted = eventQueue.front();
        eventQueue.pop();
        cout << "  [RiskAnalyzer Queue] Evicted oldest event #" << evicted.getEventID()
             << " (" << evicted.getEventType() << ") to respect capacity limit ("
             << windowCapacity << ")." << endl;
    }

    eventQueue.push(event);
    cout << "  [RiskAnalyzer Queue] Enqueued event #" << event.getEventID()
         << " (" << event.getEventType() << "). Queue size: " << eventQueue.size() << endl;
}

// Calculates the cumulative risk score by summing the severity weights
// of all events currently in the fixed-size event queue.
int RiskAnalyzer::calculateRiskScore() const {
    int totalScore = 0;
    queue<Event> tempQueue = eventQueue; // Copy queue to traverse without modifying the original

    while (!tempQueue.empty()) {
        totalScore += tempQueue.front().getSeverityWeight();
        tempQueue.pop();
    }

    return totalScore;
}

// Categorizes the current risk level based on the cumulative risk score:
//   - Score < 20:       LOW
//   - Score 20 to 49:   MODERATE
//   - Score 50 to 79:   HIGH
//   - Score >= 80:      CRITICAL
string RiskAnalyzer::getRiskLevel() const {
    int score = calculateRiskScore();
    if (score < 20) {
        return "LOW";
    } else if (score < 50) {
        return "MODERATE";
    } else if (score < 80) {
        return "HIGH";
    } else {
        return "CRITICAL";
    }
}

// Returns true if the cumulative risk score meets or exceeds the HIGH threshold (50)
bool RiskAnalyzer::isRiskHigh() const {
    return calculateRiskScore() >= 50;
}

// Displays all events currently stored in the fixed-size queue
void RiskAnalyzer::displayRecentEvents() const {
    if (eventQueue.empty()) {
        cout << "  (No events currently in the recent-event queue)" << endl;
        return;
    }

    cout << "  Recent Events in Queue (Oldest -> Newest):" << endl;
    queue<Event> tempQueue = eventQueue;
    int index = 1;
    while (!tempQueue.empty()) {
        cout << "    " << index++ << ". ";
        tempQueue.front().displayEvent();
        tempQueue.pop();
    }

    cout << "  Total Calculated Risk Score: " << calculateRiskScore()
         << " [" << getRiskLevel() << " RISK]" << endl;
}

// Returns the number of events currently held in the queue
int RiskAnalyzer::getEventCount() const {
    return static_cast<int>(eventQueue.size());
}

// Clears all events from the queue
void RiskAnalyzer::clearEvents() {
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
}

