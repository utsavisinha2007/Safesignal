#ifndef RISK_ANALYZER_H
#define RISK_ANALYZER_H

#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include <unordered_map>
#include "Event.h"

using namespace std;

// Detailed diagnostic structure capturing intelligent risk scoring contributions
struct RiskAnalysisResult {
    int baseScore;
    int frequencyBonus;
    int patternBonus;
    int totalScore;
    string riskLevel;
    unordered_map<string, int> frequencyMap;
    vector<string> detectedPatterns;
    vector<string> scoringBreakdown;
};

// Internal timestamped event wrapper for sliding window tracking
struct TimestampedEvent {
    Event event;
    long long absoluteSeconds; // Absolute timeline seconds from Jan 1, 2000
};

// Analyzes events within a sliding time window, detects danger patterns,
// enforces chronological ordering, handles midnight/multi-day rollover,
// supports mixed timestamp formats safely, and dynamically calculates explainable risk scores.
class RiskAnalyzer {
private:
    deque<TimestampedEvent> eventWindow; // STL deque-based sliding time window
    int windowDurationMinutes;           // Configurable window duration in minutes (default: 10 mins)
    long long currentTimelineSeconds;    // Current simulated timeline in absolute seconds (-1 if unset)
    long long activeDateDays;            // Active calendar day in days from Jan 1, 2000
    int lastTimeOfDaySeconds;            // Last observed time-of-day in seconds for rollover detection
    string latestTimestampStr;           // String representation of latest simulated time

    // Helper: Computes total days since Jan 1, 2000
    long long daysFrom2000(int year, int month, int day) const;

    // Strict time-of-day parser (HH:MM or HH:MM:SS)
    bool parseTimeOfDay(const string& timeStr, int& outSeconds) const;

    // Unified timestamp parser supporting time-only and date-aware timestamps
    bool parseTimestamp(const string& timeStr, long long& outAbsoluteSeconds, bool& isDateAware) const;

    // Pure evaluation helper: calculates candidate timeline seconds WITHOUT mutating state.
    // Handles mixed formats, anchoring, and detects valid midnight rollovers vs backward jumps.
    bool calculateCandidateTimelineSeconds(const string& timeStr, long long& outCandidateSeconds,
                                           int& outCandidateTimeOfDay, long long& outCandidateDateDays) const;

    // Prunes events from the front of the deque that fall outside the active sliding time window
    void pruneExpiredEvents(long long currentSeconds);

public:
    // Constructor with configurable sliding window duration in minutes (default: 10 mins)
    RiskAnalyzer(int windowMinutes = 10);
    ~RiskAnalyzer();

    // Configuration
    void setWindowDurationMinutes(int minutes);
    int getWindowDurationMinutes() const;

    // Event collection with strict chronological ordering validation
    void addEvent(const Event& event);

    // Reliable simulated-time advancement mechanism (prunes expired events without requiring new events)
    void updateCurrentTime(const string& currentTime);

    // Advanced risk analysis calculation
    RiskAnalysisResult analyzeRisk() const;

    // Core backward-compatible methods required by SafetySystem and Main Menu
    int calculateRiskScore() const;
    string getRiskLevel() const;
    bool isRiskHigh() const;

    // Inspection and diagnostic display
    void displayRecentEvents() const;
    void displayRiskBreakdown() const;
    int getEventCount() const;
    void clearEvents();
    vector<string> getDetectedPatterns() const;
    string getLatestTimestamp() const;
};

#endif // RISK_ANALYZER_H
