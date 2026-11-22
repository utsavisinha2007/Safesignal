#include <iostream>
#include <mutex>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <string>
#include "api_handlers.h"

using namespace std;

static httplib::Server* gServer = nullptr;

void handleSignal(int) {
    cout << "\n[SafeSignalServer] Shutting down cleanly..." << endl;
    if (gServer) {
        gServer->stop();
    }
}

// Ensures a header key is present at most ONCE in the response multimap
inline void setHeaderUnique(httplib::Response& res, const string& key, const string& val) {
    res.headers.erase(key);
    res.set_header(key, val);
}

int main() {
    SafetySystem safetySystem;
    mutex stateMutex;
    httplib::Server svr;
    gServer = &svr;

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // 1. Resolve host and port from environment (Render compatibility & local dev default)
    const char* portEnv = getenv("PORT");
    int port = 8080;
    if (portEnv != nullptr && strlen(portEnv) > 0) {
        try {
            port = stoi(portEnv);
        } catch (...) {
            port = 8080;
        }
    }

    const char* hostEnv = getenv("HOST");
    string host = (hostEnv != nullptr && strlen(hostEnv) > 0) ? hostEnv : "0.0.0.0";

    // 2. Resolve CORS configuration for the production frontend
    const char* allowedOriginEnv = getenv("ALLOWED_ORIGIN");
    string configuredOrigin = (allowedOriginEnv != nullptr && strlen(allowedOriginEnv) > 0)
                                  ? allowedOriginEnv
                                  : "https://web-innovision7.vercel.app";

    auto isOriginAllowed = [configuredOrigin](const string& origin) -> bool {
        if (origin.empty()) return false;
        if (origin == "https://web-innovision7.vercel.app") return true;
        if (origin == "https://web-puce-six-93.vercel.app") return true;
        if (!configuredOrigin.empty() && configuredOrigin != "*" && origin == configuredOrigin) return true;
        if (origin == "http://localhost:5173" || origin == "http://127.0.0.1:5173" ||
            origin == "http://localhost:3000" || origin == "http://127.0.0.1:3000") {
            return true;
        }
        // Support Vercel deployment preview origins (e.g. https://web-*.vercel.app)
        if (origin.rfind("https://web-", 0) == 0 && origin.find(".vercel.app") != string::npos) {
            return true;
        }
        return false;
    };

    auto applyCors = [configuredOrigin, isOriginAllowed](const httplib::Request& req, httplib::Response& res) {
        string originToAllow = "https://web-innovision7.vercel.app";
        if (req.has_header("Origin")) {
            string reqOrigin = req.get_header_value("Origin");
            if (isOriginAllowed(reqOrigin)) {
                originToAllow = reqOrigin;
            }
        } else if (!configuredOrigin.empty() && configuredOrigin != "*") {
            originToAllow = configuredOrigin;
        }

        // Set headers strictly ONCE per response
        setHeaderUnique(res, "Access-Control-Allow-Origin", originToAllow);
        setHeaderUnique(res, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        setHeaderUnique(res, "Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With, Accept");
        setHeaderUnique(res, "Access-Control-Max-Age", "86400");
        setHeaderUnique(res, "Vary", "Origin");
    };

    // Global post-routing handler attaches CORS headers exactly once on ALL responses (including OPTIONS)
    svr.set_post_routing_handler([applyCors](const httplib::Request& req, httplib::Response& res) {
        applyCors(req, res);
    });

    // OPTIONS preflight handler only sets 204 status; post-routing handler attaches the CORS headers
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // 3. Root health checks for cloud load balancers & monitoring (Render / UptimeRobot)
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({"status":"ok","server":"SafeSignalServer","version":"1.0","service":"SafeSignal C++ Safety Engine API"})",
            "application/json"
        );
    });

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({"status":"ok","server":"SafeSignalServer","version":"1.0"})",
            "application/json"
        );
    });

    // 4. Register all SafeSignal API endpoints
    registerApiRoutes(svr, safetySystem, stateMutex);

    cout << "=============================================" << endl;
    cout << "  SafeSignal C++ REST API Server v1.0" << endl;
    cout << "  Host: " << host << " | Port: " << port << endl;
    cout << "  Listening on: http://" << (host == "0.0.0.0" ? "127.0.0.1" : host) << ":" << port << endl;
    cout << "  CORS Policy: " << configuredOrigin << endl;
    cout << "  Press Ctrl+C to stop" << endl;
    cout << "=============================================" << endl;

    if (!svr.listen(host.c_str(), port)) {
        cerr << "[Error] Failed to start server on " << host << ":" << port << endl;
        return 1;
    }

    cout << "[SafeSignalServer] Server stopped successfully." << endl;
    return 0;
}
