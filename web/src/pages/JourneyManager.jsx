import React, { useState } from 'react';
import {
  MapPin,
  Navigation,
  Plus,
  Trash2,
  CheckCircle2,
  AlertTriangle,
  Send,
  Flag,
  Radio
} from 'lucide-react';
import { api } from '../services/api';

export default function JourneyManager({ journey, onRefresh }) {
  const [startLoc, setStartLoc] = useState('');
  const [dest, setDest] = useState('');
  const [eta, setEta] = useState('18:00:00');
  const [checkpoints, setCheckpoints] = useState(['Main Gate', 'Library Quad']);
  const [simLocation, setSimLocation] = useState('');
  const [simTimestamp, setSimTimestamp] = useState('17:30:00');

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
        setFeedback({ type: 'success', msg: 'Safe Journey started! GPS route deviation monitoring is active.' });
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
      const res = await api.updateLocation({ location: simLocation, timestamp: simTimestamp });
      setFeedback({
        type: 'success',
        msg: `Simulated location updated to "${simLocation}". The C++ engine verified route progression.`
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
    if (!window.confirm('Are you sure you want to end this journey? Associated check-ins will be cleared.')) return;
    setLoading(true);
    setFeedback(null);
    try {
      await api.endJourney();
      setFeedback({ type: 'success', msg: 'Journey completed safely. Route tracking ended.' });
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-6">
      <div>
        <h1 className="page-title">Safe Journey Manager</h1>
        <p className="page-subtitle">
          Plan walking routes with milestones. SafeSignal monitors checkpoints and automatically detects skipped or deviated routes.
        </p>
      </div>

      {feedback && (
        <div
          className={`p-4 rounded-xl flex items-center justify-between text-sm ${
            feedback.type === 'success'
              ? 'bg-emerald-50 text-emerald-800 border border-emerald-200'
              : 'bg-red-50 text-red-800 border border-red-200'
          }`}
        >
          <div className="flex items-center gap-2">
            {feedback.type === 'success' ? (
              <CheckCircle2 className="w-5 h-5 text-emerald-600 shrink-0" />
            ) : (
              <AlertTriangle className="w-5 h-5 text-red-600 shrink-0" />
            )}
            <span>{feedback.msg}</span>
          </div>
          <button onClick={() => setFeedback(null)} className="text-xs font-semibold hover:underline">
            Dismiss
          </button>
        </div>
      )}

      {/* ACTIVE JOURNEY CARD */}
      {journey?.active ? (
        <div className="card border-violet-200 bg-gradient-to-br from-white to-violet-50/30">
          <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-6 border-b border-gray-100">
            <div>
              <span className="badge bg-emerald-100 text-emerald-800 border border-emerald-200 mb-2">
                ACTIVE JOURNEY UNDERWAY
              </span>
              <h2 className="text-xl font-bold text-gray-900">
                {journey.startLocation} → {journey.destination}
              </h2>
              <p className="text-xs text-gray-500 mt-1">
                Expected Arrival Time: <strong>{journey.expectedArrivalTime}</strong>
              </p>
            </div>
            <button onClick={handleEndJourney} disabled={loading} className="btn-danger text-sm self-start md:self-auto">
              End Journey Safely
            </button>
          </div>

          {/* CHECKPOINT PROGRESS STEPPER */}
          <div className="py-6">
            <h3 className="text-xs font-bold text-gray-500 uppercase tracking-wider mb-4">Route Checkpoints</h3>
            <div className="grid grid-cols-1 md:grid-cols-4 gap-3">
              {journey.route?.map((cp, idx) => {
                const isPassed = idx < journey.currentCheckpointIndex;
                const isCurrent = idx === journey.currentCheckpointIndex;
                return (
                  <div
                    key={idx}
                    className={`p-4 rounded-xl border transition-all ${
                      isPassed
                        ? 'bg-emerald-50 border-emerald-200 text-emerald-900'
                        : isCurrent
                        ? 'bg-violet-50 border-violet-300 text-violet-900 shadow-sm ring-2 ring-violet-200'
                        : 'bg-gray-50 border-gray-200 text-gray-500'
                    }`}
                  >
                    <div className="flex items-center justify-between text-xs font-bold mb-1">
                      <span>Checkpoint #{idx + 1}</span>
                      {isPassed ? (
                        <CheckCircle2 className="w-4 h-4 text-emerald-600" />
                      ) : isCurrent ? (
                        <Radio className="w-4 h-4 text-violet-600 animate-pulse" />
                      ) : (
                        <Flag className="w-4 h-4 text-gray-400" />
                      )}
                    </div>
                    <p className="text-sm font-semibold truncate">{cp}</p>
                    <span className="text-[10px] font-medium block mt-1">
                      {isPassed ? '✓ Passed' : isCurrent ? '● Expected Next' : 'Upcoming'}
                    </span>
                  </div>
                );
              })}
            </div>
          </div>

          {/* SIMULATED LOCATION UPDATE FORM */}
          <div className="mt-4 pt-6 border-t border-gray-100">
            <h3 className="text-xs font-bold text-gray-700 uppercase tracking-wider mb-2 flex items-center gap-1.5">
              <Navigation className="w-4 h-4 text-violet-600" />
              Simulate Location / GPS Step Along Route
            </h3>
            <p className="text-xs text-gray-500 mb-4">
              Enter your current simulated location. If you report a skipped checkpoint, the C++ forward recovery algorithm will skip forward. If you report an unexpected path, a Route Deviation event is raised.
            </p>

            <form onSubmit={handleUpdateLocation} className="grid grid-cols-1 md:grid-cols-3 gap-3">
              <div>
                <input
                  type="text"
                  placeholder="e.g. Library Quad or Unexpected Alley"
                  value={simLocation}
                  onChange={(e) => setSimLocation(e.target.value)}
                  className="input-field"
                  required
                />
              </div>
              <div>
                <input
                  type="text"
                  placeholder="Timestamp (HH:MM or HH:MM:SS)"
                  value={simTimestamp}
                  onChange={(e) => setSimTimestamp(e.target.value)}
                  className="input-field"
                  required
                />
              </div>
              <button type="submit" disabled={loading} className="btn-primary text-sm flex items-center justify-center gap-2">
                <Send className="w-4 h-4" />
                Submit Location Step
              </button>
            </form>

            {/* Quick Helper Buttons for Next Checkpoint */}
            {journey.nextExpectedCheckpoint && (
              <div className="mt-3 flex items-center gap-2 text-xs text-gray-600">
                <span>Quick Next:</span>
                <button
                  type="button"
                  onClick={() => setSimLocation(journey.nextExpectedCheckpoint)}
                  className="px-2.5 py-1 bg-white border border-gray-300 hover:border-violet-400 rounded-lg text-violet-700 font-medium"
                >
                  "{journey.nextExpectedCheckpoint}"
                </button>
                <button
                  type="button"
                  onClick={() => setSimLocation('Unknown Dark Alley')}
                  className="px-2.5 py-1 bg-white border border-red-200 hover:border-red-400 rounded-lg text-red-600 font-medium"
                >
                  Test Deviation: "Unknown Dark Alley"
                </button>
              </div>
            )}
          </div>
        </div>
      ) : (
        /* START NEW JOURNEY FORM */
        <div className="card">
          <h2 className="text-lg font-bold text-gray-900 mb-4 flex items-center gap-2">
            <Navigation className="w-5 h-5 text-violet-600" />
            Plan and Start a New Journey
          </h2>

          <form onSubmit={handleStartJourney} className="space-y-4">
            <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
              <div>
                <label className="block text-xs font-semibold text-gray-700 mb-1">Starting Point</label>
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
                <label className="block text-xs font-semibold text-gray-700 mb-1">Final Destination</label>
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
                <label className="block text-xs font-semibold text-gray-700 mb-1">Expected Arrival Time (ETA)</label>
                <input
                  type="text"
                  placeholder="HH:MM:SS (e.g. 18:30:00)"
                  value={eta}
                  onChange={(e) => setEta(e.target.value)}
                  className="input-field"
                  required
                />
              </div>
            </div>

            {/* Checkpoints Builder */}
            <div>
              <div className="flex items-center justify-between mb-2">
                <label className="block text-xs font-semibold text-gray-700">Planned Route Milestones / Checkpoints</label>
                <button
                  type="button"
                  onClick={addCheckpoint}
                  className="text-xs font-bold text-violet-600 hover:text-violet-800 flex items-center gap-1"
                >
                  <Plus className="w-3.5 h-3.5" />
                  Add Milestone
                </button>
              </div>

              <div className="space-y-2">
                {checkpoints.map((cp, idx) => (
                  <div key={idx} className="flex items-center gap-2">
                    <span className="text-xs font-semibold text-gray-400 w-6">#{idx + 1}</span>
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
                      className="p-2 text-gray-400 hover:text-red-500 disabled:opacity-30"
                    >
                      <Trash2 className="w-4 h-4" />
                    </button>
                  </div>
                ))}
              </div>
            </div>

            <button type="submit" disabled={loading} className="btn-primary w-full md:w-auto">
              Start Monitored Journey
            </button>
          </form>
        </div>
      )}
    </div>
  );
}
