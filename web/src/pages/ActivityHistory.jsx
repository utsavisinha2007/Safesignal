import React, { useState, useEffect } from 'react';
import { History, ShieldAlert, Radio, Clock, CheckCircle2, FileText, Bell } from 'lucide-react';
import { api } from '../services/api';
import StatusBadge from '../components/StatusBadge';

export default function ActivityHistory() {
  const [activeTab, setActiveTab] = useState('events'); // events | incidents | verifications | checkins
  const [events, setEvents] = useState([]);
  const [incidents, setIncidents] = useState([]);
  const [notifications, setNotifications] = useState([]);
  const [verifications, setVerifications] = useState([]);
  const [checkins, setCheckins] = useState([]);
  const [loading, setLoading] = useState(true);

  const fetchData = async () => {
    setLoading(true);
    try {
      const [evRes, auditRes, verifRes, checkinRes] = await Promise.all([
        api.getEventsHistory(),
        api.getAuditHistory(),
        api.getVerifications(),
        api.getCheckins()
      ]);
      setEvents(evRes.events || []);
      setIncidents(auditRes.incidents || []);
      setNotifications(auditRes.notifications || []);
      setVerifications(verifRes.all || []);
      setCheckins(checkinRes.checkins || []);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchData();
  }, []);

  return (
    <div className="space-y-6">
      <div>
        <h1 className="page-title">Activity & Audit History</h1>
        <p className="page-subtitle">
          Complete chronological audit trails immutably recorded in the C++ backend structures.
        </p>
      </div>

      {/* TABS HEADER */}
      <div className="flex border-b border-gray-200 space-x-4">
        <button
          onClick={() => setActiveTab('events')}
          className={`pb-3 text-sm font-bold flex items-center gap-2 border-b-2 transition-all ${
            activeTab === 'events'
              ? 'border-violet-600 text-violet-700'
              : 'border-transparent text-gray-500 hover:text-gray-700'
          }`}
        >
          <Radio className="w-4 h-4" />
          Recent Safety Events ({events.length})
        </button>

        <button
          onClick={() => setActiveTab('incidents')}
          className={`pb-3 text-sm font-bold flex items-center gap-2 border-b-2 transition-all ${
            activeTab === 'incidents'
              ? 'border-violet-600 text-violet-700'
              : 'border-transparent text-gray-500 hover:text-gray-700'
          }`}
        >
          <ShieldAlert className="w-4 h-4" />
          Incident & Dispatch Audit ({incidents.length})
        </button>

        <button
          onClick={() => setActiveTab('verifications')}
          className={`pb-3 text-sm font-bold flex items-center gap-2 border-b-2 transition-all ${
            activeTab === 'verifications'
              ? 'border-violet-600 text-violet-700'
              : 'border-transparent text-gray-500 hover:text-gray-700'
          }`}
        >
          <FileText className="w-4 h-4" />
          Verifications ({verifications.length})
        </button>

        <button
          onClick={() => setActiveTab('checkins')}
          className={`pb-3 text-sm font-bold flex items-center gap-2 border-b-2 transition-all ${
            activeTab === 'checkins'
              ? 'border-violet-600 text-violet-700'
              : 'border-transparent text-gray-500 hover:text-gray-700'
          }`}
        >
          <Clock className="w-4 h-4" />
          Check-in Records ({checkins.length})
        </button>
      </div>

      {/* TAB CONTENT */}

      {/* 1. EVENTS TAB */}
      {activeTab === 'events' && (
        <div className="card overflow-hidden p-0">
          <div className="p-4 border-b border-gray-100 flex items-center justify-between">
            <h3 className="font-bold text-gray-900 text-sm">Active Sliding Window Events (std::deque)</h3>
            <span className="text-xs text-gray-500">10-minute sliding window</span>
          </div>

          <div className="overflow-x-auto">
            <table className="w-full text-left text-xs">
              <thead className="bg-gray-50 text-gray-500 uppercase tracking-wider font-semibold border-b border-gray-100">
                <tr>
                  <th className="px-4 py-3">Event ID</th>
                  <th className="px-4 py-3">Event Type</th>
                  <th className="px-4 py-3">Location</th>
                  <th className="px-4 py-3">Timestamp</th>
                  <th className="px-4 py-3">Severity Weight</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-100">
                {events.length === 0 ? (
                  <tr>
                    <td colSpan={5} className="px-4 py-8 text-center text-gray-400 italic">
                      Sliding window currently empty (no events in last 10 minutes).
                    </td>
                  </tr>
                ) : (
                  events.map((ev) => (
                    <tr key={ev.id} className="hover:bg-gray-50/50">
                      <td className="px-4 py-3 font-mono font-bold text-gray-700">#{ev.id}</td>
                      <td className="px-4 py-3 font-semibold text-gray-900">{ev.type}</td>
                      <td className="px-4 py-3 text-gray-600">{ev.location}</td>
                      <td className="px-4 py-3 font-mono text-gray-600">{ev.timestamp}</td>
                      <td className="px-4 py-3">
                        <span
                          className={`badge ${
                            ev.severity >= 5
                              ? 'bg-red-100 text-red-800'
                              : ev.severity >= 3
                              ? 'bg-amber-100 text-amber-800'
                              : 'bg-emerald-100 text-emerald-800'
                          }`}
                        >
                          Weight {ev.severity}
                        </span>
                      </td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* 2. INCIDENTS & DISPATCH AUDIT TAB */}
      {activeTab === 'incidents' && (
        <div className="space-y-6">
          <div className="card overflow-hidden p-0">
            <div className="p-4 border-b border-gray-100">
              <h3 className="font-bold text-gray-900 text-sm">All Incident Lifecycle Records</h3>
            </div>
            <div className="overflow-x-auto">
              <table className="w-full text-left text-xs">
                <thead className="bg-gray-50 text-gray-500 uppercase tracking-wider font-semibold border-b border-gray-100">
                  <tr>
                    <th className="px-4 py-3">Incident ID</th>
                    <th className="px-4 py-3">Description</th>
                    <th className="px-4 py-3">Location</th>
                    <th className="px-4 py-3">Severity</th>
                    <th className="px-4 py-3">Status</th>
                    <th className="px-4 py-3">Resolution / Notes</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-gray-100">
                  {incidents.length === 0 ? (
                    <tr>
                      <td colSpan={6} className="px-4 py-8 text-center text-gray-400 italic">
                        No incident records in system.
                      </td>
                    </tr>
                  ) : (
                    incidents.map((inc) => (
                      <tr key={inc.id} className="hover:bg-gray-50/50">
                        <td className="px-4 py-3 font-mono font-bold text-gray-700">#{inc.id}</td>
                        <td className="px-4 py-3 font-semibold text-gray-900">{inc.description}</td>
                        <td className="px-4 py-3 text-gray-600">{inc.location}</td>
                        <td className="px-4 py-3">
                          <span className="badge bg-red-100 text-red-800">{inc.severity}/5</span>
                        </td>
                        <td className="px-4 py-3">
                          <StatusBadge status={inc.status} />
                        </td>
                        <td className="px-4 py-3 text-gray-600">
                          {inc.resolutionNotes || inc.cancellationReason || '—'}
                        </td>
                      </tr>
                    ))
                  )}
                </tbody>
              </table>
            </div>
          </div>

          {/* NOTIFICATION BROADCAST LOG */}
          <div className="card overflow-hidden p-0">
            <div className="p-4 border-b border-gray-100 flex items-center justify-between">
              <h3 className="font-bold text-gray-900 text-sm flex items-center gap-2">
                <Bell className="w-4 h-4 text-violet-600" />
                Contact Notification Dispatch Trail
              </h3>
              <span className="text-xs text-gray-500">{notifications.length} Simulated Broadcast(s)</span>
            </div>

            <div className="overflow-x-auto">
              <table className="w-full text-left text-xs">
                <thead className="bg-gray-50 text-gray-500 uppercase tracking-wider font-semibold border-b border-gray-100">
                  <tr>
                    <th className="px-4 py-3">Incident</th>
                    <th className="px-4 py-3">Recipient Contact</th>
                    <th className="px-4 py-3">Priority Tier</th>
                    <th className="px-4 py-3">Sent Time</th>
                    <th className="px-4 py-3">Status</th>
                    <th className="px-4 py-3">Ack Time</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-gray-100">
                  {notifications.length === 0 ? (
                    <tr>
                      <td colSpan={6} className="px-4 py-8 text-center text-gray-400 italic">
                        No notification records yet.
                      </td>
                    </tr>
                  ) : (
                    notifications.map((n, idx) => (
                      <tr key={idx} className="hover:bg-gray-50/50">
                        <td className="px-4 py-3 font-mono font-bold text-gray-700">#{n.incidentID}</td>
                        <td className="px-4 py-3 font-semibold text-gray-900">
                          {n.contactName} ({n.contactPhone})
                        </td>
                        <td className="px-4 py-3">Tier {n.priorityLevel}</td>
                        <td className="px-4 py-3 font-mono text-gray-600">{n.timestamp}</td>
                        <td className="px-4 py-3">
                          <span
                            className={`badge ${
                              n.acknowledged ? 'bg-emerald-100 text-emerald-800' : 'bg-amber-100 text-amber-800'
                            }`}
                          >
                            {n.acknowledged ? 'ACKNOWLEDGED' : 'AWAITING RESPONSE'}
                          </span>
                        </td>
                        <td className="px-4 py-3 font-mono text-gray-600">
                          {n.acknowledgementTimestamp || '—'}
                        </td>
                      </tr>
                    ))
                  )}
                </tbody>
              </table>
            </div>
          </div>
        </div>
      )}

      {/* 3. VERIFICATIONS TAB */}
      {activeTab === 'verifications' && (
        <div className="card overflow-hidden p-0">
          <div className="p-4 border-b border-gray-100">
            <h3 className="font-bold text-gray-900 text-sm">All Safety Verification Requests</h3>
          </div>
          <div className="overflow-x-auto">
            <table className="w-full text-left text-xs">
              <thead className="bg-gray-50 text-gray-500 uppercase tracking-wider font-semibold border-b border-gray-100">
                <tr>
                  <th className="px-4 py-3">Request ID</th>
                  <th className="px-4 py-3">Risk at Trigger</th>
                  <th className="px-4 py-3">Trigger Location</th>
                  <th className="px-4 py-3">Created</th>
                  <th className="px-4 py-3">Deadline (3 min)</th>
                  <th className="px-4 py-3">Status</th>
                  <th className="px-4 py-3">Resolved Time</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-100">
                {verifications.length === 0 ? (
                  <tr>
                    <td colSpan={7} className="px-4 py-8 text-center text-gray-400 italic">
                      No verification requests recorded.
                    </td>
                  </tr>
                ) : (
                  verifications.map((v) => (
                    <tr key={v.requestID} className="hover:bg-gray-50/50">
                      <td className="px-4 py-3 font-mono font-bold text-gray-700">#{v.requestID}</td>
                      <td className="px-4 py-3 font-semibold text-gray-900">
                        Score {v.riskScore} ({v.riskLevel})
                      </td>
                      <td className="px-4 py-3 text-gray-600">{v.triggerLocation}</td>
                      <td className="px-4 py-3 font-mono text-gray-600">{v.creationTimestamp}</td>
                      <td className="px-4 py-3 font-mono text-gray-600">{v.deadlineTimestamp}</td>
                      <td className="px-4 py-3">
                        <StatusBadge status={v.status} />
                      </td>
                      <td className="px-4 py-3 font-mono text-gray-600">{v.resolutionTimestamp || '—'}</td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* 4. CHECKINS TAB */}
      {activeTab === 'checkins' && (
        <div className="card overflow-hidden p-0">
          <div className="p-4 border-b border-gray-100">
            <h3 className="font-bold text-gray-900 text-sm">Scheduled Check-in Log</h3>
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
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-100">
                {checkins.length === 0 ? (
                  <tr>
                    <td colSpan={5} className="px-4 py-8 text-center text-gray-400 italic">
                      No check-ins recorded.
                    </td>
                  </tr>
                ) : (
                  checkins.map((ci) => (
                    <tr key={ci.id} className="hover:bg-gray-50/50">
                      <td className="px-4 py-3 font-mono font-bold text-gray-700">#{ci.id}</td>
                      <td className="px-4 py-3 font-semibold text-gray-900">{ci.scheduledTime}</td>
                      <td className="px-4 py-3 text-gray-600">{ci.location}</td>
                      <td className="px-4 py-3 text-gray-600">{ci.description}</td>
                      <td className="px-4 py-3">
                        <StatusBadge status={ci.status} />
                      </td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
        </div>
      )}
    </div>
  );
}
