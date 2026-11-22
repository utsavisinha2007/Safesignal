#ifndef API_HANDLERS_H
#define API_HANDLERS_H

#include <mutex>
#include "third_party/httplib.h"
#include "third_party/json.hpp"
#include "SafetySystem.h"

void registerApiRoutes(httplib::Server& svr, SafetySystem& safetySystem, std::mutex& mtx);

#endif
