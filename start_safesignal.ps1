# SafeSignal Unified Startup Script
# Starts both SafeSignal C++ REST API Server and Vite React Frontend

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "     SAFESIGNAL — WEB APPLICATION LAUNCHER               " -ForegroundColor Cyan
Write-Host "   Connecting React Frontend to C++17 Safety Engine       " -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. Check SafeSignalServer.exe
if (!(Test-Path ".\SafeSignalServer.exe")) {
    Write-Host "[Info] Compiling SafeSignalServer.exe..." -ForegroundColor Yellow
    g++ -std=c++17 -O2 -o SafeSignalServer.exe server/server_main.cpp server/api_handlers.cpp src/CheckInManager.cpp src/Contact.cpp src/Event.cpp src/Incident.cpp src/JourneyManager.cpp src/ResponseManager.cpp src/RiskAnalyzer.cpp src/SafetySystem.cpp src/VerificationManager.cpp -Iinclude -Iserver -lws2_32
    if (!$?) {
        Write-Host "[Error] Compilation of SafeSignalServer.exe failed!" -ForegroundColor Red
        exit 1
    }
}

# 2. Start C++ API Server
Write-Host "[1/2] Starting C++ REST API Server (http://127.0.0.1:8080)..." -ForegroundColor Green
$server = Start-Process -FilePath ".\SafeSignalServer.exe" -PassThru -NoNewWindow
Start-Sleep -Seconds 1

# 3. Start Frontend Development Server
Write-Host "[2/2] Launching React Development Server (http://localhost:5173)..." -ForegroundColor Green
Write-Host ""
Write-Host "Open your browser to: http://localhost:5173" -ForegroundColor White -BackgroundColor DarkBlue
Write-Host "Press Ctrl+C to terminate both servers." -ForegroundColor Gray
Write-Host ""

try {
    Set-Location "web"
    npm run dev
} finally {
    Write-Host "`nStopping SafeSignal C++ API Server..." -ForegroundColor Yellow
    Stop-Process -Id $server.Id -Force -ErrorAction SilentlyContinue
    Set-Location ".."
    Write-Host "SafeSignal shutdown complete." -ForegroundColor Green
}
