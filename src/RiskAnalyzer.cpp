#include "../include/RiskAnalyzer.h"
#include <algorithm>
#include <cctype>

using namespace std;

// Constructor: Initializes sliding time-window duration and baseline date
RiskAnalyzer::RiskAnalyzer(int windowMinutes) {
    windowDurationMinutes = (windowMinutes > 0) ? windowMinutes : 10;
    currentTimelineSeconds = -1;
    activeDateDays = daysFrom2000(2026, 11, 7); // Default baseline simulation date: 2026-11-07
    lastTimeOfDaySeconds = -1;
    latestTimestampStr = "00:00:00";
}

// Destructor
RiskAnalyzer::~RiskAnalyzer() {
    clearEvents();
}

// Helper: Computes total days since Jan 1, 2000 for date-aware multi-day simulations
long long RiskAnalyzer::daysFrom2000(int year, int month, int day) const {
    long long days = 0;
    for (int y = 2000; y < year; y++) {
        bool leap = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        days += leap ? 366 : 365;
    }
    bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    int monthDays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (isLeap) monthDays[2] = 29;

    for (int m = 1; m < month; m++) {
        days += monthDays[m];
    }
    days += (day - 1);
    return days;
}

// Strict time-of-day parser (HH:MM or HH:MM:SS)
bool RiskAnalyzer::parseTimeOfDay(const string& timeStr, int& outSeconds) const {
    outSeconds = -1;
    if (timeStr.empty()) return false;

    int colonCount = 0;
    for (char c : timeStr) {
        if (c == ':') colonCount++;
        else if (!isdigit(static_cast<unsigned char>(c))) return false;
    }

    if (colonCount != 1 && colonCount != 2) return false;

    size_t firstColon = timeStr.find(':');
    string hourStr = timeStr.substr(0, firstColon);
    if (hourStr.empty() || hourStr.length() > 2) return false;

    string minStr = "";
    string secStr = "0";

    if (colonCount == 1) {
        minStr = timeStr.substr(firstColon + 1);
        if (minStr.empty() || minStr.length() > 2) return false;
    } else {
        size_t secondColon = timeStr.find(':', firstColon + 1);
        minStr = timeStr.substr(firstColon + 1, secondColon - firstColon - 1);
        secStr = timeStr.substr(secondColon + 1);
        if (minStr.empty() || minStr.length() > 2 || secStr.empty() || secStr.length() > 2) return false;
    }

    int hours = stoi(hourStr);
    int minutes = stoi(minStr);
    int seconds = stoi(secStr);

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {
        return false;
    }

    outSeconds = hours * 3600 + minutes * 60 + seconds;
    return true;
}

// Unified timestamp parser supporting:
// 1. Time only: "HH:MM" and "HH:MM:SS"
// 2. Full date-aware: "YYYY-MM-DD HH:MM" and "YYYY-MM-DD HH:MM:SS"
bool RiskAnalyzer::parseTimestamp(const string& timeStr, long long& outAbsoluteSeconds, bool& isDateAware) const {
    outAbsoluteSeconds = -1;
    isDateAware = false;
    if (timeStr.empty()) return false;

    size_t sep = timeStr.find_first_of(" T");
    if (sep != string::npos) {
        // Date-aware format
        string datePart = timeStr.substr(0, sep);
        string timePart = timeStr.substr(sep + 1);

        if (datePart.length() != 10 || datePart[4] != '-' || datePart[7] != '-') return false;
        for (size_t i = 0; i < datePart.length(); i++) {
            if (i != 4 && i != 7 && !isdigit(static_cast<unsigned char>(datePart[i]))) return false;
        }

        int y = stoi(datePart.substr(0, 4));
        int m = stoi(datePart.substr(5, 2));
        int d = stoi(datePart.substr(8, 2));

        if (y < 2000 || y > 2099 || m < 1 || m > 12 || d < 1 || d > 31) return false;
        bool isLeap = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        int maxDays = (m == 2) ? (isLeap ? 29 : 28) : ((m == 4 || m == 6 || m == 9 || m == 11) ? 30 : 31);
        if (d > maxDays) return false;

        int timeSec = 0;
        if (!parseTimeOfDay(timePart, timeSec)) return false;

        long long days = daysFrom2000(y, m, d);
        outAbsoluteSeconds = days * 86400LL + timeSec;
        isDateAware = true;
        return true;
    } else {
        // Time-only format
        int timeSec = 0;
        if (!parseTimeOfDay(timeStr, timeSec)) return false;
        outAbsoluteSeconds = timeSec;
        isDateAware = false;
        return true;
    }
}

