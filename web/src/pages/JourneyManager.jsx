import React, { useState } from 'react';
import {
  Navigation,
  MapPin,
  Plus,
  Trash2,
  CheckCircle2,
  AlertTriangle,
  Send,
  Flag,
  Radio,
  Compass,
  CornerDownRight,
  ShieldCheck,
  RotateCcw
} from 'lucide-react';
import { api } from '../services/api';

export default function JourneyManager({ journey, onRefresh }) {
  const [startLoc, setStartLoc] = useState('');
  const [dest, setDest] = useState('');
  const [eta, setEta] = useState('18:30:00');
  const [checkpoints, setCheckpoints] = useState(['North Gate', 'Central Library', 'Science Quad']);
  const [simLocation, setSimLocation] = useState('');
  const [simTimestamp, setSimTimestamp] = useState('18:15:00');

  const [loading, setLoading] = useState(false);
  const [feedback, setFeedback] = useState(null);

  const addCheckpoint = () => {
    setCheckpoints([...checkpoints, '']);
  };

  const removeCheckpoint = (idx) => {
    if (checkpoints.length <= 1) return;
    setCheckpoints(checkpoints.filter((_, i) => i !== idx));
  };

  const updateCheckpoint = (idx, val) => {
    const copy = [...checkpoints];
    copy[idx] = val;
    setCheckpoints(copy);
  };

  const handleStartJourney = async (e) => {
    e.preventDefault();
    if (!startLoc || !dest || !eta) {
      setFeedback({ type: 'error', msg: 'Please provide start location, destination, and ETA.' });
      return;
    }
    const cleanRoute = checkpoints.filter((c) => c.trim().length > 0);
    if (cleanRoute.length === 0) {
      setFeedback({ type: 'error', msg: 'Please add at least 1 route checkpoint.' });
      return;
    }

    setLoading(true);
    setFeedback(null);
    try {
      const res = await api.startJourney({
        startLocation: startLoc,
        destination: dest,
        eta,
        route: cleanRoute
      });
      if (res.success) {
        setFeedback({
          type: 'success',
          msg: 'Safe Journey active! C++ JourneyManager is monitoring checkpoints and route deviations.'
        });
        onRefresh();
      } else {
        setFeedback({ type: 'error', msg: res.message });
      }
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    } finally {
      setLoading(false);
    }
  };

  const handleUpdateLocation = async (e) => {
    e.preventDefault();
    if (!simLocation || !simTimestamp) {
      setFeedback({ type: 'error', msg: 'Please enter location and timestamp.' });
      return;
    }

    setLoading(true);
    setFeedback(null);
    try {
      await api.updateLocation({ location: simLocation, timestamp: simTimestamp });
      setFeedback({
        type: 'success',
        msg: `Simulated location updated: "${simLocation}" at ${simTimestamp}. Verified by C++ JourneyManager.`
      });
      setSimLocation('');
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    } finally {
      setLoading(false);
    }
  };

  const handleEndJourney = async () => {
    if (!window.confirm('End this journey? All associated check-in deadlines will be safely cleared.')) return;
    setLoading(true);
    setFeedback(null);
    try {
      await api.endJourney();
      setFeedback({ type: 'success', msg: 'Journey completed safely. Route monitoring stopped.' });
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-6">
      {/* Page Title */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-2 border-b border-[rgba(148,163,184,0.1)]">
        <div>
          <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-wider text-violet-400 mb-1">
            <Compass className="w-3.5 h-3.5 animate-spin-slow" />
            <span>Route Deviation Detection & Forward Recovery</span>
          </div>
          <h1 className="page-title">Journey Monitoring Command</h1>
          <p className="page-subtitle">
            Sequential milestone monitoring. Skipped checkpoints trigger lookahead recovery; unknown paths generate route-deviation hazard events.
          </p>
        </div>

        {journey?.active && (
          <button
            onClick={handleEndJourney}
            disabled={loading}
            className="btn-danger text-xs px-4 py-2 self-start md:self-auto"
          >
            <ShieldCheck className="w-3.5 h-3.5" />
            <span>END MISSION SAFELY</span>
          </button>
        )}
      </div>

      {feedback && (
        <div
          className={`p-4 rounded-xl flex items-center justify-between text-xs font-medium border ${
            feedback.type === 'success'
              ? 'bg-emerald-950/40 text-emerald-200 border-emerald-500/30'
              : 'bg-rose-950/40 text-rose-200 border-rose-500/30'
          }`}
        >
          <div className="flex items-center gap-2.5">
            {feedback.type === 'success' ? (
              <CheckCircle2 className="w-4 h-4 text-emerald-400 shrink-0" />
            ) : (
              <AlertTriangle className="w-4 h-4 text-rose-400 shrink-0" />
            )}
            <span>{feedback.msg}</span>
          </div>
          <button onClick={() => setFeedback(null)} className="text-[11px] underline text-slate-300 hover:text-white">
            Dismiss
          </button>
        </div>
      )}

      {/* ACTIVE JOURNEY TELEMETRY DISPLAY */}
      {journey?.active ? (
        <div className="space-y-6">
          <div className="cmd-card border-violet-500/30 space-y-6">
            <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-4 border-b border-[rgba(148,163,184,0.12)]">
              <div>
                <div className="inline-flex items-center gap-2 px-2.5 py-1 rounded-full text-xs font-bold bg-emerald-500/10 text-emerald-300 border border-emerald-500/30 mb-2">
                  <span className="w-2 h-2 rounded-full bg-emerald-400 animate-pulse" />
                  <span>ACTIVE JOURNEY IN PROGRESS</span>
                </div>
                <h2 className="text-xl font-extrabold text-white tracking-tight flex items-center gap-2">
                  <span>{journey.startLocation}</span>
                  <span className="text-violet-400">→</span>
                  <span className="text-emerald-300">{journey.destination}</span>
                </h2>
              </div>

              <div className="text-left md:text-right">
                <span className="text-xs text-slate-400 block">Expected Arrival Time</span>
                <span className="text-base font-mono font-bold text-violet-300">
                  {journey.expectedArrivalTime}
                </span>
              </div>
            </div>

            {/* ROUTE TIMELINE / MILESTONE VISUALIZATION */}
            <div className="space-y-3">
              <div className="flex items-center justify-between text-xs">
                <span className="font-bold text-slate-300 uppercase tracking-wider">
                  Checkpoint Progression Stepper
                </span>
                <span className="text-slate-400 font-mono text-[11px]">
                  Progress: {journey.currentCheckpointIndex} / {journey.totalCheckpoints} checkpoints
                </span>
              </div>

              <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-3">
                {journey.route?.map((cp, idx) => {
                  const isPassed = idx < journey.currentCheckpointIndex;
                  const isCurrent = idx === journey.currentCheckpointIndex;
                  return (
                    <div
                      key={idx}
                      className={`p-4 rounded-xl border transition-all ${
                        isPassed
                          ? 'bg-emerald-950/20 border-emerald-500/40 text-emerald-200'
                          : isCurrent
                          ? 'bg-violet-950/40 border-violet-400 text-white shadow-cmd-glow ring-1 ring-violet-400'
                          : 'bg-[#172033] border-[rgba(148,163,184,0.1)] text-slate-400'
                      }`}
                    >
                      <div className="flex items-center justify-between font-bold text-[10px] mb-1">
                        <span>CHECKPOINT #{idx + 1}</span>
                        {isPassed ? (
                          <CheckCircle2 className="w-4 h-4 text-emerald-400" />
                        ) : isCurrent ? (
                          <Radio className="w-4 h-4 text-violet-400 animate-pulse" />
                        ) : (
                          <Flag className="w-3.5 h-3.5 text-slate-600" />
                        )}
                      </div>
                      <p className="font-bold text-sm truncate mt-1 text-white">{cp}</p>
                      <span className="text-[10px] font-mono block mt-1.5">
                        {isPassed ? '✓ Confirmed Milestone' : isCurrent ? '● EXPECTED NEXT WAYPOINT' : 'Upcoming Path'}
                      </span>
                    </div>
                  );
                })}
              </div>
            </div>

            {/* SIMULATED LOCATION STEP INJECTOR */}
            <div className="pt-6 border-t border-[rgba(148,163,184,0.12)] space-y-3">
              <div>
                <h3 className="text-xs font-bold text-slate-200 uppercase tracking-wider flex items-center gap-1.5">
                  <Navigation className="w-4 h-4 text-violet-400" />
                  Simulate GPS Location Step Along Path
                </h3>
                <p className="text-xs text-slate-400 mt-0.5">
                  Input simulated location. If you report the expected milestone, the stepper advances. If you skip a waypoint, forward-recovery catches up. If you enter an unknown off-route location, a Route Deviation hazard is raised.
                </p>
              </div>

              <form onSubmit={handleUpdateLocation} className="grid grid-cols-1 sm:grid-cols-3 gap-3">
                <input
                  type="text"
                  placeholder="Location (e.g. Science Quad)"
                  value={simLocation}
                  onChange={(e) => setSimLocation(e.target.value)}
                  className="input-field"
                  required
                />
                <input
                  type="text"
                  placeholder="Timestamp (HH:MM:SS)"
                  value={simTimestamp}
                  onChange={(e) => setSimTimestamp(e.target.value)}
                  className="input-field font-mono"
                  required
                />
                <button type="submit" disabled={loading} className="btn-primary text-xs">
                  <Send className="w-3.5 h-3.5" />
                  <span>Transmit Location Step</span>
                </button>
              </form>

              {/* Fast Test Shortcut Pills */}
              <div className="flex flex-wrap items-center gap-2 pt-2 text-xs">
                <span className="text-slate-500 font-semibold text-[11px]">Quick Triggers:</span>
                {journey.nextExpectedCheckpoint && (
                  <button
                    type="button"
                    onClick={() => setSimLocation(journey.nextExpectedCheckpoint)}
                    className="px-3 py-1 bg-[#172033] hover:bg-[#1f2b45] border border-violet-500/30 rounded-lg text-violet-300 font-mono text-xs"
                  >
                    Match Next: "{journey.nextExpectedCheckpoint}"
                  </button>
                )}
                <button
                  type="button"
                  onClick={() => setSimLocation('Dark Alley Off-Campus')}
                  className="px-3 py-1 bg-rose-950/30 hover:bg-rose-950/50 border border-rose-500/40 rounded-lg text-rose-300 font-mono text-xs"
                >
                  Test Hazard: "Dark Alley Off-Campus"
                </button>
              </div>
            </div>
          </div>
        </div>
      ) : (
        /* START NEW JOURNEY PLANNER FORM */
        <div className="cmd-card space-y-6">
          <div className="flex items-center gap-2 border-b border-[rgba(148,163,184,0.12)] pb-4">
            <Navigation className="w-5 h-5 text-violet-400" />
            <div>
              <h2 className="text-base font-bold text-white">Plan & Register New Journey Route</h2>
              <p className="text-xs text-slate-400">Configure origin, destination, ETA, and sequential checkpoints</p>
            </div>
          </div>

          <form onSubmit={handleStartJourney} className="space-y-4">
            <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
              <div>
                <label className="block text-xs font-semibold text-slate-300 mb-1.5">Starting Location</label>
                <input
                  type="text"
                  placeholder="e.g. Science Library"
                  value={startLoc}
                  onChange={(e) => setStartLoc(e.target.value)}
                  className="input-field"
                  required
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-300 mb-1.5">Destination</label>
                <input
                  type="text"
                  placeholder="e.g. Dormitory Tower C"
                  value={dest}
                  onChange={(e) => setDest(e.target.value)}
                  className="input-field"
                  required
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-300 mb-1.5">Expected Arrival Time (ETA)</label>
                <input
                  type="text"
                  placeholder="HH:MM:SS (e.g. 18:30:00)"
                  value={eta}
                  onChange={(e) => setEta(e.target.value)}
                  className="input-field font-mono"
                  required
                />
              </div>
            </div>

            {/* Checkpoints Dynamic Builder */}
            <div className="pt-2">
              <div className="flex items-center justify-between mb-2">
                <label className="block text-xs font-semibold text-slate-300">
                  Sequential Route Checkpoints / Milestones
                </label>
                <button
                  type="button"
                  onClick={addCheckpoint}
                  className="text-xs font-bold text-violet-400 hover:text-violet-300 flex items-center gap-1"
                >
                  <Plus className="w-3.5 h-3.5" />
                  <span>Add Checkpoint</span>
                </button>
              </div>

              <div className="space-y-2">
                {checkpoints.map((cp, idx) => (
                  <div key={idx} className="flex items-center gap-2">
                    <span className="text-xs font-mono font-bold text-slate-500 w-8">#{idx + 1}</span>
                    <input
                      type="text"
                      placeholder={`Checkpoint #${idx + 1} (e.g. Campus Quad)`}
                      value={cp}
                      onChange={(e) => updateCheckpoint(idx, e.target.value)}
                      className="input-field flex-1"
                      required
                    />
                    <button
                      type="button"
                      onClick={() => removeCheckpoint(idx)}
                      disabled={checkpoints.length <= 1}
                      className="p-2 text-slate-500 hover:text-rose-400 disabled:opacity-30 transition-colors"
                      title="Remove checkpoint"
                    >
                      <Trash2 className="w-4 h-4" />
                    </button>
                  </div>
                ))}
              </div>
            </div>

            <div className="pt-4 border-t border-[rgba(148,163,184,0.12)]">
              <button type="submit" disabled={loading} className="btn-primary text-xs px-6 py-3">
                <Navigation className="w-4 h-4" />
                <span>START MONITORED SAFE JOURNEY</span>
              </button>
            </div>
          </form>
        </div>
      )}
    </div>
  );
}
