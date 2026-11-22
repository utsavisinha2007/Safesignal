#include <iostream>
#include <mutex>
#include <csignal>
#include "api_handlers.h"

using namespace std;

static httplib::Server* gServer = nullptr;

void handleSignal(int) {
    cout << "\n[SafeSignalServer] Shutting down..." << endl;
    if (gServer) gServer->stop();
}

int main() {
    SafetySystem safetySystem;
    mutex stateMutex;
    httplib::Server svr;
    gServer = &svr;

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    registerApiRoutes(svr, safetySystem, stateMutex);

    cout << "=============================================" << endl;
    cout << "  SafeSignal REST API Server v1.0" << endl;
    cout << "  http://127.0.0.1:8080" << endl;
    cout << "  Press Ctrl+C to stop" << endl;
    cout << "=============================================" << endl;

    if (!svr.listen("127.0.0.1", 8080)) {
        cerr << "[Error] Failed to start server on port 8080." << endl;
        return 1;
    }

    cout << "[SafeSignalServer] Stopped." << endl;
    return 0;
}