// Pure calculation helper: evaluates candidate timeline seconds WITHOUT mutating state.
// Handles mixed formats by anchoring time-only timestamps to the active date, and strictly
// distinguishes legitimate midnight rollovers from backward time jumps.
bool RiskAnalyzer::calculateCandidateTimelineSeconds(const string& timeStr, long long& outCandidateSeconds,
                                                    int& outCandidateTimeOfDay, long long& outCandidateDateDays) const {
    long long parsedSeconds = -1;
    bool isDateAware = false;

    if (!parseTimestamp(timeStr, parsedSeconds, isDateAware)) {
        return false;
    }

    if (isDateAware) {
        outCandidateSeconds = parsedSeconds;
        outCandidateDateDays = parsedSeconds / 86400LL;
        outCandidateTimeOfDay = static_cast<int>(parsedSeconds % 86400LL);
        return true;
    }

    // Time-only format: anchor to active calendar date
    int timeSec = static_cast<int>(parsedSeconds);
    outCandidateTimeOfDay = timeSec;

    if (lastTimeOfDaySeconds == -1) {
        outCandidateDateDays = activeDateDays;
        outCandidateSeconds = outCandidateDateDays * 86400LL + timeSec;
    } else {
        if (timeSec < lastTimeOfDaySeconds) {
            // Check for valid forward midnight rollover:
            // Previous time was late evening (>= 18:00) AND candidate is early morning (<= 06:00)
            // with forward wrap <= 6 hours (21600 seconds)
            if (lastTimeOfDaySeconds >= 64800 && timeSec <= 21600 &&
                ((timeSec + 86400) - lastTimeOfDaySeconds <= 21600)) {
                outCandidateDateDays = activeDateDays + 1;
                outCandidateSeconds = outCandidateDateDays * 86400LL + timeSec;
            } else {
                // Backward jump on same day (e.g. 15:00:00 to 02:00:00)
                outCandidateDateDays = activeDateDays;
                outCandidateSeconds = outCandidateDateDays * 86400LL + timeSec;
            }
        } else {
            // Normal chronological progression on the same day
            outCandidateDateDays = activeDateDays;
            outCandidateSeconds = outCandidateDateDays * 86400LL + timeSec;
        }
    }

    return true;
}

// Configures the sliding time-window duration in minutes
// Requirement 4: Changing window duration immediately updates active event window
void RiskAnalyzer::setWindowDurationMinutes(int minutes) {
    if (minutes > 0) {
        windowDurationMinutes = minutes;
        cout << "  [RiskAnalyzer] Active sliding time-window configured to "
             << windowDurationMinutes << " minutes." << endl;
        if (currentTimelineSeconds != -1) {
            pruneExpiredEvents(currentTimelineSeconds);
        }
    } else {
        cout << "  [RiskAnalyzer Error] Window duration must be positive." << endl;
    }
}

int RiskAnalyzer::getWindowDurationMinutes() const {
    return windowDurationMinutes;
}

// Prunes events from the front of the deque that fall outside the active sliding time window
void RiskAnalyzer::pruneExpiredEvents(long long currentSeconds) {
    long long windowDurationSeconds = static_cast<long long>(windowDurationMinutes) * 60;

    while (!eventWindow.empty()) {
        long long elapsed = currentSeconds - eventWindow.front().absoluteSeconds;
        if (elapsed > windowDurationSeconds) {
            Event expired = eventWindow.front().event;
            eventWindow.pop_front();
            cout << "  [Sliding Window] Pruned expired Event #" << expired.getEventID()
                 << " (" << expired.getEventType() << " at " << expired.getTimestamp()
                 << ") - Elapsed: " << (elapsed / 60) << "m " << (elapsed % 60)
                 << "s (Exceeds " << windowDurationMinutes << "m window)." << endl;
        } else {
            break; // Chronological ordering guarantees subsequent events are newer
        }
    }
}

