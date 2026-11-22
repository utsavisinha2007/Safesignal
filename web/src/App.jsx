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
      <div className="min-h-screen bg-gray-50 flex flex-col text-gray-900">
        <DisclosureBanner />

        {/* Server Connection Disconnection Notice */}
        {!serverConnected && !initialLoading && (
          <div className="bg-red-600 text-white px-4 py-2.5 text-xs font-semibold flex items-center justify-between shadow-md">
            <div className="flex items-center gap-2">
              <AlertCircle className="w-4 h-4 animate-bounce" />
              <span>
                C++ REST API Server Disconnected (127.0.0.1:8080). Please ensure <code>SafeSignalServer.exe</code> is running.
              </span>
            </div>
            <button
              onClick={fetchGlobalState}
              className="px-3 py-1 bg-white text-red-700 rounded-lg text-xs font-bold hover:bg-red-50 flex items-center gap-1"
            >
              <RefreshCw className="w-3.5 h-3.5" />
              Retry Connection
            </button>
          </div>
        )}

        <div className="flex-1 flex overflow-hidden">
          <Sidebar
            activeVerification={status?.hasActiveVerification}
            activeIncidents={status?.activeIncidentCount}
          />

          <div className="flex-1 flex flex-col min-w-0 overflow-y-auto">
            <Navbar
              status={status}
              onRefresh={fetchGlobalState}
              refreshing={refreshing}
              serverConnected={serverConnected}
            />

            <main className="flex-1 p-6 max-w-7xl w-full mx-auto">
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
