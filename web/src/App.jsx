import React, { useState, useEffect, useCallback } from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import Sidebar from './components/Sidebar';
import Navbar from './components/Navbar';
import DisclosureBanner from './components/DisclosureBanner';

import Dashboard from './pages/Dashboard';
import JourneyManager from './pages/JourneyManager';
import EmergencyContacts from './pages/EmergencyContacts';
import SafetyCheckins from './pages/SafetyCheckins';
import EmergencyCenter from './pages/EmergencyCenter';
import ActivityHistory from './pages/ActivityHistory';
import DemonstrationLab from './pages/DemonstrationLab';

import { api } from './services/api';
import { AlertCircle, RefreshCw } from 'lucide-react';

export default function App() {
  const [status, setStatus] = useState(null);
  const [journey, setJourney] = useState(null);
  const [serverConnected, setServerConnected] = useState(false);
  const [refreshing, setRefreshing] = useState(false);
  const [initialLoading, setInitialLoading] = useState(true);

  // Command center layout states
  const [collapsed, setCollapsed] = useState(false);
  const [mobileOpen, setMobileOpen] = useState(false);

  const fetchGlobalState = useCallback(async () => {
    try {
      setRefreshing(true);
      const [statusRes, journeyRes] = await Promise.all([
        api.getStatus(),
        api.getJourney()
      ]);
      setStatus(statusRes);
      setJourney(journeyRes);
      setServerConnected(true);
    } catch (err) {
      console.warn('Backend server currently unreachable:', err.message);
      setServerConnected(false);
    } finally {
      setRefreshing(false);
      setInitialLoading(false);
    }
  }, []);

  // Poll state every 3.5 seconds
  useEffect(() => {
    fetchGlobalState();
    const interval = setInterval(fetchGlobalState, 3500);
    return () => clearInterval(interval);
  }, [fetchGlobalState]);

  return (
    <BrowserRouter>
      <div className="min-h-screen bg-cmd-bg text-cmd-text flex flex-col font-sans selection:bg-cmd-primary/30 selection:text-white antialiased">
        {/* Compliance / Simulated Telemetry Banner */}
        <DisclosureBanner />

        {/* Server Disconnection Notice (Command Center Alert Banner) */}
        {!serverConnected && !initialLoading && (
          <div className="bg-cmd-critical/15 border-b border-cmd-critical/40 text-rose-300 px-4 py-2.5 text-xs font-mono flex items-center justify-between shadow-lg backdrop-blur-sm z-30">
            <div className="flex items-center gap-2">
              <AlertCircle className="w-4 h-4 text-cmd-critical animate-pulse" />
              <span>
                <strong className="text-rose-200">REST API OFFLINE:</strong> C++ Engine unreachable at <code>{import.meta.env.VITE_API_BASE_URL || '127.0.0.1:8080'}</code>. Ensure <code>SafeSignalServer</code> is running.
              </span>
            </div>
            <button
              onClick={fetchGlobalState}
              className="px-2.5 py-1 bg-cmd-critical/20 hover:bg-cmd-critical/30 text-rose-200 rounded border border-cmd-critical/40 text-xs font-bold flex items-center gap-1.5 transition-all"
            >
              <RefreshCw className="w-3 h-3 text-rose-300" />
              <span>Retry Link</span>
            </button>
          </div>
        )}

        <div className="flex-1 flex overflow-hidden relative">
          {/* Collapsible Command Center Sidebar */}
          <Sidebar
            collapsed={collapsed}
            setCollapsed={setCollapsed}
            mobileOpen={mobileOpen}
            setMobileOpen={setMobileOpen}
            activeVerification={status?.hasActiveVerification}
            activeIncidents={status?.activeIncidentCount}
            serverConnected={serverConnected}
          />

          <div className="flex-1 flex flex-col min-w-0 overflow-y-auto">
            {/* Command Center Top Navigation Bar */}
            <Navbar
              status={status}
              onRefresh={fetchGlobalState}
              refreshing={refreshing}
              serverConnected={serverConnected}
              toggleSidebar={() => setCollapsed((prev) => !prev)}
              setMobileOpen={setMobileOpen}
            />

            <main className="flex-1 p-4 md:p-6 max-w-7xl w-full mx-auto space-y-6">
              <Routes>
                <Route
                  path="/"
                  element={
                    <Dashboard
                      status={status}
                      journey={journey}
                      onRefresh={fetchGlobalState}
                    />
                  }
                />
                <Route
                  path="/journey"
                  element={
                    <JourneyManager
                      journey={journey}
                      onRefresh={fetchGlobalState}
                    />
                  }
                />
                <Route
                  path="/contacts"
                  element={<EmergencyContacts onRefresh={fetchGlobalState} />}
                />
                <Route
                  path="/checkins"
                  element={
                    <SafetyCheckins
                      journey={journey}
                      onRefresh={fetchGlobalState}
                    />
                  }
                />
                <Route
                  path="/emergency"
                  element={
                    <EmergencyCenter
                      status={status}
                      onRefresh={fetchGlobalState}
                    />
                  }
                />
                <Route path="/history" element={<ActivityHistory />} />
                <Route
                  path="/demo"
                  element={
                    <DemonstrationLab
                      status={status}
                      onRefresh={fetchGlobalState}
                    />
                  }
                />
              </Routes>
            </main>
          </div>
        </div>
      </div>
    </BrowserRouter>
  );
}
