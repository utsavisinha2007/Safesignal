import React, { useState, useEffect } from 'react';
import {
  History,
  ShieldAlert,
  Radio,
  Clock,
  CheckCircle2,
  FileText,
  Bell,
  Search,
  RefreshCw,
  Layers,
  Activity,
  AlertTriangle,
  Cpu,
  ChevronRight
} from 'lucide-react';
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
  const [searchQuery, setSearchQuery] = useState('');

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
      console.error('Failed to fetch activity logs:', err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchData();
  }, []);

  // Filter helper
  const filterList = (list, fields) => {
    if (!searchQuery.trim()) return list;
    const q = searchQuery.toLowerCase();
    return list.filter((item) =>
      fields.some((f) => {
        const val = item[f];
        return val && String(val).toLowerCase().includes(q);
      })
    );
  };

  const filteredEvents = filterList(events, ['id', 'type', 'location', 'timestamp']);
  const filteredIncidents = filterList(incidents, ['id', 'description', 'location', 'status']);
  const filteredNotifications = filterList(notifications, ['incidentID', 'contactName', 'contactPhone', 'priorityLevel']);
  const filteredVerifications = filterList(verifications, ['requestID', 'triggerLocation', 'riskLevel', 'status']);
  const filteredCheckins = filterList(checkins, ['id', 'location', 'description', 'status']);

  return (
    <div className="space-y-6">
      {/* PAGE HEADER */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
        <div>
          <div className="flex items-center gap-2 mb-1">
            <span className="px-2 py-0.5 rounded text-[10px] font-mono tracking-wider font-bold bg-cmd-primary/20 text-cmd-primaryLight border border-cmd-primary/40 uppercase">
              Immutable C++ Logs
            </span>
            <span className="text-xs text-cmd-muted flex items-center gap-1 font-mono">
              <Cpu className="w-3.5 h-3.5 text-cmd-teal" />
              DSA Memory Resident
            </span>
          </div>
          <h1 className="text-2xl font-black text-cmd-text tracking-tight flex items-center gap-2">
            <History className="w-6 h-6 text-cmd-primaryLight" />
            Audit & Telemetry Records
          </h1>
          <p className="text-xs text-cmd-muted mt-1">
            High-integrity chronological audit ledger tracked across sliding deques, hash indices, and priority heaps.
          </p>
        </div>

        <div className="flex items-center gap-3">
          <div className="relative">
            <Search className="w-4 h-4 text-cmd-muted absolute left-3 top-1/2 -translate-y-1/2" />
            <input
              type="text"
              placeholder="Search audit records..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              className="input-field pl-9 py-1.5 text-xs w-56 font-mono"
            />
          </div>
          <button
            onClick={fetchData}
            disabled={loading}
            className="btn-outline flex items-center gap-2 py-1.5 px-3 text-xs"
          >
            <RefreshCw className={`w-3.5 h-3.5 ${loading ? 'animate-spin text-cmd-teal' : ''}`} />
            <span>Sync</span>
          </button>
        </div>
      </div>

      {/* TELEMETRY COUNTER TILES */}
      <div className="grid grid-cols-2 md:grid-cols-4 gap-3">
        <div className="cmd-card p-3 border-l-2 border-l-cmd-teal">
          <div className="flex items-center justify-between text-xs text-cmd-muted font-mono">
            <span>SLIDING WINDOW</span>
            <Radio className="w-3.5 h-3.5 text-cmd-teal" />
          </div>
          <div className="text-xl font-bold font-mono text-cmd-teal mt-1">
            {events.length}
          </div>
          <p className="text-[10px] text-cmd-muted font-mono mt-0.5">std::deque active events</p>
        </div>

        <div className="cmd-card p-3 border-l-2 border-l-cmd-critical">
          <div className="flex items-center justify-between text-xs text-cmd-muted font-mono">
            <span>INCIDENT LOG</span>
            <ShieldAlert className="w-3.5 h-3.5 text-cmd-critical" />
          </div>
          <div className="text-xl font-bold font-mono text-cmd-critical mt-1">
            {incidents.length}
          </div>
          <p className="text-[10px] text-cmd-muted font-mono mt-0.5">Historical life-cycles</p>
        </div>

        <div className="cmd-card p-3 border-l-2 border-l-cmd-primary">
          <div className="flex items-center justify-between text-xs text-cmd-muted font-mono">
            <span>DISPATCH TRAIL</span>
            <Bell className="w-3.5 h-3.5 text-cmd-primaryLight" />
          </div>
          <div className="text-xl font-bold font-mono text-cmd-primaryLight mt-1">
            {notifications.length}
          </div>
          <p className="text-[10px] text-cmd-muted font-mono mt-0.5">Escalated broadcasts</p>
        </div>

        <div className="cmd-card p-3 border-l-2 border-l-cmd-warning">
          <div className="flex items-center justify-between text-xs text-cmd-muted font-mono">
            <span>VERIFICATIONS</span>
            <FileText className="w-3.5 h-3.5 text-cmd-warning" />
          </div>
          <div className="text-xl font-bold font-mono text-cmd-warning mt-1">
            {verifications.length}
          </div>
          <p className="text-[10px] text-cmd-muted font-mono mt-0.5">Audit requests logged</p>
        </div>
      </div>

      {/* TABS NAVIGATION */}
      <div className="flex border-b border-cmd-border/60 overflow-x-auto gap-2">
        <button
          onClick={() => setActiveTab('events')}
          className={`pb-3 px-3 text-xs font-mono font-bold flex items-center gap-2 border-b-2 transition-all whitespace-nowrap ${
            activeTab === 'events'
              ? 'border-cmd-teal text-cmd-teal'
              : 'border-transparent text-cmd-muted hover:text-cmd-text'
          }`}
        >
          <Radio className="w-3.5 h-3.5" />
          Sliding Window Events ({filteredEvents.length})
        </button>

        <button
          onClick={() => setActiveTab('incidents')}
          className={`pb-3 px-3 text-xs font-mono font-bold flex items-center gap-2 border-b-2 transition-all whitespace-nowrap ${
            activeTab === 'incidents'
              ? 'border-cmd-critical text-rose-400'
              : 'border-transparent text-cmd-muted hover:text-cmd-text'
          }`}
        >
          <ShieldAlert className="w-3.5 h-3.5" />
          Incident & Dispatch Logs ({filteredIncidents.length})
        </button>

        <button
          onClick={() => setActiveTab('verifications')}
          className={`pb-3 px-3 text-xs font-mono font-bold flex items-center gap-2 border-b-2 transition-all whitespace-nowrap ${
            activeTab === 'verifications'
              ? 'border-cmd-warning text-amber-400'
              : 'border-transparent text-cmd-muted hover:text-cmd-text'
          }`}
        >
          <FileText className="w-3.5 h-3.5" />
          Verifications ({filteredVerifications.length})
        </button>

        <button
          onClick={() => setActiveTab('checkins')}
          className={`pb-3 px-3 text-xs font-mono font-bold flex items-center gap-2 border-b-2 transition-all whitespace-nowrap ${
            activeTab === 'checkins'
              ? 'border-cmd-primaryLight text-cmd-primaryLight'
              : 'border-transparent text-cmd-muted hover:text-cmd-text'
          }`}
        >
          <Clock className="w-3.5 h-3.5" />
          Scheduled Check-ins ({filteredCheckins.length})
        </button>
      </div>

      {/* TAB CONTENT PANELS */}

      {/* 1. EVENTS TAB */}
      {activeTab === 'events' && (
        <div className="cmd-card overflow-hidden p-0">
          <div className="p-4 border-b border-cmd-border/60 flex flex-col sm:flex-row sm:items-center justify-between gap-2 bg-cmd-card/40">
            <div>
              <h3 className="font-bold text-cmd-text text-sm flex items-center gap-2">
                <Layers className="w-4 h-4 text-cmd-teal" />
                Active Sliding Window Events
              </h3>
              <p className="text-xs text-cmd-muted font-mono mt-0.5">
                Resident in C++ <code className="text-cmd-teal">std::deque&lt;Event&gt;</code> · Pruned after 10-minute validity
              </p>
            </div>
            <span className="text-[11px] font-mono text-cmd-muted bg-cmd-elevated px-2 py-1 rounded border border-cmd-border/40">
              Pruning Complexity: O(1) Amortized
            </span>
          </div>

          <div className="overflow-x-auto">
            <table className="w-full text-left text-xs">
              <thead className="bg-cmd-elevated/70 text-cmd-muted uppercase font-mono tracking-wider text-[10px] border-b border-cmd-border/40">
                <tr>
                  <th className="px-4 py-3">Event ID</th>
                  <th className="px-4 py-3">Event Type</th>
                  <th className="px-4 py-3">Telemetry Location</th>
                  <th className="px-4 py-3">Recorded Time</th>
                  <th className="px-4 py-3">Base Weight</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-cmd-border/20 font-mono">
                {filteredEvents.length === 0 ? (
                  <tr>
                    <td colSpan={5} className="px-4 py-12 text-center text-cmd-muted italic">
                      <Radio className="w-8 h-8 text-cmd-muted/40 mx-auto mb-2 animate-pulse" />
                      No active events in sliding window. Memory buffer clear.
                    </td>
                  </tr>
                ) : (
                  filteredEvents.map((ev) => (
                    <tr key={ev.id} className="hover:bg-cmd-elevated/40 transition-colors">
                      <td className="px-4 py-3 font-bold text-cmd-teal">#{ev.id}</td>
                      <td className="px-4 py-3 font-sans font-semibold text-cmd-text flex items-center gap-1.5">
                        <span className={`w-1.5 h-1.5 rounded-full ${ev.severity >= 5 ? 'bg-cmd-critical animate-ping' : ev.severity >= 3 ? 'bg-cmd-warning' : 'bg-cmd-teal'}`} />
                        {ev.type}
                      </td>
                      <td className="px-4 py-3 text-cmd-muted font-sans">{ev.location}</td>
                      <td className="px-4 py-3 text-cmd-muted">{ev.timestamp}</td>
                      <td className="px-4 py-3">
                        <span
                          className={`px-2 py-0.5 rounded text-[10px] font-bold ${
                            ev.severity >= 5
                              ? 'bg-cmd-critical/20 text-rose-300 border border-cmd-critical/40'
                              : ev.severity >= 3
                              ? 'bg-cmd-warning/20 text-amber-300 border border-cmd-warning/40'
                              : 'bg-cmd-teal/20 text-teal-300 border border-cmd-teal/40'
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
          {/* INCIDENT LIFECYCLE TABLE */}
          <div className="cmd-card overflow-hidden p-0">
            <div className="p-4 border-b border-cmd-border/60 flex items-center justify-between bg-cmd-card/40">
              <div>
                <h3 className="font-bold text-cmd-text text-sm flex items-center gap-2">
                  <ShieldAlert className="w-4 h-4 text-cmd-critical" />
                  Incident Lifecycle & Resolution Records
                </h3>
                <p className="text-xs text-cmd-muted font-mono mt-0.5">
                  Managed via <code className="text-cmd-critical">std::unordered_map&lt;int, Incident&gt;</code> & Max-Heap
                </p>
              </div>
              <span className="text-xs font-mono text-cmd-muted">
                {incidents.length} recorded
              </span>
            </div>

            <div className="overflow-x-auto">
              <table className="w-full text-left text-xs">
                <thead className="bg-cmd-elevated/70 text-cmd-muted uppercase font-mono tracking-wider text-[10px] border-b border-cmd-border/40">
                  <tr>
                    <th className="px-4 py-3">Incident ID</th>
                    <th className="px-4 py-3">Description</th>
                    <th className="px-4 py-3">Coordinates / Location</th>
                    <th className="px-4 py-3">Severity</th>
                    <th className="px-4 py-3">Status</th>
                    <th className="px-4 py-3">Resolution / Notes</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-cmd-border/20 font-mono">
                  {filteredIncidents.length === 0 ? (
                    <tr>
                      <td colSpan={6} className="px-4 py-12 text-center text-cmd-muted italic">
                        <CheckCircle2 className="w-8 h-8 text-cmd-safe/40 mx-auto mb-2" />
                        No incidents registered in incident ledger.
                      </td>
                    </tr>
                  ) : (
                    filteredIncidents.map((inc) => (
                      <tr key={inc.id} className="hover:bg-cmd-elevated/40 transition-colors">
                        <td className="px-4 py-3 font-bold text-rose-400">#{inc.id}</td>
                        <td className="px-4 py-3 font-sans font-semibold text-cmd-text">
                          {inc.description}
                        </td>
                        <td className="px-4 py-3 text-cmd-muted font-sans">{inc.location}</td>
                        <td className="px-4 py-3">
                          <span className="px-2 py-0.5 rounded text-[10px] font-bold bg-cmd-critical/20 text-rose-300 border border-cmd-critical/40">
                            {inc.severity}/5
                          </span>
                        </td>
                        <td className="px-4 py-3">
                          <StatusBadge status={inc.status} />
                        </td>
                        <td className="px-4 py-3 text-cmd-muted font-sans">
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
          <div className="cmd-card overflow-hidden p-0">
            <div className="p-4 border-b border-cmd-border/60 flex items-center justify-between bg-cmd-card/40">
              <div className="flex items-center gap-2">
                <Bell className="w-4 h-4 text-cmd-primaryLight" />
                <h3 className="font-bold text-cmd-text text-sm">
                  Contact Broadcast & Escalation Audit Trail
                </h3>
              </div>
              <span className="text-xs font-mono text-cmd-muted">
                {notifications.length} simulated SMS/Alert dispatch(es)
              </span>
            </div>

            <div className="overflow-x-auto">
              <table className="w-full text-left text-xs">
                <thead className="bg-cmd-elevated/70 text-cmd-muted uppercase font-mono tracking-wider text-[10px] border-b border-cmd-border/40">
                  <tr>
                    <th className="px-4 py-3">Incident</th>
                    <th className="px-4 py-3">Recipient Contact</th>
                    <th className="px-4 py-3">Tier</th>
                    <th className="px-4 py-3">Broadcast Sent</th>
                    <th className="px-4 py-3">Acknowledgement</th>
                    <th className="px-4 py-3">Ack Time</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-cmd-border/20 font-mono">
                  {filteredNotifications.length === 0 ? (
                    <tr>
                      <td colSpan={6} className="px-4 py-8 text-center text-cmd-muted italic">
                        No contact notifications dispatched.
                      </td>
                    </tr>
                  ) : (
                    filteredNotifications.map((n, idx) => (
                      <tr key={idx} className="hover:bg-cmd-elevated/40 transition-colors">
                        <td className="px-4 py-3 font-bold text-rose-400">#{n.incidentID}</td>
                        <td className="px-4 py-3 font-sans font-semibold text-cmd-text">
                          {n.contactName} <span className="font-mono text-cmd-muted text-xs">({n.contactPhone})</span>
                        </td>
                        <td className="px-4 py-3">
                          <span className={`px-2 py-0.5 rounded text-[10px] font-bold ${
                            n.priorityLevel === 1 ? 'bg-cmd-critical/20 text-rose-300' :
                            n.priorityLevel === 2 ? 'bg-cmd-primary/20 text-violet-300' : 'bg-cmd-teal/20 text-teal-300'
                          }`}>
                            Tier {n.priorityLevel}
                          </span>
                        </td>
                        <td className="px-4 py-3 text-cmd-muted">{n.timestamp}</td>
                        <td className="px-4 py-3">
                          <span
                            className={`px-2 py-0.5 rounded text-[10px] font-bold ${
                              n.acknowledged
                                ? 'bg-cmd-safe/20 text-cmd-safe border border-cmd-safe/40'
                                : 'bg-cmd-warning/20 text-cmd-warning border border-cmd-warning/40'
                            }`}
                          >
                            {n.acknowledged ? 'ACKNOWLEDGED' : 'AWAITING RESPONSE'}
                          </span>
                        </td>
                        <td className="px-4 py-3 text-cmd-muted">
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
        <div className="cmd-card overflow-hidden p-0">
          <div className="p-4 border-b border-cmd-border/60 flex items-center justify-between bg-cmd-card/40">
            <div>
              <h3 className="font-bold text-cmd-text text-sm flex items-center gap-2">
                <FileText className="w-4 h-4 text-cmd-warning" />
                Safety Verification Request History
              </h3>
              <p className="text-xs text-cmd-muted font-mono mt-0.5">
                3-minute user safety prompt ledger with auto-escalation tracking
              </p>
            </div>
            <span className="text-xs font-mono text-cmd-muted">
              {verifications.length} verifications logged
            </span>
          </div>

          <div className="overflow-x-auto">
            <table className="w-full text-left text-xs">
              <thead className="bg-cmd-elevated/70 text-cmd-muted uppercase font-mono tracking-wider text-[10px] border-b border-cmd-border/40">
                <tr>
                  <th className="px-4 py-3">Request ID</th>
                  <th className="px-4 py-3">Trigger Risk</th>
                  <th className="px-4 py-3">Location</th>
                  <th className="px-4 py-3">Created</th>
                  <th className="px-4 py-3">Deadline (3m)</th>
                  <th className="px-4 py-3">Status</th>
                  <th className="px-4 py-3">Resolved Time</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-cmd-border/20 font-mono">
                {filteredVerifications.length === 0 ? (
                  <tr>
                    <td colSpan={7} className="px-4 py-12 text-center text-cmd-muted italic">
                      <CheckCircle2 className="w-8 h-8 text-cmd-safe/40 mx-auto mb-2" />
                      No safety verification requests recorded.
                    </td>
                  </tr>
                ) : (
                  filteredVerifications.map((v) => (
                    <tr key={v.requestID} className="hover:bg-cmd-elevated/40 transition-colors">
                      <td className="px-4 py-3 font-bold text-amber-400">#{v.requestID}</td>
                      <td className="px-4 py-3 font-sans font-semibold text-cmd-text">
                        Score {v.riskScore} <span className="font-mono text-cmd-muted text-xs">({v.riskLevel})</span>
                      </td>
                      <td className="px-4 py-3 text-cmd-muted font-sans">{v.triggerLocation}</td>
                      <td className="px-4 py-3 text-cmd-muted">{v.creationTimestamp}</td>
                      <td className="px-4 py-3 text-cmd-muted">{v.deadlineTimestamp}</td>
                      <td className="px-4 py-3">
                        <StatusBadge status={v.status} />
                      </td>
                      <td className="px-4 py-3 text-cmd-muted">{v.resolutionTimestamp || '—'}</td>
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
        <div className="cmd-card overflow-hidden p-0">
          <div className="p-4 border-b border-cmd-border/60 flex items-center justify-between bg-cmd-card/40">
            <div>
              <h3 className="font-bold text-cmd-text text-sm flex items-center gap-2">
                <Clock className="w-4 h-4 text-cmd-primaryLight" />
                Scheduled Check-in Registry & Log
              </h3>
              <p className="text-xs text-cmd-muted font-mono mt-0.5">
                Proactive time-checkpoint monitoring with 5-minute grace tolerance
              </p>
            </div>
            <span className="text-xs font-mono text-cmd-muted">
              {checkins.length} check-in(s)
            </span>
          </div>

          <div className="overflow-x-auto">
            <table className="w-full text-left text-xs">
              <thead className="bg-cmd-elevated/70 text-cmd-muted uppercase font-mono tracking-wider text-[10px] border-b border-cmd-border/40">
                <tr>
                  <th className="px-4 py-3">ID</th>
                  <th className="px-4 py-3">Scheduled Time</th>
                  <th className="px-4 py-3">Target Location</th>
                  <th className="px-4 py-3">Description</th>
                  <th className="px-4 py-3">Status</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-cmd-border/20 font-mono">
                {filteredCheckins.length === 0 ? (
                  <tr>
                    <td colSpan={5} className="px-4 py-12 text-center text-cmd-muted italic">
                      <Clock className="w-8 h-8 text-cmd-muted/40 mx-auto mb-2" />
                      No scheduled safety check-ins found.
                    </td>
                  </tr>
                ) : (
                  filteredCheckins.map((ci) => (
                    <tr key={ci.id} className="hover:bg-cmd-elevated/40 transition-colors">
                      <td className="px-4 py-3 font-bold text-cmd-teal">#{ci.id}</td>
                      <td className="px-4 py-3 text-cmd-text font-bold">{ci.scheduledTime}</td>
                      <td className="px-4 py-3 text-cmd-muted font-sans">{ci.location}</td>
                      <td className="px-4 py-3 text-cmd-muted font-sans">{ci.description}</td>
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
