
#ifndef SAFETY_SYSTEM_H
#define SAFETY_SYSTEM_H

#include "Incident.h"
#include "RiskAnalyzer.h"
#include "ResponseManager.h"

class SafetySystem {
private:
    RiskAnalyzer riskAnalyzer;
    ResponseManager responseManager;

public:
    SafetySystem();
    ~SafetySystem();

    void reportIncident(const Incident& incident);
    void displaySystemStatus();
};

#endif
