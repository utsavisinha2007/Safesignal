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

    // 2. Resolve CORS configuration (supports Vercel frontend, local dev, or custom domain)
    const char* allowedOriginEnv = getenv("ALLOWED_ORIGIN");
    string configuredOrigin = (allowedOriginEnv != nullptr && strlen(allowedOriginEnv) > 0)
                                  ? allowedOriginEnv
                                  : "*";

    auto applyCors = [configuredOrigin](const httplib::Request& req, httplib::Response& res) {
        if (configuredOrigin != "*") {
            res.set_header("Access-Control-Allow-Origin", configuredOrigin);
        } else if (req.has_header("Origin")) {
            res.set_header("Access-Control-Allow-Origin", req.get_header_value("Origin"));
        } else {
            res.set_header("Access-Control-Allow-Origin", "*");
        }
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With, Accept");
        res.set_header("Access-Control-Max-Age", "86400");
    };

    svr.set_post_routing_handler([applyCors](const httplib::Request& req, httplib::Response& res) {
        applyCors(req, res);
    });

    svr.Options(".*", [applyCors](const httplib::Request& req, httplib::Response& res) {
        applyCors(req, res);
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
    cout << "  CORS Policy: " << (configuredOrigin == "*" ? "Wildcard / Dynamic Origin" : configuredOrigin) << endl;
    cout << "  Press Ctrl+C to stop" << endl;
    cout << "=============================================" << endl;

    if (!svr.listen(host.c_str(), port)) {
        cerr << "[Error] Failed to start server on " << host << ":" << port << endl;
        return 1;
    }

    cout << "[SafeSignalServer] Server stopped successfully." << endl;
    return 0;
}
