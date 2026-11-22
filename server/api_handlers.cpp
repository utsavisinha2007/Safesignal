#include "api_handlers.h"
#include <atomic>
#include <sstream>

using json = nlohmann::json;
using namespace std;

static atomic<int> autoEventID(9000);

static string vstatStr(VerificationStatus s) {
    switch (s) {
        case VSTAT_PENDING: return "PENDING";
        case VSTAT_CONFIRMED_SAFE: return "CONFIRMED_SAFE";
        case VSTAT_HELP_REQUESTED: return "HELP_REQUESTED";
        case VSTAT_TIMED_OUT: return "TIMED_OUT";
        default: return "UNKNOWN";
    }
}

static json contactJ(const Contact& c) {
    return {{"id", c.getContactID()}, {"name", c.getName()}, {"phone", c.getPhoneNumber()},
            {"relationship", c.getRelationship()}, {"priority", c.getPriorityLevel()},
            {"priorityDescription", c.getPriorityDescription()}};
}

static json eventJ(const Event& e) {
    return {{"id", e.getEventID()}, {"type", e.getEventType()},
            {"location", e.getLocation()}, {"timestamp", e.getTimestamp()},
            {"severity", e.getSeverityWeight()}};
}

static json checkInJ(const CheckIn& ci) {
    string status = "PENDING";
    if (ci.isCompleted && !ci.isMissed) status = "COMPLETED";
    else if (ci.isCompleted && ci.isMissed) status = "LATE_CONFIRMATION";
    else if (!ci.isCompleted && ci.isMissed) status = "MISSED";
    return {{"id", ci.checkInID}, {"scheduledTime", ci.scheduledTime},
            {"description", ci.description}, {"location", ci.location},
            {"isCompleted", ci.isCompleted}, {"isMissed", ci.isMissed}, {"status", status}};
}

static json verificationJ(const VerificationRequest& r) {
    return {{"requestID", r.requestID}, {"riskScore", r.riskScore}, {"riskLevel", r.riskLevel},
            {"triggerLocation", r.triggerLocation}, {"creationTimestamp", r.creationTimestamp},
            {"deadlineTimestamp", r.deadlineTimestamp}, {"status", vstatStr(r.status)},
            {"resolutionTimestamp", r.resolutionTimestamp}};
}

static json notificationJ(const NotificationRecord& n) {
    return {{"incidentID", n.incidentID}, {"contactID", n.contactID},
            {"contactName", n.contactName}, {"contactPhone", n.contactPhone},
            {"priorityLevel", n.priorityLevel}, {"timestamp", n.timestamp},
            {"acknowledged", n.acknowledged}, {"acknowledgementTimestamp", n.acknowledgementTimestamp}};
}

static json incidentJ(const Incident& inc) {
    json j = {{"id", inc.getIncidentID()}, {"description", inc.getDescription()},
              {"location", inc.getLocation()}, {"severity", inc.getSeverity()},
              {"status", inc.getStatusString()}, {"isActive", inc.isActive()},
              {"verificationRequestID", inc.getVerificationRequestID()},
              {"riskScore", inc.getRiskScore()}, {"riskLevel", inc.getRiskLevel()},
              {"creationTimestamp", inc.getCreationTimestamp()},
              {"currentTier", inc.getCurrentTier()},
              {"tierDeadlineTimestamp", inc.getTierDeadlineTimestamp()},
              {"acknowledgedByContactID", inc.getAcknowledgedByContactID()},
              {"acknowledgedByName", inc.getAcknowledgedByName()},
              {"acknowledgementTimestamp", inc.getAcknowledgementTimestamp()},
              {"resolutionNotes", inc.getResolutionNotes()},
              {"resolutionTimestamp", inc.getResolutionTimestamp()},
              {"cancellationReason", inc.getCancellationReason()},
              {"cancellationTimestamp", inc.getCancellationTimestamp()},
              {"notificationCount", inc.getNotificationCount()}};
    json nots = json::array();
    for (const auto& n : inc.getNotificationHistory()) nots.push_back(notificationJ(n));
    j["notifications"] = nots;
    return j;
}

static json tryParse(const string& body) {
    try { return json::parse(body); }
    catch (...) { return json(); }
}