// Ingests an incoming event into the STL deque-based sliding time window
// Requirement 3: Strictly validates chronological ordering and rejects out-of-order events
void RiskAnalyzer::addEvent(const Event& event) {
    long long candidateSec = -1;
    int candidateTimeOfDay = -1;
    long long candidateDateDays = activeDateDays;

    if (!calculateCandidateTimelineSeconds(event.getTimestamp(), candidateSec, candidateTimeOfDay, candidateDateDays)) {
        cout << "  [RiskAnalyzer Warning] Event #" << event.getEventID()
             << " rejected from time window: Invalid timestamp '" << event.getTimestamp()
             << "'. Risk calculation remains uncorrupted." << endl;
        return;
    }

    // Chronological validation check: strictly reject earlier events
    if (currentTimelineSeconds != -1 && candidateSec < currentTimelineSeconds) {
        cout << "  [RiskAnalyzer Warning] Out-of-order Event #" << event.getEventID()
             << " rejected! Timestamp '" << event.getTimestamp()
             << "' is earlier than current timeline (" << latestTimestampStr
             << "). Active window preserved." << endl;
        return; // Returns WITHOUT mutating internal timeline state
    }

    // Commit state
    currentTimelineSeconds = candidateSec;
    activeDateDays = candidateDateDays;
    lastTimeOfDaySeconds = candidateTimeOfDay;
    latestTimestampStr = event.getTimestamp();

    pruneExpiredEvents(currentTimelineSeconds);

    eventWindow.push_back({ event, currentTimelineSeconds });
    cout << "  [Sliding Window] Ingested Event #" << event.getEventID()
         << " (" << event.getEventType() << " at " << event.getTimestamp()
         << "). Active events in window: " << eventWindow.size() << endl;
}

// Requirement 1: Reliable simulated-time advancement mechanism
// Prunes expired events when time progresses even when no new safety event arrives
void RiskAnalyzer::updateCurrentTime(const string& currentTime) {
    long long candidateSec = -1;
    int candidateTimeOfDay = -1;
    long long candidateDateDays = activeDateDays;

    if (!calculateCandidateTimelineSeconds(currentTime, candidateSec, candidateTimeOfDay, candidateDateDays)) {
        cout << "  [RiskAnalyzer Warning] Time advancement rejected: Invalid timestamp '"
             << currentTime << "'." << endl;
        return;
    }

    if (currentTimelineSeconds != -1 && candidateSec < currentTimelineSeconds) {
        cout << "  [RiskAnalyzer Warning] Cannot rewind simulation time from "
             << latestTimestampStr << " to " << currentTime << "." << endl;
        return; // Returns WITHOUT mutating internal timeline state
    }

    // Commit state
    currentTimelineSeconds = candidateSec;
    activeDateDays = candidateDateDays;
    lastTimeOfDaySeconds = candidateTimeOfDay;
    latestTimestampStr = currentTime;

    pruneExpiredEvents(currentTimelineSeconds);
}

// Analyzes events in the active sliding window and computes an explainable risk breakdown
RiskAnalysisResult RiskAnalyzer::analyzeRisk() const {
    RiskAnalysisResult result;
    result.baseScore = 0;
    result.frequencyBonus = 0;
    result.patternBonus = 0;
    result.totalScore = 0;

    if (eventWindow.empty()) {
        result.riskLevel = "Normal";
        return result;
    }

    // Step 1: Base severity sum & frequency counting
    for (const auto& item : eventWindow) {
        result.baseScore += item.event.getSeverityWeight();
        result.frequencyMap[item.event.getEventType()]++;
    }

    // Step 2: Event frequency bonus for recurring threats
    for (const auto& entry : result.frequencyMap) {
        const string& type = entry.first;
        int count = entry.second;
        if (count >= 2) {
            int bonus = (count - 1) * 5; // +5 points for each recurrence beyond the first
            result.frequencyBonus += bonus;
            result.scoringBreakdown.push_back("Recurring Event (" + type + " x" + to_string(count) + "): +" + to_string(bonus) + " pts");
        }
    }

    // Step 3: Pattern detection rules (each applied at most once per cycle)
    int routeDevCount = result.frequencyMap["Route Deviation"];
    int missedCheckInCount = result.frequencyMap["Missed Check-in"];

    // Pattern A: Route Deviation + Missed Check-in (High danger correlation)
    if (routeDevCount >= 1 && missedCheckInCount >= 1) {
        result.patternBonus += 25;
        result.detectedPatterns.push_back("Route Deviation + Missed Check-in");
        result.scoringBreakdown.push_back("Pattern Match: Route Deviation + Missed Check-in (+25 pts)");
    }

    // Pattern B: Multiple Missed Check-ins
    if (missedCheckInCount >= 2) {
        result.patternBonus += 15;
        result.detectedPatterns.push_back("Multiple Missed Check-ins");
        result.scoringBreakdown.push_back("Pattern Match: Multiple Missed Check-ins (" + to_string(missedCheckInCount) + ") (+15 pts)");
    }

    // Pattern C: Repeated Route Deviations
    if (routeDevCount >= 2) {
        result.patternBonus += 15;
        result.detectedPatterns.push_back("Repeated Route Deviations");
        result.scoringBreakdown.push_back("Pattern Match: Repeated Route Deviations (" + to_string(routeDevCount) + ") (+15 pts)");
    }

    // Pattern D: High Activity Burst (Three or more safety events within active window)
    if (eventWindow.size() >= 3) {
        result.patternBonus += 10;
        result.detectedPatterns.push_back("Three or more safety events within active window");
        result.scoringBreakdown.push_back("Pattern Match: High Activity Burst (>= 3 events) (+10 pts)");
    }

    // Step 4: Final score capped at 100
    int rawScore = result.baseScore + result.frequencyBonus + result.patternBonus;
    result.totalScore = min(100, rawScore);

    // Step 5: Risk classification
    if (result.totalScore < 20) {
        result.riskLevel = "Normal";
    } else if (result.totalScore < 50) {
        result.riskLevel = "Unusual";
    } else if (result.totalScore < 80) {
        result.riskLevel = "Concerning";
    } else {
        result.riskLevel = "High Risk";
    }

    return result;
}

