import React, { useState, useEffect } from 'react';
import {
  Clock,
  CheckCircle2,
  AlertTriangle,
  Plus,
  FastForward,
  Check,
  Calendar,
  Hourglass,
  ArrowRight
} from 'lucide-react';
import { api } from '../services/api';
import StatusBadge from '../components/StatusBadge';

export default function SafetyCheckins({ journey, onRefresh }) {
  const [data, setData] = useState({ checkins: [], stats: {} });
  const [loading, setLoading] = useState(true);
  const [feedback, setFeedback] = useState(null);

  // New check-in form
  const [schedTime, setSchedTime] = useState('18:30:00');
  const [description, setDescription] = useState('Passing Student Union Quad');
  const [location, setLocation] = useState('Student Union Quad');

  // Sim time control
  const [advanceTimeStr, setAdvanceTimeStr] = useState('18:36:00');

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
        setFeedback({
          type: 'success',
          msg: `Safety Check-in #${res.checkInID} scheduled for ${schedTime} with 5-min grace period!`
        });
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
        setFeedback({
          type: 'success',
          msg: `Check-in #${id} confirmed! C++ engine marked status as confirmed safe.`
        });
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
        msg: `Simulated clock advanced to ${advanceTimeStr}. C++ CheckInManager evaluated deadlines.`
      });
      fetchCheckins();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-2 border-b border-[rgba(148,163,184,0.1)]">
        <div>
          <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-wider text-teal-400 mb-1">
            <Clock className="w-3.5 h-3.5" />
            <span>5-Minute Grace Period Protocol</span>
          </div>
          <h1 className="page-title">Safety Check-ins Command</h1>
          <p className="page-subtitle">
            Timed safety check-ins. If unconfirmed past scheduled deadline + 5 minutes grace, the C++ engine generates automated Missed Check-in alerts.
          </p>
        </div>
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

      {/* METRIC TILES */}
      <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
        <div className="cmd-card p-4 text-center">
          <span className="text-[10px] font-bold uppercase tracking-wider text-slate-400">Total Registered</span>
          <p className="text-2xl font-black font-mono text-white mt-1">{data.stats?.total ?? 0}</p>
        </div>

        <div className="cmd-card p-4 text-center">
          <span className="text-[10px] font-bold uppercase tracking-wider text-amber-400">Pending</span>
          <p className="text-2xl font-black font-mono text-amber-300 mt-1">{data.stats?.pending ?? 0}</p>
        </div>

        <div className="cmd-card p-4 text-center">
          <span className="text-[10px] font-bold uppercase tracking-wider text-emerald-400">On-Time</span>
          <p className="text-2xl font-black font-mono text-emerald-300 mt-1">{data.stats?.completed ?? 0}</p>
        </div>

        <div className="cmd-card p-4 text-center">
          <span className="text-[10px] font-bold uppercase tracking-wider text-purple-400">Late Confirm</span>
          <p className="text-2xl font-black font-mono text-purple-300 mt-1">{data.stats?.late ?? 0}</p>
        </div>

        <div className="cmd-card p-4 text-center">
          <span className="text-[10px] font-bold uppercase tracking-wider text-rose-400">Missed (Alert)</span>
          <p className="text-2xl font-black font-mono text-rose-400 mt-1">{data.stats?.missed ?? 0}</p>
        </div>
      </div>

      {/* SCHEDULE FORM & TIME ADVANCE BOX */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Schedule Form */}
        <div className="cmd-card lg:col-span-2 space-y-4">
          <div className="flex items-center gap-2 border-b border-[rgba(148,163,184,0.12)] pb-3">
            <Plus className="w-4 h-4 text-teal-400" />
            <h2 className="text-sm font-bold text-white uppercase tracking-wider">
              Schedule New Check-in
            </h2>
          </div>

          {!journey?.active ? (
            <div className="p-4 rounded-xl bg-amber-500/10 border border-amber-500/30 text-xs text-amber-300">
              <strong className="block font-bold mb-1">C++ SAFETY SYSTEM REQUIREMENT:</strong>
              Check-ins can only be scheduled during an <strong>Active Journey</strong> to associate expected waypoint coordinates and arrival deadlines. Please start a journey in the Journey Monitoring tab first.
            </div>
          ) : (
            <form onSubmit={handleSchedule} className="grid grid-cols-1 md:grid-cols-3 gap-3">
              <div>
                <label className="block text-xs font-semibold text-slate-300 mb-1.5">Check-in Time</label>
                <input
                  type="text"
                  placeholder="HH:MM:SS (e.g. 18:30:00)"
                  value={schedTime}
                  onChange={(e) => setSchedTime(e.target.value)}
                  className="input-field font-mono"
                  required
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-300 mb-1.5">Description / Purpose</label>
                <input
                  type="text"
                  placeholder="e.g. Reached South Bridge"
                  value={description}
                  onChange={(e) => setDescription(e.target.value)}
                  className="input-field"
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-300 mb-1.5">Expected Location</label>
                <input
                  type="text"
                  placeholder="e.g. South Bridge"
                  value={location}
                  onChange={(e) => setLocation(e.target.value)}
                  className="input-field"
                />
              </div>

              <div className="md:col-span-3 pt-2">
                <button type="submit" className="btn-primary text-xs">
                  <Plus className="w-4 h-4" />
                  <span>Register Scheduled Check-in</span>
                </button>
              </div>
            </form>
          )}
        </div>

        {/* Clock Advance Simulation */}
        <div className="cmd-card space-y-4">
          <div className="flex items-center gap-2 border-b border-[rgba(148,163,184,0.12)] pb-3">
            <FastForward className="w-4 h-4 text-violet-400" />
            <h2 className="text-sm font-bold text-white uppercase tracking-wider">
              Clock Simulator
            </h2>
          </div>
          <p className="text-xs text-slate-400">
            Fast-forward time past a scheduled check-in (+5 min grace period) to test automated Missed Check-in alerts!
          </p>

          <form onSubmit={handleAdvanceTime} className="space-y-3">
            <input
              type="text"
              placeholder="HH:MM:SS"
              value={advanceTimeStr}
              onChange={(e) => setAdvanceTimeStr(e.target.value)}
              className="input-field font-mono"
              required
            />
            <button type="submit" className="btn-outline w-full text-xs">
              <FastForward className="w-3.5 h-3.5" />
              <span>Step Clock Forward</span>
            </button>
          </form>

          <div className="p-3 bg-[#172033] rounded-xl border border-[rgba(148,163,184,0.12)] text-[11px] text-slate-400 space-y-1">
            <span className="font-semibold text-slate-300 block">Check-in Lifecycle States:</span>
            <p>1. Scheduled (Pending)</p>
            <p>2. Confirmed On-Time (Safe)</p>
            <p>3. Grace Period Active (+5 min)</p>
            <p>4. Missed Check-in Event Fired</p>
          </div>
        </div>
      </div>

      {/* CHECK-INS RECORDS DATA TABLE */}
      <div className="cmd-card p-0 overflow-hidden">
        <div className="p-4 border-b border-[rgba(148,163,184,0.12)] flex items-center justify-between">
          <div className="flex items-center gap-2">
            <Clock className="w-4 h-4 text-teal-400" />
            <h3 className="text-sm font-bold text-white uppercase tracking-wider">
              Scheduled Check-in Registry
            </h3>
          </div>
          <span className="text-xs font-mono text-slate-400">
            Enforced Grace Period: {data.stats?.gracePeriodMinutes || 5} min
          </span>
        </div>

        <div className="overflow-x-auto">
          <table className="w-full text-left text-xs">
            <thead className="bg-[#172033]/60 text-slate-400 uppercase tracking-wider font-semibold border-b border-[rgba(148,163,184,0.1)]">
              <tr>
                <th className="px-4 py-3">ID</th>
                <th className="px-4 py-3">Scheduled Time</th>
                <th className="px-4 py-3">Location</th>
                <th className="px-4 py-3">Description</th>
                <th className="px-4 py-3">Status</th>
                <th className="px-4 py-3 text-right">Action</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-[rgba(148,163,184,0.08)]">
              {data.checkins?.length === 0 ? (
                <tr>
                  <td colSpan={6} className="px-4 py-8 text-center text-slate-500 italic">
                    No safety check-ins currently registered in C++ CheckInManager.
                  </td>
                </tr>
              ) : (
                data.checkins.map((ci) => (
                  <tr key={ci.id} className="hover:bg-[#172033]/40 transition-colors">
                    <td className="px-4 py-3 font-mono font-bold text-violet-300">#{ci.id}</td>
                    <td className="px-4 py-3 font-mono font-semibold text-white">{ci.scheduledTime}</td>
                    <td className="px-4 py-3 text-slate-300">{ci.location}</td>
                    <td className="px-4 py-3 text-slate-400">{ci.description}</td>
                    <td className="px-4 py-3">
                      <StatusBadge status={ci.status} />
                    </td>
                    <td className="px-4 py-3 text-right">
                      {!ci.isCompleted && (
                        <button
                          onClick={() => handleConfirm(ci.id)}
                          className="px-3 py-1 bg-emerald-600 hover:bg-emerald-500 text-white font-bold rounded-lg text-xs shadow-sm flex items-center gap-1.5 ml-auto"
                        >
                          <Check className="w-3.5 h-3.5" />
                          <span>I'm Safe</span>
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
