import React, { useState, useEffect } from 'react';
import { Clock, CheckCircle2, AlertTriangle, Plus, FastForward, Check } from 'lucide-react';
import { api } from '../services/api';
import StatusBadge from '../components/StatusBadge';

export default function SafetyCheckins({ journey, onRefresh }) {
  const [data, setData] = useState({ checkins: [], stats: {} });
  const [loading, setLoading] = useState(true);
  const [feedback, setFeedback] = useState(null);

  // New check-in form
  const [schedTime, setSchedTime] = useState('18:15:00');
  const [description, setDescription] = useState('Crossing North Bridge');
  const [location, setLocation] = useState('North Bridge');

  // Sim time control
  const [advanceTimeStr, setAdvanceTimeStr] = useState('18:25:00');

  const fetchCheckins = async () => {
    try {
      setLoading(true);
      const res = await api.getCheckins();
      setData(res);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchCheckins();
  }, []);

  const handleSchedule = async (e) => {
    e.preventDefault();
    if (!schedTime) {
      setFeedback({ type: 'error', msg: 'Scheduled time is required.' });
      return;
    }

    try {
      const res = await api.scheduleCheckin({
        time: schedTime,
        description,
        location
      });
      if (res.success) {
        setFeedback({ type: 'success', msg: `Check-in #${res.checkInID} scheduled for ${schedTime}!` });
        fetchCheckins();
        onRefresh();
      } else {
        setFeedback({ type: 'error', msg: res.message });
      }
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleConfirm = async (id) => {
    try {
      const res = await api.confirmCheckin({ checkInID: id });
      if (res.success) {
        setFeedback({ type: 'success', msg: `Check-in #${id} confirmed! Contacts know you are safe.` });
        fetchCheckins();
        onRefresh();
      } else {
        setFeedback({ type: 'error', msg: res.message });
      }
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleAdvanceTime = async (e) => {
    e.preventDefault();
    try {
      const res = await api.advanceTime(advanceTimeStr);
      setFeedback({
        type: 'success',
        msg: `Simulated clock advanced to ${advanceTimeStr}. The C++ CheckInManager evaluated grace periods.`
      });
      fetchCheckins();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  return (
    <div className="space-y-6">
      <div>
        <h1 className="page-title">Safety Check-ins Tracker</h1>
        <p className="page-subtitle">
          Periodic check-in deadlines with an automatic 5-minute grace period. Missed check-ins raise automated safety alerts.
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

      {/* STATS CARDS */}
      <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
        <div className="card text-center p-4">
          <span className="text-xs text-gray-500 font-bold uppercase">Total Scheduled</span>
          <p className="text-2xl font-extrabold text-gray-900 mt-1">{data.stats?.total ?? 0}</p>
        </div>
        <div className="card text-center p-4">
          <span className="text-xs text-amber-600 font-bold uppercase">Pending</span>
          <p className="text-2xl font-extrabold text-amber-600 mt-1">{data.stats?.pending ?? 0}</p>
        </div>
        <div className="card text-center p-4">
          <span className="text-xs text-emerald-600 font-bold uppercase">On-Time</span>
          <p className="text-2xl font-extrabold text-emerald-600 mt-1">{data.stats?.completed ?? 0}</p>
        </div>
        <div className="card text-center p-4">
          <span className="text-xs text-purple-600 font-bold uppercase">Late Confirm</span>
          <p className="text-2xl font-extrabold text-purple-600 mt-1">{data.stats?.late ?? 0}</p>
        </div>
        <div className="card text-center p-4">
          <span className="text-xs text-red-600 font-bold uppercase">Missed</span>
          <p className="text-2xl font-extrabold text-red-600 mt-1">{data.stats?.missed ?? 0}</p>
        </div>
      </div>

      {/* SCHEDULE FORM & SIMULATED TIME BAR */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* Schedule form */}
        <div className="card md:col-span-2">
          <h2 className="text-base font-bold text-gray-900 mb-3 flex items-center gap-2">
            <Plus className="w-4 h-4 text-violet-600" />
            Schedule a Safety Check-in
          </h2>

          {!journey?.active ? (
            <p className="text-xs text-amber-700 bg-amber-50 p-3 rounded-xl border border-amber-200">
              Note: The SafeSignal safety policy requires an <strong>Active Journey</strong> to associate check-in locations and expected arrival times. Please start a journey first.
            </p>
          ) : (
            <form onSubmit={handleSchedule} className="grid grid-cols-1 md:grid-cols-3 gap-3">
              <div>
                <label className="block text-xs font-semibold text-gray-700 mb-1">Check-in Time</label>
                <input
                  type="text"
                  placeholder="HH:MM:SS (e.g. 18:15:00)"
                  value={schedTime}
                  onChange={(e) => setSchedTime(e.target.value)}
                  className="input-field"
                  required
                />
              </div>
              <div>
                <label className="block text-xs font-semibold text-gray-700 mb-1">Description / Note</label>
                <input
                  type="text"
                  placeholder="e.g. Reached Bus Stop"
                  value={description}
                  onChange={(e) => setDescription(e.target.value)}
                  className="input-field"
                />
              </div>
              <div>
                <label className="block text-xs font-semibold text-gray-700 mb-1">Expected Location</label>
                <input
                  type="text"
                  placeholder="e.g. Bus Stop A"
                  value={location}
                  onChange={(e) => setLocation(e.target.value)}
                  className="input-field"
                />
              </div>
              <div className="md:col-span-3">
                <button type="submit" className="btn-primary text-sm">
                  Schedule Check-in
                </button>
              </div>
            </form>
          )}
        </div>

        {/* Advance Clock Box */}
        <div className="card bg-violet-50/50 border-violet-200">
          <h2 className="text-base font-bold text-violet-950 mb-2 flex items-center gap-2">
            <FastForward className="w-4 h-4 text-violet-600" />
            Simulate Clock Advance
          </h2>
          <p className="text-xs text-gray-600 mb-3">
            Fast-forward time past a scheduled check-in (+5 min grace period) to test automated Missed Check-in alerts!
          </p>

          <form onSubmit={handleAdvanceTime} className="space-y-3">
            <input
              type="text"
              placeholder="e.g. 18:25:00"
              value={advanceTimeStr}
              onChange={(e) => setAdvanceTimeStr(e.target.value)}
              className="input-field"
              required
            />
            <button type="submit" className="btn-outline w-full text-xs">
              Advance Simulated Time
            </button>
          </form>
        </div>
      </div>

      {/* CHECK-INS TABLE */}
      <div className="card overflow-hidden p-0">
        <div className="p-4 border-b border-gray-100 flex items-center justify-between">
          <h3 className="font-bold text-gray-900 text-sm">Scheduled Check-in Records</h3>
          <span className="text-xs text-gray-500">Grace period: 5 minutes</span>
        </div>

        <div className="overflow-x-auto">
          <table className="w-full text-left text-xs">
            <thead className="bg-gray-50 text-gray-500 uppercase tracking-wider font-semibold border-b border-gray-100">
              <tr>
                <th className="px-4 py-3">ID</th>
                <th className="px-4 py-3">Scheduled Time</th>
                <th className="px-4 py-3">Location</th>
                <th className="px-4 py-3">Description</th>
                <th className="px-4 py-3">Status</th>
                <th className="px-4 py-3 text-right">Action</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-gray-100">
              {data.checkins?.length === 0 ? (
                <tr>
                  <td colSpan={6} className="px-4 py-8 text-center text-gray-400 italic">
                    No safety check-ins currently scheduled.
                  </td>
                </tr>
              ) : (
                data.checkins.map((ci) => (
                  <tr key={ci.id} className="hover:bg-gray-50/50">
                    <td className="px-4 py-3 font-mono font-bold text-gray-700">#{ci.id}</td>
                    <td className="px-4 py-3 font-semibold text-gray-900">{ci.scheduledTime}</td>
                    <td className="px-4 py-3 text-gray-600">{ci.location}</td>
                    <td className="px-4 py-3 text-gray-600">{ci.description}</td>
                    <td className="px-4 py-3">
                      <StatusBadge status={ci.status} />
                    </td>
                    <td className="px-4 py-3 text-right">
                      {!ci.isCompleted && (
                        <button
                          onClick={() => handleConfirm(ci.id)}
                          className="px-3 py-1 bg-emerald-600 hover:bg-emerald-700 text-white font-semibold rounded-lg text-xs shadow-sm flex items-center gap-1 ml-auto"
                        >
                          <Check className="w-3.5 h-3.5" />
                          Confirm Safe
                        </button>
                      )}
                    </td>
                  </tr>
                ))
              )}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}
