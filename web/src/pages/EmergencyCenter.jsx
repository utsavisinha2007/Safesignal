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
  X
} from 'lucide-react';
import { api } from '../services/api';
import StatusBadge from '../components/StatusBadge';

export default function EmergencyCenter({ status, onRefresh }) {
  const [incidents, setIncidents] = useState([]);
  const [loading, setLoading] = useState(true);
  const [feedback, setFeedback] = useState(null);

  // SOS Modal
  const [showSosModal, setShowSosModal] = useState(false);
  const [sosLocation, setSosLocation] = useState('Campus South Quad');

  // Action modals
  const [ackModal, setAckModal] = useState(null); // { incidentID }
  const [contactIdInput, setContactIdInput] = useState('101');
  const [resolveModal, setResolveModal] = useState(null); // { incidentID }
  const [resolveNotes, setResolveNotes] = useState('User verified safe on site by responder');
  const [cancelModal, setCancelModal] = useState(null); // { incidentID }
  const [cancelReason, setCancelReason] = useState('Accidental button press');

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
      const res = await api.triggerSOS({
        location: sosLocation,
        timestamp: new Date().toTimeString().split(' ')[0]
      });
      setFeedback({
        type: 'error',
        msg: 'EMERGENCY SOS DISPATCHED: Alert broadcasted to Priority 1 contacts! (Simulated)'
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
      <div>
        <h1 className="page-title text-red-700 flex items-center gap-2">
          <AlertTriangle className="w-7 h-7 text-red-600" />
          Emergency Response Center
        </h1>
        <p className="page-subtitle">
          Immediate distress dispatch, priority escalation tracking, and incident lifecycle management.
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
              <AlertOctagon className="w-5 h-5 text-red-600 shrink-0" />
            )}
            <span>{feedback.msg}</span>
          </div>
          <button onClick={() => setFeedback(null)} className="text-xs font-semibold hover:underline">
            Dismiss
          </button>
        </div>
      )}

      {/* ONE-TOUCH SOS SECTION */}
      <div className="card bg-gradient-to-br from-red-600 to-rose-800 text-white p-8 text-center rounded-3xl shadow-xl border-4 border-red-400/40">
        <h2 className="text-2xl font-black tracking-tight mb-2 uppercase">One-Touch Emergency SOS</h2>
        <p className="text-sm text-red-100 max-w-lg mx-auto mb-6">
          Pressing SOS initiates immediate priority-based emergency dispatch, automatically notifying your Tier 1 contacts and campus security.
        </p>

        <button
          onClick={() => setShowSosModal(true)}
          className="w-40 h-40 mx-auto rounded-full bg-white text-red-600 font-black text-2xl shadow-2xl hover:scale-105 active:scale-95 transition-all flex flex-col items-center justify-center gap-2 border-8 border-red-200/50 hover:bg-red-50"
        >
          <Radio className="w-10 h-10 text-red-600 animate-ping" />
          <span>SOS</span>
        </button>

        <p className="text-[11px] text-red-200 mt-4 italic">
          * Simulated emergency dispatch for campus safety demonstrations.
        </p>
      </div>

      {/* PENDING VERIFICATION ACTION (IF ACTIVE) */}
      {status?.hasActiveVerification && (
        <div className="card border-2 border-red-300 bg-red-50/50">
          <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
            <div>
              <span className="badge bg-red-600 text-white mb-2">VERIFICATION PENDING</span>
              <h3 className="text-lg font-bold text-gray-900">
                Safety Verification #{status.activeVerificationID} Awaiting User Confirmation
              </h3>
              <p className="text-xs text-gray-600 mt-1">
                Risk score threshold exceeded. Confirm you are safe to avert automatic emergency dispatch.
              </p>
            </div>
            <div className="flex items-center gap-3">
              <button onClick={handleConfirmSafe} className="btn-safe text-sm flex items-center gap-1.5">
                <CheckCircle2 className="w-4 h-4" />
                I AM SAFE
              </button>
              <button onClick={handleRequestHelp} className="btn-danger text-sm flex items-center gap-1.5">
                <AlertTriangle className="w-4 h-4" />
                I NEED HELP NOW
              </button>
            </div>
          </div>
        </div>
      )}

      {/* ACTIVE EMERGENCY INCIDENTS */}
      <div className="space-y-4">
        <div className="flex items-center justify-between">
          <h2 className="text-lg font-bold text-gray-900 flex items-center gap-2">
            <ShieldAlert className="w-5 h-5 text-red-600" />
            Active Emergencies & Incidents ({incidents.length})
          </h2>
          <button onClick={fetchIncidents} className="text-xs font-semibold text-violet-600 hover:underline">
            Refresh Incidents
          </button>
        </div>

        {incidents.length === 0 ? (
          <div className="card text-center py-12 text-gray-400">
            <CheckCircle2 className="w-12 h-12 text-emerald-400 mx-auto mb-2 opacity-60" />
            <p className="font-semibold text-gray-600">No active emergency incidents</p>
            <p className="text-xs text-gray-400 mt-1">All clear. The SafeSignal response queue is currently idle.</p>
          </div>
        ) : (
          incidents.map((inc) => (
            <div key={inc.id} className="card border-l-4 border-l-red-600 space-y-4">
              <div className="flex flex-col md:flex-row md:items-center justify-between gap-2">
                <div>
                  <div className="flex items-center gap-2">
                    <span className="font-mono font-bold text-gray-900 text-base">Incident #{inc.id}</span>
                    <span className="badge bg-red-100 text-red-800">Severity {inc.severity}/5</span>
                    <StatusBadge status={inc.status} />
                  </div>
                  <p className="text-sm text-gray-700 mt-1">{inc.description}</p>
                  <p className="text-xs text-gray-500 mt-0.5">
                    Location: <strong>{inc.location}</strong> • Created at: {inc.creationTimestamp}
                  </p>
                </div>

                <div className="text-right">
                  <span className="text-xs font-bold text-violet-700 block">
                    Current Dispatch Tier: {inc.currentTier > 0 ? `Tier ${inc.currentTier}` : 'Unnotified'}
                  </span>
                  {inc.tierDeadlineTimestamp && (
                    <span className="text-xs text-gray-500 flex items-center justify-end gap-1 mt-0.5">
                      <Clock className="w-3.5 h-3.5 text-gray-400" />
                      Next Escalation: {inc.tierDeadlineTimestamp}
                    </span>
                  )}
                </div>
              </div>

              {/* Responder Notifications Trail */}
              {inc.notifications?.length > 0 && (
                <div className="p-3 bg-gray-50 rounded-xl border border-gray-100">
                  <span className="text-xs font-bold text-gray-500 uppercase tracking-wider block mb-2">
                    Dispatched Contact Notifications (Audit Trail)
                  </span>
                  <div className="grid grid-cols-1 md:grid-cols-2 gap-2 text-xs">
                    {inc.notifications.map((n, idx) => (
                      <div key={idx} className="p-2 bg-white rounded-lg border border-gray-200 flex justify-between">
                        <div>
                          <strong>{n.contactName}</strong> ({n.contactPhone})
                          <span className="text-gray-400 block text-[11px]">Priority Tier {n.priorityLevel}</span>
                        </div>
                        <div className="text-right">
                          <span className="text-gray-500 block">{n.timestamp}</span>
                          <span className={`text-[10px] font-bold ${n.acknowledged ? 'text-emerald-600' : 'text-amber-600'}`}>
                            {n.acknowledged ? '✓ Acknowledged' : 'Awaiting Ack'}
                          </span>
                        </div>
                      </div>
                    ))}
                  </div>
                </div>
              )}

              {/* Action Buttons */}
              <div className="flex flex-wrap items-center gap-3 pt-2 border-t border-gray-100">
                <button
                  onClick={() => setAckModal({ incidentID: inc.id })}
                  className="px-4 py-2 bg-amber-500 hover:bg-amber-600 text-white font-semibold rounded-xl text-xs flex items-center gap-1.5 shadow-sm"
                >
                  <FileCheck className="w-4 h-4" />
                  Acknowledge (Contact)
                </button>
                <button
                  onClick={() => setResolveModal({ incidentID: inc.id })}
                  className="px-4 py-2 bg-emerald-600 hover:bg-emerald-700 text-white font-semibold rounded-xl text-xs flex items-center gap-1.5 shadow-sm"
                >
                  <CheckCircle2 className="w-4 h-4" />
                  Resolve Incident
                </button>
                <button
                  onClick={() => setCancelModal({ incidentID: inc.id })}
                  className="px-4 py-2 bg-gray-600 hover:bg-gray-700 text-white font-semibold rounded-xl text-xs flex items-center gap-1.5 shadow-sm"
                >
                  <XCircle className="w-4 h-4" />
                  Cancel False Alarm
                </button>
              </div>
            </div>
          ))
        )}
      </div>

      {/* SOS CONFIRMATION MODAL */}
      {showSosModal && (
        <div className="fixed inset-0 bg-black/60 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <div className="bg-white rounded-2xl max-w-md w-full p-6 shadow-2xl border-4 border-red-500 animate-in fade-in zoom-in duration-200">
            <div className="flex items-start justify-between mb-4">
              <div className="flex items-center gap-3">
                <div className="p-3 bg-red-100 text-red-600 rounded-xl">
                  <AlertOctagon className="w-7 h-7" />
                </div>
                <div>
                  <h3 className="text-lg font-bold text-gray-900">Trigger Emergency SOS?</h3>
                  <p className="text-xs text-red-600 font-semibold">Immediate Priority Dispatch</p>
                </div>
              </div>
              <button onClick={() => setShowSosModal(false)} className="text-gray-400 hover:text-gray-600">
                <X className="w-5 h-5" />
              </button>
            </div>

            <p className="text-xs text-gray-600 mb-4">
              Are you sure you want to trigger an emergency alert? This will immediately escalate distress signals to your registered priority contacts and campus emergency responders.
            </p>

            <div className="mb-4">
              <label className="block text-xs font-semibold text-gray-700 mb-1">Your Current Location</label>
              <input
                type="text"
                value={sosLocation}
                onChange={(e) => setSosLocation(e.target.value)}
                className="input-field"
                required
              />
            </div>

            <div className="flex items-center gap-3">
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
        <div className="fixed inset-0 bg-black/50 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <form onSubmit={handleAcknowledge} className="bg-white rounded-2xl max-w-sm w-full p-6 shadow-2xl">
            <h3 className="text-base font-bold text-gray-900 mb-2">Acknowledge Incident #{ackModal.incidentID}</h3>
            <p className="text-xs text-gray-500 mb-4">Simulate an emergency contact responding to the alert.</p>

            <div className="mb-4">
              <label className="block text-xs font-semibold text-gray-700 mb-1">Contact ID</label>
              <input
                type="number"
                value={contactIdInput}
                onChange={(e) => setContactIdInput(e.target.value)}
                className="input-field"
                required
              />
            </div>

            <div className="flex gap-2">
              <button type="button" onClick={() => setAckModal(null)} className="btn-outline flex-1 text-xs">
                Cancel
              </button>
              <button type="submit" className="btn-primary flex-1 text-xs">
                Confirm Acknowledgement
              </button>
            </div>
          </form>
        </div>
      )}

      {/* RESOLVE MODAL */}
      {resolveModal && (
        <div className="fixed inset-0 bg-black/50 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <form onSubmit={handleResolve} className="bg-white rounded-2xl max-w-md w-full p-6 shadow-2xl">
            <h3 className="text-base font-bold text-gray-900 mb-2">Resolve Incident #{resolveModal.incidentID}</h3>
            <p className="text-xs text-gray-500 mb-4">Record resolution notes and safely close this emergency incident.</p>

            <div className="mb-4">
              <label className="block text-xs font-semibold text-gray-700 mb-1">Resolution Notes</label>
              <textarea
                value={resolveNotes}
                onChange={(e) => setResolveNotes(e.target.value)}
                className="input-field h-24"
                required
              />
            </div>

            <div className="flex gap-2">
              <button type="button" onClick={() => setResolveModal(null)} className="btn-outline flex-1 text-xs">
                Cancel
              </button>
              <button type="submit" className="btn-safe flex-1 text-xs">
                Mark as Resolved
              </button>
            </div>
          </form>
        </div>
      )}

      {/* CANCEL MODAL */}
      {cancelModal && (
        <div className="fixed inset-0 bg-black/50 backdrop-blur-sm flex items-center justify-center p-4 z-50">
          <form onSubmit={handleCancel} className="bg-white rounded-2xl max-w-md w-full p-6 shadow-2xl">
            <h3 className="text-base font-bold text-gray-900 mb-2">Cancel Incident #{cancelModal.incidentID}</h3>
            <p className="text-xs text-gray-500 mb-4">Cancel incident as a false alarm with a documented reason.</p>

            <div className="mb-4">
              <label className="block text-xs font-semibold text-gray-700 mb-1">Cancellation Reason</label>
              <input
                type="text"
                value={cancelReason}
                onChange={(e) => setCancelReason(e.target.value)}
                className="input-field"
                required
              />
            </div>

            <div className="flex gap-2">
              <button type="button" onClick={() => setCancelModal(null)} className="btn-outline flex-1 text-xs">
                Back
              </button>
              <button type="submit" className="btn-danger flex-1 text-xs">
                Confirm Cancellation
              </button>
            </div>
          </form>
        </div>
      )}
    </div>
  );
}
