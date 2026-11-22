import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  ShieldAlert,
  ShieldCheck,
  MapPin,
  Clock,
  Users,
  AlertTriangle,
  ArrowRight,
  CheckCircle2,
  XCircle,
  HelpCircle,
  AlertOctagon
} from 'lucide-react';
import { api } from '../services/api';

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
      {/* Page Header */}
      <div>
        <h1 className="page-title">Personal Safety Dashboard</h1>
        <p className="page-subtitle">Real-time status monitored by the SafeSignal C++ Safety Engine</p>
      </div>

      {/* Action Feedback Notification */}
      {actionFeedback && (
        <div
          className={`p-4 rounded-xl flex items-center justify-between text-sm ${
            actionFeedback.type === 'success'
              ? 'bg-emerald-50 text-emerald-800 border border-emerald-200'
              : 'bg-red-50 text-red-800 border border-red-200'
          }`}
        >
          <div className="flex items-center gap-2">
            {actionFeedback.type === 'success' ? (
              <CheckCircle2 className="w-5 h-5 text-emerald-600 shrink-0" />
            ) : (
              <XCircle className="w-5 h-5 text-red-600 shrink-0" />
            )}
            <span>{actionFeedback.msg}</span>
          </div>
          <button onClick={() => setActionFeedback(null)} className="text-xs font-semibold hover:underline">
            Dismiss
          </button>
        </div>
      )}

      {/* PENDING VERIFICATION ALERT HERO BANNER */}
      {hasVerification && (
        <div className="bg-gradient-to-r from-red-600 to-rose-700 text-white rounded-2xl p-6 shadow-lg border-2 border-red-400 animate-pulse">
          <div className="flex flex-col md:flex-row md:items-center justify-between gap-6">
            <div className="flex items-start gap-4">
              <div className="p-3 bg-white/20 rounded-xl shrink-0">
                <AlertOctagon className="w-8 h-8 text-white" />
              </div>
              <div>
                <span className="inline-block px-2.5 py-0.5 rounded-full text-xs font-bold bg-white text-red-700 mb-2">
                  ACTION REQUIRED • VERIFICATION #{activeVerifID}
                </span>
                <h2 className="text-xl font-bold">Safety Verification Request Pending</h2>
                <p className="text-sm text-red-100 mt-1 max-w-xl">
                  SafeSignal detected abnormal activity (Risk Score: {riskScore}/100 [{riskLevel}]). Please confirm your safety within the 3-minute deadline before emergency dispatch initiates.
                </p>
              </div>
            </div>

            <div className="flex flex-col sm:flex-row gap-3 shrink-0">
              <button
                onClick={handleConfirmSafe}
                disabled={loadingAction}
                className="px-6 py-3 bg-white text-emerald-700 hover:bg-emerald-50 font-bold rounded-xl shadow-md transition-all text-sm flex items-center justify-center gap-2"
              >
                <CheckCircle2 className="w-4 h-4 text-emerald-600" />
                I AM SAFE (False Alarm)
              </button>
              <button
                onClick={handleRequestHelp}
                disabled={loadingAction}
                className="px-6 py-3 bg-red-950 text-white hover:bg-black font-bold rounded-xl shadow-md transition-all text-sm flex items-center justify-center gap-2 border border-red-400/40"
              >
                <AlertTriangle className="w-4 h-4 text-red-400" />
                I NEED HELP NOW
              </button>
            </div>
          </div>
        </div>
      )}

      {/* MAIN STATUS TILES */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* Risk Score Tile */}
        <div className="card flex flex-col justify-between">
          <div className="flex items-start justify-between">
            <div>
              <p className="text-xs font-semibold text-gray-500 uppercase tracking-wider">Current Risk Score</p>
              <div className="flex items-baseline gap-2 mt-2">
                <span className="text-4xl font-extrabold text-gray-900">{riskScore}</span>
                <span className="text-sm font-semibold text-gray-500">/ 100</span>
              </div>
            </div>
            <div
              className={`p-3 rounded-2xl ${
                isHighRisk || riskScore >= 50
                  ? 'bg-red-100 text-red-600'
                  : riskScore >= 20
                  ? 'bg-amber-100 text-amber-600'
                  : 'bg-emerald-100 text-emerald-600'
              }`}
            >
              {isHighRisk || riskScore >= 50 ? (
                <ShieldAlert className="w-6 h-6" />
              ) : (
                <ShieldCheck className="w-6 h-6" />
              )}
            </div>
          </div>

          <div className="mt-4 pt-4 border-t border-gray-100">
            <div className="flex items-center justify-between text-xs mb-1.5">
              <span className="text-gray-600">Risk Assessment:</span>
              <span
                className={`font-bold ${
                  riskScore >= 50 ? 'text-red-600' : riskScore >= 20 ? 'text-amber-600' : 'text-emerald-600'
                }`}
              >
                {riskLevel}
              </span>
            </div>
            {/* Visual Risk Progress Bar */}
            <div className="w-full bg-gray-100 h-2 rounded-full overflow-hidden">
              <div
                className={`h-full transition-all duration-500 ${
                  riskScore >= 50 ? 'bg-red-500' : riskScore >= 20 ? 'bg-amber-500' : 'bg-emerald-500'
                }`}
                style={{ width: `${Math.min(100, Math.max(5, riskScore))}%` }}
              />
            </div>
            {status?.detectedPatterns?.length > 0 && (
              <p className="text-[11px] text-red-600 mt-2 font-medium">
                Detected Pattern: {status.detectedPatterns.join(', ')}
              </p>
            )}
          </div>
        </div>

        {/* Active Journey Tile */}
        <div className="card flex flex-col justify-between">
          <div className="flex items-start justify-between">
            <div>
              <p className="text-xs font-semibold text-gray-500 uppercase tracking-wider">Active Journey</p>
              <div className="mt-2">
                <span
                  className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-bold ${
                    journey?.active
                      ? 'bg-emerald-100 text-emerald-800 border border-emerald-200'
                      : 'bg-gray-100 text-gray-600'
                  }`}
                >
                  {journey?.active ? 'TRACKING IN PROGRESS' : 'NO ACTIVE JOURNEY'}
                </span>
              </div>
            </div>
            <div className="p-3 bg-violet-100 text-violet-600 rounded-2xl">
              <MapPin className="w-6 h-6" />
            </div>
          </div>

          <div className="mt-4 pt-4 border-t border-gray-100">
            {journey?.active ? (
              <div className="space-y-1.5 text-xs">
                <div className="flex justify-between">
                  <span className="text-gray-500">Destination:</span>
                  <span className="font-semibold text-gray-900">{journey.destination}</span>
                </div>
                <div className="flex justify-between">
                  <span className="text-gray-500">Next Target:</span>
                  <span className="font-semibold text-violet-700">{journey.nextExpectedCheckpoint || 'Arriving soon'}</span>
                </div>
                <div className="flex justify-between">
                  <span className="text-gray-500">ETA:</span>
                  <span className="font-semibold text-gray-900">{journey.expectedArrivalTime}</span>
                </div>
              </div>
            ) : (
              <p className="text-xs text-gray-500">
                Plan a walking route with checkpoints to enable automated route deviation monitoring.
              </p>
            )}
            <button
              onClick={() => navigate('/journey')}
              className="mt-3 text-xs font-semibold text-violet-600 hover:text-violet-800 flex items-center gap-1"
            >
              {journey?.active ? 'View Journey Progress' : 'Start a Safe Journey'}
              <ArrowRight className="w-3.5 h-3.5" />
            </button>
          </div>
        </div>

        {/* Safety Check-ins & Contacts Tile */}
        <div className="card flex flex-col justify-between">
          <div className="flex items-start justify-between">
            <div>
              <p className="text-xs font-semibold text-gray-500 uppercase tracking-wider">Safety Overview</p>
              <div className="mt-2 space-y-1">
                <div className="flex items-center gap-2">
                  <Clock className="w-4 h-4 text-violet-500" />
                  <span className="text-sm font-semibold text-gray-900">
                    {status?.pendingCheckIns ?? 0} Pending Check-in(s)
                  </span>
                </div>
                <div className="flex items-center gap-2">
                  <Users className="w-4 h-4 text-violet-500" />
                  <span className="text-sm font-semibold text-gray-900">
                    {status?.contactCount ?? 0} Emergency Contacts
                  </span>
                </div>
              </div>
            </div>
            <div className="p-3 bg-teal-100 text-teal-600 rounded-2xl">
              <Clock className="w-6 h-6" />
            </div>
          </div>

          <div className="mt-4 pt-4 border-t border-gray-100 flex items-center justify-between">
            <span className="text-xs text-gray-500">
              Active Incidents: <strong>{status?.activeIncidentCount ?? 0}</strong>
            </span>
            <button
              onClick={() => navigate('/emergency')}
              className="text-xs font-semibold text-red-600 hover:text-red-800 flex items-center gap-1"
            >
              Emergency Center
              <ArrowRight className="w-3.5 h-3.5" />
            </button>
          </div>
        </div>
      </div>

      {/* QUICK ACTIONS ROW */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
        <button
          onClick={() => navigate('/journey')}
          className="p-4 bg-white rounded-xl border border-gray-200 hover:border-violet-300 hover:shadow-sm text-left transition-all flex items-center gap-3"
        >
          <div className="p-2.5 bg-violet-50 text-violet-600 rounded-lg">
            <MapPin className="w-5 h-5" />
          </div>
          <div>
            <h4 className="text-sm font-bold text-gray-900">Journey Tracking</h4>
            <p className="text-xs text-gray-500">Route & deviation</p>
          </div>
        </button>

        <button
          onClick={() => navigate('/checkins')}
          className="p-4 bg-white rounded-xl border border-gray-200 hover:border-violet-300 hover:shadow-sm text-left transition-all flex items-center gap-3"
        >
          <div className="p-2.5 bg-teal-50 text-teal-600 rounded-lg">
            <Clock className="w-5 h-5" />
          </div>
          <div>
            <h4 className="text-sm font-bold text-gray-900">Safety Check-in</h4>
            <p className="text-xs text-gray-500">Confirm "I'm Safe"</p>
          </div>
        </button>

        <button
          onClick={() => navigate('/contacts')}
          className="p-4 bg-white rounded-xl border border-gray-200 hover:border-violet-300 hover:shadow-sm text-left transition-all flex items-center gap-3"
        >
          <div className="p-2.5 bg-blue-50 text-blue-600 rounded-lg">
            <Users className="w-5 h-5" />
          </div>
          <div>
            <h4 className="text-sm font-bold text-gray-900">Contacts</h4>
            <p className="text-xs text-gray-500">3 Priority tiers</p>
          </div>
        </button>

        <button
          onClick={() => navigate('/emergency')}
          className="p-4 bg-red-50 rounded-xl border border-red-200 hover:border-red-400 hover:shadow-sm text-left transition-all flex items-center gap-3"
        >
          <div className="p-2.5 bg-red-600 text-white rounded-lg">
            <AlertTriangle className="w-5 h-5" />
          </div>
          <div>
            <h4 className="text-sm font-bold text-red-900">One-Touch SOS</h4>
            <p className="text-xs text-red-600">Immediate dispatch</p>
          </div>
        </button>
      </div>
    </div>
  );
}
