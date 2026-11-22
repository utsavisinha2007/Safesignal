import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  ShieldAlert,
  ShieldCheck,
  Navigation,
  Clock,
  Users,
  AlertTriangle,
  ArrowRight,
  CheckCircle2,
  XCircle,
  AlertOctagon,
  Radio,
  Activity,
  Zap,
  Flame,
  Layers,
  ArrowUpRight
} from 'lucide-react';
import { api } from '../services/api';
import StatusBadge from '../components/StatusBadge';

export default function Dashboard({ status, journey, onRefresh }) {
  const navigate = useNavigate();
  const [loadingAction, setLoadingAction] = useState(false);
  const [actionFeedback, setActionFeedback] = useState(null);

  const riskScore = status?.riskScore ?? 0;
  const riskLevel = status?.riskLevel || 'Normal';
  const isHighRisk = status?.isRiskHigh ?? false;
  const hasVerification = status?.hasActiveVerification ?? false;
  const activeVerifID = status?.activeVerificationID;

  const handleConfirmSafe = async () => {
    setLoadingAction(true);
    setActionFeedback(null);
    try {
      const res = await api.confirmSafe(activeVerifID);
      setActionFeedback({ type: 'success', msg: res.message });
      onRefresh();
    } catch (err) {
      setActionFeedback({ type: 'error', msg: err.message });
    } finally {
      setLoadingAction(false);
    }
  };

  const handleRequestHelp = async () => {
    setLoadingAction(true);
    setActionFeedback(null);
    try {
      const res = await api.requestHelp(activeVerifID);
      setActionFeedback({ type: 'error', msg: res.message });
      onRefresh();
    } catch (err) {
      setActionFeedback({ type: 'error', msg: err.message });
    } finally {
      setLoadingAction(false);
    }
  };

  return (
    <div className="space-y-6">
      {/* Tactical Operational Greeting */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-2 border-b border-[rgba(148,163,184,0.1)]">
        <div>
          <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-wider text-violet-400 mb-1">
            <Radio className="w-3.5 h-3.5 animate-pulse" />
            <span>Real-Time Situational Awareness</span>
          </div>
          <h1 className="text-2xl lg:text-3xl font-black text-white tracking-tight">
            Safety Command Centre
          </h1>
          <p className="text-xs lg:text-sm text-slate-400 mt-1 max-w-2xl">
            Autonomous threat detection, sliding-window sensor analysis, and priority incident triage for solo campus travelers.
          </p>
        </div>

        <div className="flex items-center gap-2 self-start md:self-auto">
          <button
            onClick={() => navigate('/emergency')}
            className="btn-danger text-xs px-4 py-2"
          >
            <AlertTriangle className="w-3.5 h-3.5" />
            <span>ONE-TOUCH SOS</span>
          </button>
        </div>
      </div>

      {/* Action Notification Feedback */}
      {actionFeedback && (
        <div
          className={`p-4 rounded-xl flex items-center justify-between text-xs font-medium border ${
            actionFeedback.type === 'success'
              ? 'bg-emerald-950/40 text-emerald-200 border-emerald-500/30'
              : 'bg-rose-950/40 text-rose-200 border-rose-500/30'
          }`}
        >
          <div className="flex items-center gap-2.5">
            {actionFeedback.type === 'success' ? (
              <CheckCircle2 className="w-4 h-4 text-emerald-400 shrink-0" />
            ) : (
              <XCircle className="w-4 h-4 text-rose-400 shrink-0" />
            )}
            <span>{actionFeedback.msg}</span>
          </div>
          <button onClick={() => setActionFeedback(null)} className="text-[11px] underline text-slate-300 hover:text-white">
            Dismiss
          </button>
        </div>
      )}

      {/* CRITICAL VERIFICATION HERO BANNER */}
      {hasVerification && (
        <div className="p-6 rounded-2xl bg-gradient-to-r from-rose-950/80 via-red-950/60 to-[#111827] border-2 border-rose-500 shadow-danger-glow relative overflow-hidden">
          <div className="absolute top-0 right-0 p-8 opacity-10 pointer-events-none">
            <AlertOctagon className="w-40 h-40 text-rose-500 animate-pulse" />
          </div>

          <div className="relative z-10 flex flex-col lg:flex-row lg:items-center justify-between gap-6">
            <div className="space-y-2">
              <div className="inline-flex items-center gap-2 px-2.5 py-1 rounded-full text-xs font-extrabold bg-rose-500/20 text-rose-200 border border-rose-500/40">
                <span className="w-2 h-2 rounded-full bg-rose-400 animate-ping" />
                <span>SAFETY VERIFICATION #{activeVerifID} PENDING</span>
              </div>
              <h2 className="text-xl font-black text-white tracking-tight">
                Abnormal Risk Threshold Exceeded ({riskScore}/100)
              </h2>
              <p className="text-xs text-rose-200/80 max-w-2xl leading-relaxed">
                SafeSignal detected unusual danger patterns and has initiated a 3-minute safety verification window. Confirm you are safe to cancel automated emergency escalation to your Tier 1 contacts.
              </p>
            </div>

            <div className="flex flex-col sm:flex-row gap-3 shrink-0">
              <button
                onClick={handleConfirmSafe}
                disabled={loadingAction}
                className="px-5 py-3 bg-emerald-600 hover:bg-emerald-500 active:scale-[0.98] text-white font-bold rounded-xl text-xs flex items-center justify-center gap-2 shadow-lg shadow-emerald-950/60 border border-emerald-400/40"
              >
                <CheckCircle2 className="w-4 h-4" />
                <span>I AM SAFE (CANCEL ALERT)</span>
              </button>
              <button
                onClick={handleRequestHelp}
                disabled={loadingAction}
                className="px-5 py-3 bg-rose-700 hover:bg-rose-600 active:scale-[0.98] text-white font-bold rounded-xl text-xs flex items-center justify-center gap-2 shadow-lg shadow-rose-950/80 border border-rose-400/40"
              >
                <AlertTriangle className="w-4 h-4" />
                <span>DISPATCH EMERGENCY HELP</span>
              </button>
            </div>
          </div>
        </div>
      )}

      {/* CORE 4 TELEMETRY STATS TILES */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
        {/* Metric 1: Risk Assessment */}
        <div className="cmd-card flex flex-col justify-between">
          <div className="flex items-center justify-between">
            <span className="text-[11px] font-bold uppercase tracking-wider text-slate-400">
              Risk Level
            </span>
            <span
              className={`p-2 rounded-xl ${
                riskScore >= 50
                  ? 'bg-rose-500/20 text-rose-300'
                  : riskScore >= 20
                  ? 'bg-amber-500/20 text-amber-300'
                  : 'bg-emerald-500/20 text-emerald-300'
              }`}
            >
              {riskScore >= 50 ? <ShieldAlert className="w-4 h-4" /> : <ShieldCheck className="w-4 h-4" />}
            </span>
          </div>
          <div className="mt-3">
            <div className="flex items-baseline gap-2">
              <span className="text-3xl font-black font-mono text-white">{riskScore}</span>
              <span className="text-xs text-slate-400 font-mono">/ 100</span>
            </div>
            <span
              className={`text-xs font-bold mt-1 block ${
                riskScore >= 50
                  ? 'text-rose-400'
                  : riskScore >= 20
                  ? 'text-amber-400'
                  : 'text-emerald-400'
              }`}
            >
              {riskLevel.toUpperCase()}
            </span>
          </div>
          {/* Progress bar */}
          <div className="w-full bg-slate-800/80 h-1.5 rounded-full overflow-hidden mt-3">
            <div
              className={`h-full transition-all duration-500 ${
                riskScore >= 50
                  ? 'bg-rose-500 shadow-[0_0_8px_rgba(244,63,94,0.8)]'
                  : riskScore >= 20
                  ? 'bg-amber-400 shadow-[0_0_8px_rgba(251,191,36,0.8)]'
                  : 'bg-emerald-400 shadow-[0_0_8px_rgba(52,211,153,0.8)]'
              }`}
              style={{ width: `${Math.min(100, Math.max(5, riskScore))}%` }}
            />
          </div>
        </div>

        {/* Metric 2: Active Journey */}
        <div className="cmd-card flex flex-col justify-between">
          <div className="flex items-center justify-between">
            <span className="text-[11px] font-bold uppercase tracking-wider text-slate-400">
              Journey Status
            </span>
            <span className="p-2 rounded-xl bg-violet-500/20 text-violet-300">
              <Navigation className="w-4 h-4" />
            </span>
          </div>
          <div className="mt-3">
            <div className="text-xl font-extrabold text-white truncate">
              {journey?.active ? journey.destination : 'No Active Route'}
            </div>
            <p className="text-xs text-slate-400 mt-1 truncate">
              {journey?.active ? `Next: ${journey.nextExpectedCheckpoint || 'Destination'}` : 'Waypoints idle'}
            </p>
          </div>
          <button
            onClick={() => navigate('/journey')}
            className="text-[11px] font-bold text-violet-400 hover:text-violet-300 flex items-center gap-1 mt-3"
          >
            <span>{journey?.active ? 'Inspect Waypoints' : 'Start Journey'}</span>
            <ArrowRight className="w-3 h-3" />
          </button>
        </div>

        {/* Metric 3: Safety Check-ins */}
        <div className="cmd-card flex flex-col justify-between">
          <div className="flex items-center justify-between">
            <span className="text-[11px] font-bold uppercase tracking-wider text-slate-400">
              Safety Check-ins
            </span>
            <span className="p-2 rounded-xl bg-teal-500/20 text-teal-300">
              <Clock className="w-4 h-4" />
            </span>
          </div>
          <div className="mt-3">
            <div className="flex items-baseline gap-2">
              <span className="text-3xl font-black font-mono text-white">{status?.pendingCheckIns ?? 0}</span>
              <span className="text-xs text-slate-400">Pending</span>
            </div>
            <p className="text-xs text-slate-400 mt-1">
              Missed: <strong className="text-rose-400 font-mono">{status?.missedCheckIns ?? 0}</strong> • Total: <strong className="font-mono text-slate-200">{status?.totalCheckIns ?? 0}</strong>
            </p>
          </div>
          <button
            onClick={() => navigate('/checkins')}
            className="text-[11px] font-bold text-teal-400 hover:text-teal-300 flex items-center gap-1 mt-3"
          >
            <span>Manage Check-ins</span>
            <ArrowRight className="w-3 h-3" />
          </button>
        </div>

        {/* Metric 4: Active Incidents */}
        <div className="cmd-card flex flex-col justify-between">
          <div className="flex items-center justify-between">
            <span className="text-[11px] font-bold uppercase tracking-wider text-slate-400">
              Active Emergencies
            </span>
            <span
              className={`p-2 rounded-xl ${
                (status?.activeIncidentCount ?? 0) > 0
                  ? 'bg-rose-500/20 text-rose-300 animate-pulse'
                  : 'bg-slate-800 text-slate-400'
              }`}
            >
              <AlertTriangle className="w-4 h-4" />
            </span>
          </div>
          <div className="mt-3">
            <div className="flex items-baseline gap-2">
              <span
                className={`text-3xl font-black font-mono ${
                  (status?.activeIncidentCount ?? 0) > 0 ? 'text-rose-400' : 'text-white'
                }`}
              >
                {status?.activeIncidentCount ?? 0}
              </span>
              <span className="text-xs text-slate-400">In Triage</span>
            </div>
            <p className="text-xs text-slate-400 mt-1">
              Contacts: <strong className="text-slate-200 font-mono">{status?.contactCount ?? 0}</strong> registered
            </p>
          </div>
          <button
            onClick={() => navigate('/emergency')}
            className="text-[11px] font-bold text-rose-400 hover:text-rose-300 flex items-center gap-1 mt-3"
          >
            <span>Response Center</span>
            <ArrowRight className="w-3 h-3" />
          </button>
        </div>
      </div>

      {/* MAIN TWO-COLUMN COMMAND CONSOLE */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Left 2 Cols: Live Risk & Journey Telemetry */}
        <div className="lg:col-span-2 space-y-6">
          {/* Detailed Risk Diagnostics Card */}
          <div className="cmd-card space-y-4">
            <div className="flex items-center justify-between border-b border-[rgba(148,163,184,0.1)] pb-3">
              <div className="flex items-center gap-2">
                <Activity className="w-4 h-4 text-violet-400" />
                <h3 className="text-sm font-bold text-white uppercase tracking-wider">
                  Sliding-Window Risk Analytics (C++ std::deque)
                </h3>
              </div>
              <span className="text-xs font-mono text-slate-400">10-min Window</span>
            </div>

            <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
              <div className="cmd-card-elevated">
                <span className="text-[10px] font-bold uppercase tracking-wider text-slate-400 block mb-1">
                  Base Severity
                </span>
                <span className="text-xl font-bold font-mono text-white">{status?.baseScore ?? 0}</span>
                <span className="text-[10px] text-slate-500 block mt-1">Event severity sum</span>
              </div>
              <div className="cmd-card-elevated">
                <span className="text-[10px] font-bold uppercase tracking-wider text-slate-400 block mb-1">
                  Frequency Bonus
                </span>
                <span className="text-xl font-bold font-mono text-violet-300">+{status?.frequencyBonus ?? 0}</span>
                <span className="text-[10px] text-slate-500 block mt-1">Rapid cluster multiplier</span>
              </div>
              <div className="cmd-card-elevated">
                <span className="text-[10px] font-bold uppercase tracking-wider text-slate-400 block mb-1">
                  Pattern Bonus
                </span>
                <span className="text-xl font-bold font-mono text-amber-300">+{status?.patternBonus ?? 0}</span>
                <span className="text-[10px] text-slate-500 block mt-1">Danger sequence bonus</span>
              </div>
            </div>

            {/* Detected Abnormal Patterns Alert */}
            {status?.detectedPatterns?.length > 0 ? (
              <div className="p-3 rounded-xl bg-rose-500/10 border border-rose-500/30 text-xs text-rose-300 space-y-1">
                <div className="font-bold flex items-center gap-1.5 text-rose-400">
                  <Flame className="w-3.5 h-3.5" />
                  <span>DANGER PATTERNS RECOGNIZED BY C++ HEURISTICS:</span>
                </div>
                {status.detectedPatterns.map((pat, idx) => (
                  <p key={idx} className="font-mono text-[11px] pl-5">• {pat}</p>
                ))}
              </div>
            ) : (
              <div className="p-3 rounded-xl bg-[#172033] border border-[rgba(148,163,184,0.1)] text-xs text-slate-400 flex items-center gap-2">
                <CheckCircle2 className="w-4 h-4 text-emerald-400 shrink-0" />
                <span>No dangerous event clusters or abnormal movement patterns detected in active window.</span>
              </div>
            )}
          </div>

          {/* Active Journey Waypoint Tracker */}
          <div className="cmd-card space-y-4">
            <div className="flex items-center justify-between border-b border-[rgba(148,163,184,0.1)] pb-3">
              <div className="flex items-center gap-2">
                <Navigation className="w-4 h-4 text-violet-400" />
                <h3 className="text-sm font-bold text-white uppercase tracking-wider">
                  Active Journey Waypoint Tracker
                </h3>
              </div>
              {journey?.active && (
                <span className="cmd-badge bg-emerald-500/15 text-emerald-300 border-emerald-500/30">
                  MONITORING ACTIVE
                </span>
              )}
            </div>

            {journey?.active ? (
              <div className="space-y-4">
                <div className="flex items-center justify-between text-xs">
                  <div>
                    <span className="text-slate-400">Route:</span>{' '}
                    <strong className="text-white font-semibold">{journey.startLocation} → {journey.destination}</strong>
                  </div>
                  <div>
                    <span className="text-slate-400">Target ETA:</span>{' '}
                    <span className="font-mono text-violet-300 font-bold">{journey.expectedArrivalTime}</span>
                  </div>
                </div>

                {/* Stepper */}
                <div className="grid grid-cols-1 sm:grid-cols-3 gap-3">
                  {journey.route?.map((cp, idx) => {
                    const isPassed = idx < journey.currentCheckpointIndex;
                    const isCurrent = idx === journey.currentCheckpointIndex;
                    return (
                      <div
                        key={idx}
                        className={`p-3 rounded-xl border text-xs ${
                          isPassed
                            ? 'bg-emerald-950/20 border-emerald-500/30 text-emerald-300'
                            : isCurrent
                            ? 'bg-violet-950/30 border-violet-400/50 text-white shadow-cmd-glow ring-1 ring-violet-500/40'
                            : 'bg-[#172033] border-[rgba(148,163,184,0.1)] text-slate-500'
                        }`}
                      >
                        <div className="flex items-center justify-between font-bold text-[10px] mb-1">
                          <span>MILESTONE #{idx + 1}</span>
                          {isPassed ? (
                            <CheckCircle2 className="w-3.5 h-3.5 text-emerald-400" />
                          ) : isCurrent ? (
                            <Radio className="w-3.5 h-3.5 text-violet-400 animate-pulse" />
                          ) : null}
                        </div>
                        <p className="font-semibold text-sm truncate">{cp}</p>
                        <span className="text-[10px] block mt-1 font-mono">
                          {isPassed ? '✓ Confirmed' : isCurrent ? '● Expected Next' : 'Pending'}
                        </span>
                      </div>
                    );
                  })}
                </div>
              </div>
            ) : (
              <div className="text-center py-8 text-slate-500 space-y-2">
                <Navigation className="w-8 h-8 text-slate-600 mx-auto" />
                <p className="text-xs text-slate-400 font-medium">No active journey currently registered.</p>
                <button
                  onClick={() => navigate('/journey')}
                  className="btn-primary text-xs mx-auto mt-2"
                >
                  Plan & Start Route
                </button>
              </div>
            )}
          </div>
        </div>

        {/* Right Col: Tactical Action Panel & Emergency Escalation Status */}
        <div className="space-y-6">
          {/* Tactical Emergency Escalation Overview */}
          <div className="cmd-card space-y-4">
            <div className="flex items-center justify-between border-b border-[rgba(148,163,184,0.1)] pb-3">
              <div className="flex items-center gap-2">
                <AlertTriangle className="w-4 h-4 text-rose-400" />
                <h3 className="text-sm font-bold text-white uppercase tracking-wider">
                  Emergency Triage Status
                </h3>
              </div>
            </div>

            <div className="space-y-3">
              <div className="p-3 rounded-xl bg-[#172033] border border-[rgba(148,163,184,0.12)] space-y-1.5">
                <div className="flex items-center justify-between text-xs">
                  <span className="text-slate-400">Priority Queue Heap:</span>
                  <span className="font-mono font-bold text-white">
                    {status?.pendingIncidentCount ?? 0} In Queue
                  </span>
                </div>
                <div className="flex items-center justify-between text-xs">
                  <span className="text-slate-400">Actively Dispatching:</span>
                  <span className="font-mono font-bold text-rose-400">
                    {status?.activeIncidentCount ?? 0} Incidents
                  </span>
                </div>
              </div>

              <div className="p-3 rounded-xl bg-[#172033] border border-[rgba(148,163,184,0.12)] text-xs space-y-2">
                <span className="text-slate-400 font-semibold block text-[11px] uppercase tracking-wider">
                  Tier Escalation Policy
                </span>
                <div className="space-y-1 text-[11px] text-slate-300 font-mono">
                  <p>• Tier 1: Primary Family (Immediate)</p>
                  <p>• Tier 2: Secondary Friends (After 2m)</p>
                  <p>• Tier 3: Campus Police / 911 (After 4m)</p>
                </div>
              </div>

              <button
                onClick={() => navigate('/emergency')}
                className="btn-danger w-full text-xs font-bold"
              >
                <AlertTriangle className="w-4 h-4" />
                <span>OPEN EMERGENCY RESPONSE CENTRE</span>
              </button>
            </div>
          </div>

          {/* Quick Command Launcher */}
          <div className="cmd-card space-y-3">
            <h3 className="text-xs font-bold text-slate-400 uppercase tracking-wider">
              Quick Operations Hub
            </h3>
            <div className="grid grid-cols-2 gap-2">
              <button
                onClick={() => navigate('/journey')}
                className="p-3 bg-[#172033] hover:bg-[#1f2b45] border border-[rgba(148,163,184,0.14)] rounded-xl text-left transition-all group"
              >
                <Navigation className="w-4 h-4 text-violet-400 mb-1 group-hover:scale-110 transition-transform" />
                <span className="text-xs font-bold text-white block">Journey</span>
                <span className="text-[10px] text-slate-400">Route Tracking</span>
              </button>

              <button
                onClick={() => navigate('/checkins')}
                className="p-3 bg-[#172033] hover:bg-[#1f2b45] border border-[rgba(148,163,184,0.14)] rounded-xl text-left transition-all group"
              >
                <Clock className="w-4 h-4 text-teal-400 mb-1 group-hover:scale-110 transition-transform" />
                <span className="text-xs font-bold text-white block">Check-in</span>
                <span className="text-[10px] text-slate-400">5-min Grace</span>
              </button>

              <button
                onClick={() => navigate('/contacts')}
                className="p-3 bg-[#172033] hover:bg-[#1f2b45] border border-[rgba(148,163,184,0.14)] rounded-xl text-left transition-all group"
              >
                <Users className="w-4 h-4 text-blue-400 mb-1 group-hover:scale-110 transition-transform" />
                <span className="text-xs font-bold text-white block">Contacts</span>
                <span className="text-[10px] text-slate-400">3 Tiers</span>
              </button>

              <button
                onClick={() => navigate('/demo')}
                className="p-3 bg-[#172033] hover:bg-[#1f2b45] border border-[rgba(148,163,184,0.14)] rounded-xl text-left transition-all group"
              >
                <Zap className="w-4 h-4 text-amber-400 mb-1 group-hover:scale-110 transition-transform" />
                <span className="text-xs font-bold text-white block">DSA Lab</span>
                <span className="text-[10px] text-slate-400">Simulate Events</span>
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