int RiskAnalyzer::calculateRiskScore() const {
    return analyzeRisk().totalScore;
}

string RiskAnalyzer::getRiskLevel() const {
    return analyzeRisk().riskLevel;
}

bool RiskAnalyzer::isRiskHigh() const {
    return calculateRiskScore() >= 50;
}

vector<string> RiskAnalyzer::getDetectedPatterns() const {
    return analyzeRisk().detectedPatterns;
}

string RiskAnalyzer::getLatestTimestamp() const {
    return latestTimestampStr;
}

void RiskAnalyzer::displayRiskBreakdown() const {
    RiskAnalysisResult result = analyzeRisk();

    cout << "\n--- [Intelligent Risk Analysis Diagnostics] ---" << endl;
    cout << "  Active Time Window:      " << windowDurationMinutes << " minutes" << endl;
    cout << "  Events in Window:        " << eventWindow.size() << endl;
    cout << "  Current Risk Score:      " << result.totalScore << " / 100" << endl;
    cout << "  Risk Classification:     " << result.riskLevel << endl;
    cout << "  High-Risk State:         " << (isRiskHigh() ? "YES [ACTIONABLE]" : "NO [NORMAL]") << endl;

    cout << "\n  [Event Frequency Breakdown]:" << endl;
    if (result.frequencyMap.empty()) {
        cout << "    (No active events)" << endl;
    } else {
        for (const auto& entry : result.frequencyMap) {
            cout << "    - " << entry.first << ": " << entry.second << " event(s)" << endl;
        }
    }

    cout << "\n  [Detected Patterns]:" << endl;
    if (result.detectedPatterns.empty()) {
        cout << "    (No danger patterns detected)" << endl;
    } else {
        for (const auto& pattern : result.detectedPatterns) {
            cout << "    [!] " << pattern << endl;
        }
    }

    cout << "\n  [Scoring Contribution Breakdown]:" << endl;
    cout << "    - Base Severity Score:   +" << result.baseScore << " pts" << endl;
    cout << "    - Frequency Contribution: +" << result.frequencyBonus << " pts" << endl;
    cout << "    - Pattern-Based Bonus:   +" << result.patternBonus << " pts" << endl;
    for (const auto& note : result.scoringBreakdown) {
        cout << "      * " << note << endl;
    }
    cout << "    ---------------------------------" << endl;
    cout << "    - Final Capped Risk Score: " << result.totalScore << " / 100 ["
         << result.riskLevel << "]" << endl;
    cout << "-----------------------------------------------\n" << endl;
}

void RiskAnalyzer::displayRecentEvents() const {
    if (eventWindow.empty()) {
        cout << "  (No events currently in the active sliding window)" << endl;
        return;
    }

    cout << "  Active Sliding Window Events (" << windowDurationMinutes
         << "m window, Oldest -> Newest):" << endl;
    int index = 1;
    for (const auto& item : eventWindow) {
        cout << "    " << index++ << ". ";
        item.event.displayEvent();
    }

    displayRiskBreakdown();
}

int RiskAnalyzer::getEventCount() const {
    return static_cast<int>(eventWindow.size());
}

void RiskAnalyzer::clearEvents() {
    eventWindow.clear();
    currentTimelineSeconds = -1;
    activeDateDays = daysFrom2000(2026, 11, 7);
    lastTimeOfDaySeconds = -1;
    latestTimestampStr = "00:00:00";
}