void registerApiRoutes(httplib::Server& svr, SafetySystem& ss, mutex& mtx) {

    // ===== GET ENDPOINTS =====

    svr.Get("/api/health", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(json({{"status","ok"},{"server","SafeSignalServer"},{"version","1.0"}}).dump(), "application/json");
    });

    svr.Get("/api/status", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        const auto& ra = ss.getRiskAnalyzer();
        auto analysis = ra.analyzeRisk();
        json pats = json::array();
        for (const auto& p : analysis.detectedPatterns) pats.push_back(p);
        json breakdown = json::array();
        for (const auto& b : analysis.scoringBreakdown) breakdown.push_back(b);
        json freqMap = json::object();
        for (const auto& kv : analysis.frequencyMap) freqMap[kv.first] = kv.second;

        json j = {
            {"riskScore", analysis.totalScore}, {"riskLevel", analysis.riskLevel},
            {"baseScore", analysis.baseScore}, {"frequencyBonus", analysis.frequencyBonus},
            {"patternBonus", analysis.patternBonus},
            {"isRiskHigh", ra.isRiskHigh()}, {"eventCount", ra.getEventCount()},
            {"detectedPatterns", pats}, {"scoringBreakdown", breakdown}, {"frequencyMap", freqMap},
            {"latestTimestamp", ra.getLatestTimestamp()},
            {"hasActiveJourney", ss.hasActiveJourney()},
            {"hasActiveVerification", ss.hasActiveVerification()},
            {"activeVerificationID", ss.getActiveVerificationID()},
            {"contactCount", ss.getResponseManager().getContactCount()},
            {"activeIncidentCount", ss.getResponseManager().getActiveIncidentCount()},
            {"pendingIncidentCount", ss.getResponseManager().getPendingIncidentCount()},
            {"pendingCheckIns", ss.getCheckInManager().getPendingCount()},
            {"missedCheckIns", ss.getCheckInManager().getMissedCount()},
            {"totalCheckIns", ss.getCheckInManager().getCheckInCount()}
        };
        res.set_content(j.dump(), "application/json");
    });

    svr.Get("/api/journey", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        const auto& jm = ss.getJourneyManager();
        json route = json::array();
        for (const auto& cp : jm.getPlannedRoute()) route.push_back(cp);
        json j = {
            {"active", jm.isActive()}, {"startLocation", jm.getStartLocation()},
            {"destination", jm.getDestination()}, {"expectedArrivalTime", jm.getExpectedArrivalTime()},
            {"route", route}, {"currentCheckpointIndex", jm.getCurrentCheckpointIndex()},
            {"nextExpectedCheckpoint", jm.getNextExpectedCheckpoint()},
            {"totalCheckpoints", (int)jm.getPlannedRoute().size()}
        };
        res.set_content(j.dump(), "application/json");
    });

    svr.Get("/api/contacts", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        const auto& contacts = ss.getResponseManager().getAllContacts();
        json arr = json::array();
        for (const auto& c : contacts) arr.push_back(contactJ(c));
        res.set_content(json({{"contacts", arr}, {"count", (int)contacts.size()}}).dump(), "application/json");
    });

    svr.Get("/api/checkins", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        const auto& cm = ss.getCheckInManager();
        const auto& cis = cm.getAllCheckIns();
        json arr = json::array();
        for (const auto& ci : cis) arr.push_back(checkInJ(ci));
        json j = {{"checkins", arr}, {"total", cm.getCheckInCount()},
                  {"pending", cm.getPendingCount()}, {"completed", cm.getCompletedCount()},
                  {"lateConfirmations", cm.getLateConfirmationCount()},
                  {"missed", cm.getMissedCount()}, {"gracePeriodMinutes", cm.getGracePeriod()}};
        res.set_content(j.dump(), "application/json");
    });

    svr.Get("/api/incidents/active", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        auto incidents = ss.getResponseManager().getActiveIncidents();
        json arr = json::array();
        for (const auto& inc : incidents) arr.push_back(incidentJ(inc));
        res.set_content(json({{"incidents", arr}, {"count", (int)incidents.size()}}).dump(), "application/json");
    });

    svr.Get("/api/history/events", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        auto events = ss.getRiskAnalyzer().getRecentEvents();
        json arr = json::array();
        for (const auto& e : events) arr.push_back(eventJ(e));
        res.set_content(json({{"events", arr}, {"count", (int)events.size()}}).dump(), "application/json");
    });

    svr.Get("/api/history/audit", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        auto incidents = ss.getResponseManager().getAllIncidents();
        json arr = json::array();
        json allNots = json::array();
        for (const auto& inc : incidents) {
            arr.push_back(incidentJ(inc));
            for (const auto& n : inc.getNotificationHistory()) {
                allNots.push_back(notificationJ(n));
            }
        }
        json verifs = json::array();
        auto allReqs = ss.getVerificationManager().getAllRequests();
        for (const auto& r : allReqs) verifs.push_back(verificationJ(r));
        res.set_content(json({{"incidents", arr}, {"notifications", allNots}, {"verifications", verifs}}).dump(), "application/json");
    });

    svr.Get("/api/verifications", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        const auto& vm = ss.getVerificationManager();
        json pending = json::array();
        for (const auto& r : vm.getPendingRequests()) pending.push_back(verificationJ(r));
        json all = json::array();
        for (const auto& r : vm.getAllRequests()) all.push_back(verificationJ(r));
        json j = {{"pending", pending}, {"all", all},
                  {"hasActive", vm.hasActivePendingRequest()}, {"activeID", vm.getActiveRequestID()},
                  {"totalCount", vm.getTotalRequestCount()}, {"pendingCount", vm.getPendingCount()},
                  {"confirmedSafeCount", vm.getConfirmedSafeCount()},
                  {"helpRequestedCount", vm.getHelpRequestedCount()},
                  {"timedOutCount", vm.getTimedOutCount()}};
        res.set_content(j.dump(), "application/json");
    });

    // ===== POST ENDPOINTS =====

    svr.Post("/api/journey/start", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("startLocation") || !body.contains("destination") ||
            !body.contains("eta") || !body.contains("route")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing required fields: startLocation, destination, eta, route"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        vector<string> route;
        for (const auto& cp : body["route"]) route.push_back(cp.get<string>());
        bool ok = ss.startJourney(body["startLocation"], body["destination"], body["eta"], route);
        res.set_content(json({{"success",ok},{"message", ok ? "Journey started successfully" : "Failed to start journey (a journey may already be active)"}}).dump(),"application/json");
    });

    svr.Post("/api/journey/location", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("location") || !body.contains("timestamp")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: location, timestamp"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        ss.updateJourneyLocation(body["location"], body["timestamp"]);
        res.set_content(json({{"success",true},{"message","Location updated"}}).dump(),"application/json");
    });

    svr.Post("/api/journey/end", [&](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(mtx);
        ss.endJourney();
        res.set_content(json({{"success",true},{"message","Journey ended"}}).dump(),"application/json");
    });

    svr.Post("/api/contacts", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("id") || !body.contains("name") ||
            !body.contains("phone") || !body.contains("relationship") || !body.contains("priority")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: id, name, phone, relationship, priority"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        Contact c(body["id"].get<int>(), body["name"].get<string>(), body["phone"].get<string>(),
                  body["relationship"].get<string>(), body["priority"].get<int>());
        ss.registerContact(c);
        res.set_content(json({{"success",true},{"message","Contact registered successfully"}}).dump(),"application/json");
    });

    svr.Post("/api/checkins/schedule", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("time")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: time"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        string desc = body.value("description", "Scheduled Check-in");
        string loc = body.value("location", "Unknown");
        int cid = ss.scheduleCheckIn(body["time"], desc, loc);
        bool ok = cid > 0;
        res.set_content(json({{"success",ok},{"checkInID",cid},{"message", ok ? "Check-in scheduled" : "Failed (journey may not be active)"}}).dump(),"application/json");
    });

    svr.Post("/api/checkins/confirm", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("checkInID")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: checkInID"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        bool ok = ss.completeCheckIn(body["checkInID"].get<int>());
        res.set_content(json({{"success",ok},{"message", ok ? "Check-in confirmed" : "Failed to confirm check-in"}}).dump(),"application/json");
    });

    svr.Post("/api/emergency/verify/confirm-safe", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        int rid = (body.is_null() || !body.contains("requestID")) ? -1 : body["requestID"].get<int>();
        lock_guard<mutex> lock(mtx);
        bool ok = ss.confirmUserSafe(rid);
        res.set_content(json({{"success",ok},{"message", ok ? "Safety confirmed — escalation averted" : "No active verification to confirm"}}).dump(),"application/json");
    });

    svr.Post("/api/emergency/verify/request-help", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        int rid = (body.is_null() || !body.contains("requestID")) ? -1 : body["requestID"].get<int>();
        lock_guard<mutex> lock(mtx);
        bool ok = ss.requestImmediateHelp(rid);
        res.set_content(json({{"success",ok},{"message", ok ? "Emergency help dispatched" : "No active verification request"}}).dump(),"application/json");
    });

    svr.Post("/api/emergency/sos", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        string loc = body.is_null() ? "Unknown Location" : body.value("location", "Unknown Location");
        string ts  = body.is_null() ? "12:00:00" : body.value("timestamp", "12:00:00");
        lock_guard<mutex> lock(mtx);
        if (ss.hasActiveVerification()) {
            bool ok = ss.requestImmediateHelp();
            res.set_content(json({{"success",ok},{"message","Emergency help requested via active verification"}}).dump(),"application/json");
        } else {
            int eid = autoEventID.fetch_add(1);
            Event sosEvent(eid, "SOS", loc, ts);
            ss.logEvent(sosEvent);
            if (ss.hasActiveVerification()) {
                ss.requestImmediateHelp();
            }
            res.set_content(json({{"success",true},{"message","SOS event logged and emergency dispatch initiated"},{"eventID",eid}}).dump(),"application/json");
        }
    });

    svr.Post("/api/incidents/acknowledge", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("incidentID") || !body.contains("contactID") || !body.contains("timestamp")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: incidentID, contactID, timestamp"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        bool ok = ss.acknowledgeIncident(body["incidentID"].get<int>(), body["contactID"].get<int>(), body["timestamp"].get<string>());
        res.set_content(json({{"success",ok},{"message", ok ? "Incident acknowledged" : "Failed to acknowledge incident"}}).dump(),"application/json");
    });

    svr.Post("/api/incidents/resolve", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("incidentID")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: incidentID"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        string notes = body.value("notes","");
        string ts = body.value("timestamp","");
        bool ok = ss.resolveIncident(body["incidentID"].get<int>(), notes, ts);
        res.set_content(json({{"success",ok},{"message", ok ? "Incident resolved" : "Failed to resolve incident"}}).dump(),"application/json");
    });

    svr.Post("/api/incidents/cancel", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("incidentID")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: incidentID"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        string reason = body.value("reason","");
        string ts = body.value("timestamp","");
        bool ok = ss.cancelIncident(body["incidentID"].get<int>(), reason, ts);
        res.set_content(json({{"success",ok},{"message", ok ? "Incident cancelled" : "Failed to cancel incident"}}).dump(),"application/json");
    });

    svr.Post("/api/sim/time", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("currentTime")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: currentTime"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        ss.processCurrentTimeUpdate(body["currentTime"].get<string>());
        res.set_content(json({{"success",true},{"message","Simulated time advanced to " + body["currentTime"].get<string>()}}).dump(),"application/json");
    });

    svr.Post("/api/sim/event", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = tryParse(req.body);
        if (body.is_null() || !body.contains("type") || !body.contains("location") || !body.contains("timestamp")) {
            res.status = 400;
            res.set_content(json({{"success",false},{"message","Missing: type, location, timestamp"}}).dump(),"application/json");
            return;
        }
        lock_guard<mutex> lock(mtx);
        int eid = body.contains("id") ? body["id"].get<int>() : autoEventID.fetch_add(1);
        Event ev(eid, body["type"].get<string>(), body["location"].get<string>(), body["timestamp"].get<string>());
        ss.logEvent(ev);
        int score = ss.getRiskAnalyzer().calculateRiskScore();
        res.set_content(json({{"success",true},{"eventID",eid},{"newRiskScore",score},{"message","Event ingested"}}).dump(),"application/json");
    });
}
