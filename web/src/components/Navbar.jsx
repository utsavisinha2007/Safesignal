import React, { useState, useEffect } from 'react';
import { useLocation } from 'react-router-dom';
import {
  RefreshCw,
  Activity,
  ShieldCheck,
  AlertOctagon,
  Menu,
  Clock,
  Terminal,
  ShieldAlert,
  Server
} from 'lucide-react';

const routeTitles = {
  '/': { title: 'Emergency Command Centre', section: 'Tactical Overview' },
  '/journey': { title: 'Journey Monitoring', section: 'Route Progression & Deviation' },
  '/contacts': { title: 'Emergency Directory', section: '3-Tier Priority Contacts' },
  '/checkins': { title: 'Safety Check-ins', section: 'Automated 5-min Grace Deadlines' },
  '/emergency': { title: 'Emergency Response Centre', section: 'SOS & Priority Incident Triage' },
  '/history': { title: 'Activity & Audit History', section: 'Sliding Window & Dispatch Logs' },
  '/demo': { title: 'DSA Demonstration Lab', section: 'Algorithm & Memory Inspection' }
};

export default function Navbar({
  status,
  onRefresh,
  refreshing,
  serverConnected,
  setMobileOpen
}) {
  const location = useLocation();
  const currentRoute = routeTitles[location.pathname] || {
    title: 'Operations View',
    section: 'SafeSignal System'
  };

  const [currentTime, setCurrentTime] = useState('');

  useEffect(() => {
    const updateTime = () => {
      const now = new Date();
      setCurrentTime(now.toTimeString().split(' ')[0]);
    };
    updateTime();
    const interval = setInterval(updateTime, 1000);
    return () => clearInterval(interval);
  }, []);

  const riskScore = status?.riskScore ?? 0;
  const isHighRisk = status?.isRiskHigh ?? false;
  const hasVerification = status?.hasActiveVerification ?? false;

  return (
    <header className="h-16 bg-[#0D1220]/90 backdrop-blur-md border-b border-[rgba(148,163,184,0.12)] px-4 lg:px-6 flex items-center justify-between shrink-0 sticky top-0 z-30">
      {/* Left: Mobile hamburger & Breadcrumb */}
      <div className="flex items-center gap-3">
        <button
          onClick={() => setMobileOpen(true)}
          className="lg:hidden p-2 text-slate-400 hover:text-white rounded-lg hover:bg-slate-800/60"
        >
          <Menu className="w-5 h-5" />
        </button>

        <div>
          <div className="flex items-center gap-2 text-xs font-semibold text-slate-400">
            <span>SafeSignal OPS</span>
            <span className="text-slate-600">/</span>
            <span className="text-violet-400">{currentRoute.section}</span>
          </div>
          <h2 className="text-sm lg:text-base font-extrabold text-white tracking-tight">
            {currentRoute.title}
          </h2>
        </div>
      </div>

      {/* Right: Telemetry widgets */}
      <div className="flex items-center gap-3">
        {/* Live Digital Clock */}
        <div className="hidden sm:flex items-center gap-1.5 px-3 py-1.5 rounded-xl bg-[#111827] border border-[rgba(148,163,184,0.12)] text-xs font-mono text-slate-300">
          <Clock className="w-3.5 h-3.5 text-slate-400" />
          <span>{currentTime || '00:00:00'}</span>
        </div>

        {/* Backend Heartbeat Pill */}
        <div
          className={`hidden md:flex items-center gap-2 px-3 py-1.5 rounded-xl border text-xs font-mono ${
            serverConnected
              ? 'bg-[#111827] text-emerald-300 border-emerald-500/30'
              : 'bg-rose-950/40 text-rose-300 border-rose-500/40 animate-pulse'
          }`}
        >
          <Server className="w-3.5 h-3.5" />
          <span>{serverConnected ? 'C++ CORE :8080' : 'CORE OFFLINE'}</span>
        </div>

        {/* Tactical Risk Score Pill */}
        <div
          className={`flex items-center gap-2 px-3 py-1.5 rounded-xl text-xs font-bold border transition-all ${
            hasVerification || isHighRisk
              ? 'bg-rose-500/15 text-rose-300 border-rose-500/40 shadow-[0_0_15px_-3px_rgba(251,113,133,0.3)] animate-pulse'
              : riskScore >= 20
              ? 'bg-amber-500/15 text-amber-300 border-amber-500/30'
              : 'bg-emerald-500/15 text-emerald-300 border-emerald-500/30'
          }`}
        >
          {hasVerification || isHighRisk ? (
            <AlertOctagon className="w-3.5 h-3.5 text-rose-400" />
          ) : (
            <ShieldCheck className="w-3.5 h-3.5 text-emerald-400" />
          )}
          <span>
            RISK: <strong className="font-mono">{riskScore}/100</strong>
          </span>
        </div>

        {/* Refresh Action */}
        <button
          onClick={onRefresh}
          disabled={refreshing}
          className="p-2 text-slate-400 hover:text-violet-300 bg-[#111827] hover:bg-[#172033] border border-[rgba(148,163,184,0.14)] rounded-xl transition-all"
          title="Refresh C++ State"
        >
          <RefreshCw className={`w-4 h-4 ${refreshing ? 'animate-spin text-violet-400' : ''}`} />
        </button>

        {/* Demo Mode Badge */}
        <div className="hidden xl:flex items-center gap-1.5 px-3 py-1.5 bg-violet-600/15 text-violet-300 border border-violet-500/30 rounded-xl text-xs font-semibold">
          <Terminal className="w-3.5 h-3.5 text-violet-400" />
          <span>DSA DEFENSE MODE</span>
        </div>
      </div>
    </header>
  );
}
