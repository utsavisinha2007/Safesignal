import React, { useState, useEffect } from 'react';
import {
  AlertTriangle,
  AlertOctagon,
  ShieldAlert,
  CheckCircle2,
  XCircle,
  PhoneCall,
  Clock,
  Radio,
  FileCheck,
  X,
  Flame,
  ShieldCheck,
  Zap,
  Users
} from 'lucide-react';
import { api } from '../services/api';
import StatusBadge from '../components/StatusBadge';

export default function EmergencyCenter({ status, onRefresh }) {
  const [incidents, setIncidents] = useState([]);
  const [loading, setLoading] = useState(true);
  const [feedback, setFeedback] = useState(null);

  // SOS Safety Modal
  const [showSosModal, setShowSosModal] = useState(false);
  const [sosLocation, setSosLocation] = useState('Campus South Quad');

  // Modals for actions
  const [ackModal, setAckModal] = useState(null);
  const [contactIdInput, setContactIdInput] = useState('101');
  const [resolveModal, setResolveModal] = useState(null);
  const [resolveNotes, setResolveNotes] = useState('User verified safe on site by campus security');
  const [cancelModal, setCancelModal] = useState(null);
  const [cancelReason, setCancelReason] = useState('Accidental button trigger (false alarm)');

  const fetchIncidents = async () => {
    try {
      setLoading(true);
      const res = await api.getActiveIncidents();
      setIncidents(res.incidents || []);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchIncidents();
  }, []);

  const handleConfirmSafe = async () => {
    try {
      const res = await api.confirmSafe();
      setFeedback({ type: 'success', msg: res.message });
      fetchIncidents();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleRequestHelp = async () => {
    try {
      const res = await api.requestHelp();
      setFeedback({ type: 'error', msg: res.message });
      fetchIncidents();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleTriggerSOS = async () => {
    setShowSosModal(false);
    try {
      await api.triggerSOS({
        location: sosLocation,
        timestamp: new Date().toTimeString().split(' ')[0]
      });
      setFeedback({
        type: 'error',
        msg: 'CRITICAL ALERT: Emergency SOS dispatched! Tier 1 priority contacts alerted in C++ ResponseManager.'
      });
      fetchIncidents();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleAcknowledge = async (e) => {
    e.preventDefault();
    if (!ackModal) return;
    try {
      const res = await api.acknowledgeIncident({
        incidentID: ackModal.incidentID,
        contactID: parseInt(contactIdInput, 10),
        timestamp: new Date().toTimeString().split(' ')[0]
      });
      setFeedback({ type: 'success', msg: res.message });
      setAckModal(null);
      fetchIncidents();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleResolve = async (e) => {
    e.preventDefault();
    if (!resolveModal) return;
    try {
      const res = await api.resolveIncident({
        incidentID: resolveModal.incidentID,
        notes: resolveNotes,
        timestamp: new Date().toTimeString().split(' ')[0]
      });
      setFeedback({ type: 'success', msg: res.message });
      setResolveModal(null);
      fetchIncidents();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleCancel = async (e) => {
    e.preventDefault();
    if (!cancelModal) return;
    try {
      const res = await api.cancelIncident({
        incidentID: cancelModal.incidentID,
        reason: cancelReason,
        timestamp: new Date().toTimeString().split(' ')[0]
      });
      setFeedback({ type: 'success', msg: res.message });
      setCancelModal(null);
      fetchIncidents();
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  return (
    <div className="space-y-6">
      {/* Page Title */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-2 border-b border-[rgba(148,163,184,0.1)]">
        <div>
          <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-wider text-rose-400 mb-1">
            <Flame className="w-3.5 h-3.5 animate-pulse" />
            <span>Priority Queue Max-Heap Emergency Triage</span>
          </div>
          <h1 className="page-title text-white">Emergency Response Centre</h1>
          <p className="page-subtitle">
            Immediate panic SOS broadcasting, tiered contact escalation deadlines, and full incident lifecycle tracking.
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
              <AlertOctagon className="w-4 h-4 text-rose-400 shrink-0" />
            )}
            <span>{feedback.msg}</span>
          </div>
          <button onClick={() => setFeedback(null)} className="text-[11px] underline text-slate-300 hover:text-white">
            Dismiss
          </button>
        </div>
      )}

      {/* TACTICAL ONE-TOUCH SOS ACTION BOX */}
      <div className="cmd-card bg-gradient-to-br from-rose-950/70 via-[#111827] to-[#0D1220] border-2 border-rose-500/50 p-8 text-center rounded-3xl shadow-danger-glow relative overflow-hidden">
        <div className="max-w-xl mx-auto space-y-3 mb-6">
          <span className="cmd-badge bg-rose-500/20 text-rose-300 border-rose-500/40 font-mono text-[11px]">
            TACTICAL DISTRESS BROADCAST
          </span>
          <h2 className="text-2xl font-black text-white tracking-tight uppercase">
            One-Touch Emergency SOS
          </h2>
          <p className="text-xs text-slate-300 leading-relaxed">
            Immediately triggers the C++ Priority-Based Emergency Dispatcher. Broadcasts urgent distress telemetry to Tier 1 primary contacts and activates campus security protocol.
          </p>
        </div>

        {/* Big Radar SOS Trigger Button */}
        <div className="relative inline-block my-2">
          <div className="absolute inset-0 rounded-full bg-rose-600/30 blur-xl animate-pulse" />
          <button
            onClick={() => setShowSosModal(true)}
            className="relative w-44 h-44 rounded-full bg-gradient-to-tr from-rose-700 to-rose-500 hover:from-rose-600 hover:to-rose-400 active:scale-95 text-white font-black text-2xl shadow-2xl transition-all duration-200 flex flex-col items-center justify-center gap-2 border-4 border-rose-300/40 group"
          >
            <Radio className="w-10 h-10 text-white animate-ping group-hover:scale-110 transition-transform" />
            <span className="tracking-widest">SOS</span>
            <span className="text-[10px] font-mono tracking-normal text-rose-100 font-semibold uppercase">
              Emergency Trigger
            </span>
          </button>
        </div>

        <p className="text-[11px] text-slate-400 mt-4 font-mono">
          * Simulated emergency dispatch for campus testing. Does not contact real municipal 911.
        </p>
      </div>

      {/* PENDING VERIFICATION ACTION (IF ACTIVE) */}
      {status?.hasActiveVerification && (
        <div className="cmd-card border-2 border-rose-500 bg-rose-950/20">
          <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
            <div>
              <div className="inline-flex items-center gap-2 px-2.5 py-0.5 rounded-full text-xs font-bold bg-rose-500/20 text-rose-200 border border-rose-500/40 mb-1.5">
                <AlertOctagon className="w-3.5 h-3.5 text-rose-400" />
                <span>ACTIVE VERIFICATION #{status.activeVerificationID} AWAITING USER INPUT</span>
              </div>
              <h3 className="text-base font-bold text-white">
                Abnormal Risk Alert Active — 3-Minute Deadline
              </h3>
              <p className="text-xs text-slate-300 mt-0.5">
                Risk threshold was crossed. Confirm safe right now to cancel automatic emergency dispatch.
              </p>
            </div>

            <div className="flex items-center gap-3 shrink-0">
              <button onClick={handleConfirmSafe} className="btn-safe text-xs px-4 py-2.5">
                <CheckCircle2 className="w-4 h-4" />
                <span>I AM SAFE (CANCEL)</span>
              </button>
              <button onClick={handleRequestHelp} className="btn-danger text-xs px-4 py-2.5">
                <AlertTriangle className="w-4 h-4" />
                <span>DISPATCH RESCUE NOW</span>
              </button>
            </div>
          </div>
        </div>
      )}

      {/* ACTIVE EMERGENCY INCIDENTS LIST */}
      <div className="space-y-4">
        <div className="flex items-center justify-between">
          <h2 className="text-base font-bold text-white uppercase tracking-wider flex items-center gap-2">
            <ShieldAlert className="w-4 h-4 text-rose-400" />
            <span>Active Incidents Underway ({incidents.length})</span>
          </h2>
          <button onClick={fetchIncidents} className="text-xs font-semibold text-violet-400 hover:text-violet-300">
            Refresh Incidents
          </button>
        </div>

        {incidents.length === 0 ? (
          <div className="cmd-card text-center py-12 text-slate-500 space-y-2">
            <CheckCircle2 className="w-10 h-10 text-emerald-400/60 mx-auto" />
            <p className="text-sm font-bold text-slate-300">No Active Emergency Incidents</p>
            <p className="text-xs text-slate-500">
              The C++ Priority Queue triage heap is idle. All safety conditions are nominal.
            </p>
          </div>
        ) : (
          incidents.map((inc) => (
            <div key={inc.id} className="cmd-card border-l-4 border-l-rose-500 space-y-4">
              <div className="flex flex-col md:flex-row md:items-start justify-between gap-3">
                <div className="space-y-1">
                  <div className="flex items-center gap-2.5 flex-wrap">
                    <span className="font-mono font-black text-white text-base">INCIDENT #{inc.id}</span>
                    <span className="px-2 py-0.5 rounded text-[11px] font-bold bg-rose-500/20 text-rose-300 border border-rose-500/40">
                      SEVERITY {inc.severity}/5
                    </span>
                    <StatusBadge status={inc.status} />
                  </div>
                  <p className="text-xs font-semibold text-slate-200">{inc.description}</p>
                  <p className="text-[11px] text-slate-400 font-mono">
                    Location: <strong className="text-slate-200">{inc.location}</strong> • Dispatched: {inc.creationTimestamp}
                  </p>
                </div>

                <div className="text-left md:text-right">
                  <span className="text-xs font-bold text-violet-300 block font-mono">
                    ESCALATION TIER: {inc.currentTier > 0 ? `TIER ${inc.currentTier}` : 'UNNOTIFIED'}
                  </span>
                  {inc.tierDeadlineTimestamp && (
                    <span className="text-[11px] text-amber-300 flex items-center justify-start md:justify-end gap-1 mt-1 font-mono">
                      <Clock className="w-3 h-3 text-amber-400" />
                      Next Tier Escalation: {inc.tierDeadlineTimestamp}
                    </span>
                  )}
                </div>
              </div>

              {/* Notification Broadcast Audit Trail */}
              {inc.notifications?.length > 0 && (
                <div className="p-3 bg-[#172033] rounded-xl border border-[rgba(148,163,184,0.12)] space-y-2">
                  <span className="text-[10px] font-bold uppercase tracking-wider text-slate-400 block">
                    Dispatched Contact Notifications (Live Audit Trail)
                  </span>
                  <div className="grid grid-cols-1 sm:grid-cols-2 gap-2 text-xs">
                    {inc.notifications.map((n, idx) => (
                      <div key={idx} className="p-2 rounded-lg bg-[#0D1220] border border-[rgba(148,163,184,0.1)] flex justify-between">
                        <div>
                          <strong className="text-white">{n.contactName}</strong>
                          <span className="text-slate-400 block text-[11px] font-mono">Tier {n.priorityLevel} ({n.contactPhone})</span>
                        </div>
                        <div className="text-right font-mono">
                          <span className="text-slate-400 block text-[10px]">{n.timestamp}</span>
                          <span className={`text-[10px] font-bold ${n.acknowledged ? 'text-emerald-400' : 'text-amber-400'}`}>
                            {n.acknowledged ? '✓ ACKNOWLEDGED' : 'AWAITING ACK'}
                          </span>
                        </div>
                      </div>
                    ))}
                  </div>
                </div>
              )}

              {/* Action Buttons */}
              <div className="flex flex-wrap items-center gap-2 pt-2 border-t border-[rgba(148,163,184,0.1)]">
                <button
                  onClick={() => setAckModal({ incidentID: inc.id })}
                  className="px-3.5 py-2 bg-amber-500/20 hover:bg-amber-500/30 text-amber-200 border border-amber-500/40 rounded-xl text-xs font-bold flex items-center gap-1.5 transition-all"
                >
                  <FileCheck className="w-3.5 h-3.5 text-amber-400" />
                  <span>Acknowledge (Contact)</span>
                </button>

                <button
                  onClick={() => setResolveModal({ incidentID: inc.id })}
                  className="px-3.5 py-2 bg-emerald-500/20 hover:bg-emerald-500/30 text-emerald-200 border border-emerald-500/40 rounded-xl text-xs font-bold flex items-center gap-1.5 transition-all"
                >
                  <CheckCircle2 className="w-3.5 h-3.5 text-emerald-400" />
                  <span>Resolve Incident</span>
                </button>

                <button
                  onClick={() => setCancelModal({ incidentID: inc.id })}
                  className="px-3.5 py-2 bg-slate-800 hover:bg-slate-700 text-slate-300 border border-[rgba(148,163,184,0.2)] rounded-xl text-xs font-bold flex items-center gap-1.5 transition-all"
                >
                  <XCircle className="w-3.5 h-3.5 text-slate-400" />
                  <span>Cancel False Alarm</span>
                </button>
              </div>
            </div>
          ))
        )}
      </div>

      {/* SOS CONFIRMATION MODAL */}
      {showSosModal && (
        <div className="fixed inset-0 bg-black/80 backdrop-blur-md flex items-center justify-center p-4 z-50 animate-in fade-in duration-150">
          <div className="cmd-card max-w-md w-full border-2 border-rose-500 shadow-danger-glow space-y-4">
            <div className="flex items-start justify-between">
              <div className="flex items-center gap-3">
                <div className="p-2.5 rounded-xl bg-rose-500/20 text-rose-400 border border-rose-500/30">
                  <AlertOctagon className="w-6 h-6" />
                </div>
                <div>
                  <h3 className="text-base font-bold text-white uppercase tracking-tight">Confirm Emergency SOS</h3>
                  <span className="text-xs text-rose-400 font-mono font-semibold">Immediate Priority Dispatch</span>
                </div>
              </div>
              <button onClick={() => setShowSosModal(false)} className="text-slate-400 hover:text-white">
                <X className="w-5 h-5" />
              </button>
            </div>

            <p className="text-xs text-slate-300 leading-relaxed">
              Are you sure you want to trigger this distress broadcast? This initiates priority-based emergency dispatch, automatically notifying registered Tier 1 contacts and activating the campus security incident queue.
            </p>

            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1">Current User Location</label>
              <input
                type="text"
                value={sosLocation}
                onChange={(e) => setSosLocation(e.target.value)}
                className="input-field"
                required
              />
            </div>

            <div className="flex gap-3 pt-2">
              <button onClick={() => setShowSosModal(false)} className="btn-outline flex-1 text-xs">
                Cancel
              </button>
              <button onClick={handleTriggerSOS} className="btn-danger flex-1 text-xs">
                YES — DISPATCH SOS
              </button>
            </div>
          </div>
        </div>
      )}

      {/* ACKNOWLEDGE MODAL */}
      {ackModal && (
        <div className="fixed inset-0 bg-black/80 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <form onSubmit={handleAcknowledge} className="cmd-card max-w-sm w-full space-y-4">
            <h3 className="text-sm font-bold text-white uppercase">Acknowledge Incident #{ackModal.incidentID}</h3>
            <p className="text-xs text-slate-400">Simulate an emergency contact responding and acknowledging the alert.</p>

            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1">Responding Contact ID</label>
              <input
                type="number"
                value={contactIdInput}
                onChange={(e) => setContactIdInput(e.target.value)}
                className="input-field font-mono"
                required
              />
            </div>

            <div className="flex gap-2 pt-2">
              <button type="button" onClick={() => setAckModal(null)} className="btn-outline flex-1 text-xs">
                Cancel
              </button>
              <button type="submit" className="btn-primary flex-1 text-xs">
                Confirm
              </button>
            </div>
          </form>
        </div>
      )}

      {/* RESOLVE MODAL */}
      {resolveModal && (
        <div className="fixed inset-0 bg-black/80 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <form onSubmit={handleResolve} className="cmd-card max-w-md w-full space-y-4">
            <h3 className="text-sm font-bold text-white uppercase">Resolve Incident #{resolveModal.incidentID}</h3>
            <p className="text-xs text-slate-400">Document responder debrief notes and safely close this incident record.</p>

            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1">Resolution Notes</label>
              <textarea
                value={resolveNotes}
                onChange={(e) => setResolveNotes(e.target.value)}
                className="input-field h-24"
                required
              />
            </div>

            <div className="flex gap-2 pt-2">
              <button type="button" onClick={() => setResolveModal(null)} className="btn-outline flex-1 text-xs">
                Cancel
              </button>
              <button type="submit" className="btn-safe flex-1 text-xs">
                Close Incident
              </button>
            </div>
          </form>
        </div>
      )}

      {/* CANCEL MODAL */}
      {cancelModal && (
        <div className="fixed inset-0 bg-black/80 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <form onSubmit={handleCancel} className="cmd-card max-w-md w-full space-y-4">
            <h3 className="text-sm font-bold text-white uppercase">Cancel Incident #{cancelModal.incidentID}</h3>
            <p className="text-xs text-slate-400">Record a cancellation reason and archive as false alarm.</p>

            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1">Cancellation Reason</label>
              <input
                type="text"
                value={cancelReason}
                onChange={(e) => setCancelReason(e.target.value)}
                className="input-field"
                required
              />
            </div>

            <div className="flex gap-2 pt-2">
              <button type="button" onClick={() => setCancelModal(null)} className="btn-outline flex-1 text-xs">
                Back
              </button>
              <button type="submit" className="btn-danger flex-1 text-xs">
                Confirm Cancel
              </button>
            </div>
          </form>
        </div>
      )}
    </div>
  );
}
