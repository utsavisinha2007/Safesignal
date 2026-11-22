import React, { useState } from 'react';
import {
  Cpu,
  Layers,
  Activity,
  FastForward,
  Send,
  CheckCircle2,
  AlertTriangle,
  Code2,
  Zap,
  RefreshCw
} from 'lucide-react';
import { api } from '../services/api';

export default function DemonstrationLab({ status, onRefresh }) {
  const [feedback, setFeedback] = useState(null);
  const [simTime, setSimTime] = useState('18:05:00');

  // Custom sensor event state
  const [eventType, setEventType] = useState('Fall Detected');
  const [eventLocation, setEventLocation] = useState('Science Quad');
  const [eventTimestamp, setEventTimestamp] = useState('18:02:00');

  const handleSimulateEvent = async (type) => {
    try {
      const nowTs = new Date().toTimeString().split(' ')[0];
      const res = await api.ingestSimEvent({
        type: type || eventType,
        location: eventLocation || 'Campus Quad',
        timestamp: eventTimestamp || nowTs
      });
      setFeedback({
        type: 'success',
        msg: `Ingested Event #${res.eventID} ("${type || eventType}") into std::deque sliding window. New Risk Score: ${res.newRiskScore}/100.`
      });
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const handleAdvanceClock = async (timeStr) => {
    try {
      const res = await api.advanceTime(timeStr);
      setFeedback({
        type: 'success',
        msg: res.message
      });
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  return (
    <div className="space-y-6">
      <div>
        <h1 className="page-title flex items-center gap-2 text-violet-900">
          <Cpu className="w-7 h-7 text-violet-600" />
          DSA Demonstration & Simulation Lab
        </h1>
        <p className="page-subtitle">
          Interactive evaluation workbench for testing Data Structures & Algorithms, scoring math, and safety state machines.
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
            <CheckCircle2 className="w-5 h-5 text-emerald-600 shrink-0" />
            <span>{feedback.msg}</span>
          </div>
          <button onClick={() => setFeedback(null)} className="text-xs font-semibold hover:underline">
            Dismiss
          </button>
        </div>
      )}

      {/* CORE DSA MAPPING TILES */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
        {/* Deque */}
        <div className="card bg-navy-950 text-white border-navy-900">
          <div className="flex items-center justify-between text-xs font-mono text-violet-400 mb-1">
            <span>std::deque&lt;Event&gt;</span>
            <Layers className="w-4 h-4" />
          </div>
          <p className="text-xs text-gray-400">Sliding Time Window</p>
          <div className="text-2xl font-bold mt-2 text-white">{status?.eventCount ?? 0} Events</div>
          <p className="text-[11px] text-gray-400 mt-2">
            Pruned in $O(1)$ amortized time as events age past 10 minutes.
          </p>
        </div>

        {/* Priority Queue */}
        <div className="card bg-navy-950 text-white border-navy-900">
          <div className="flex items-center justify-between text-xs font-mono text-red-400 mb-1">
            <span>std::priority_queue</span>
            <Activity className="w-4 h-4" />
          </div>
          <p className="text-xs text-gray-400">Emergency Max-Heap</p>
          <div className="text-2xl font-bold mt-2 text-white">{status?.pendingIncidentCount ?? 0} Pending</div>
          <p className="text-[11px] text-gray-400 mt-2">
            Highest-severity incident triaged at $O(\log n)$ root extraction.
          </p>
        </div>

        {/* Unordered Map */}
        <div className="card bg-navy-950 text-white border-navy-900">
          <div className="flex items-center justify-between text-xs font-mono text-teal-400 mb-1">
            <span>std::unordered_map</span>
            <Zap className="w-4 h-4" />
          </div>
          <p className="text-xs text-gray-400">Lifecycle Registry</p>
          <div className="text-2xl font-bold mt-2 text-white">{status?.activeIncidentCount ?? 0} Active</div>
          <p className="text-[11px] text-gray-400 mt-2">
            $O(1)$ average-case lookup, updates, and duplicate suppression.
          </p>
        </div>

        {/* Vector */}
        <div className="card bg-navy-950 text-white border-navy-900">
          <div className="flex items-center justify-between text-xs font-mono text-blue-400 mb-1">
            <span>std::vector&lt;Contact&gt;</span>
            <Code2 className="w-4 h-4" />
          </div>
          <p className="text-xs text-gray-400">Tiered Directory</p>
          <div className="text-2xl font-bold mt-2 text-white">{status?.contactCount ?? 0} Registered</div>
          <p className="text-[11px] text-gray-400 mt-2">
            Dynamic array supporting Tier 1 $\to$ Tier 2 $\to$ Tier 3 filtering.
          </p>
        </div>
      </div>

      {/* SENSOR SIMULATOR & CLOCK CONTROLLER */}
      <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
        {/* Sensor Simulation Lab */}
        <div className="card space-y-4">
          <h2 className="text-base font-bold text-gray-900 flex items-center gap-2">
            <Zap className="w-5 h-5 text-amber-500" />
            Hardware Sensor Simulator
          </h2>
          <p className="text-xs text-gray-500">
            Simulate incoming wearable sensor events. High-severity events immediately trigger safety verification or emergency triage.
          </p>

          <div className="grid grid-cols-2 gap-2">
            <button
              onClick={() => handleSimulateEvent('Fall Detected')}
              className="p-3 bg-red-50 hover:bg-red-100 text-red-700 rounded-xl text-xs font-bold border border-red-200 transition-all text-left"
            >
              ⚠️ Fall Detected (Weight 5)
              <span className="block text-[10px] font-normal text-red-500 mt-0.5">High severity sensor alert</span>
            </button>

            <button
              onClick={() => handleSimulateEvent('Impact')}
              className="p-3 bg-rose-50 hover:bg-rose-100 text-rose-700 rounded-xl text-xs font-bold border border-rose-200 transition-all text-left"
            >
              💥 Impact / Collision (Weight 5)
              <span className="block text-[10px] font-normal text-rose-500 mt-0.5">Severe physical shock</span>
            </button>

            <button
              onClick={() => handleSimulateEvent('Route Deviation')}
              className="p-3 bg-amber-50 hover:bg-amber-100 text-amber-700 rounded-xl text-xs font-bold border border-amber-200 transition-all text-left"
            >
              🧭 Route Deviation (Weight 3)
              <span className="block text-[10px] font-normal text-amber-500 mt-0.5">Off planned path</span>
            </button>

            <button
              onClick={() => handleSimulateEvent('Motion Alert')}
              className="p-3 bg-blue-50 hover:bg-blue-100 text-blue-700 rounded-xl text-xs font-bold border border-blue-200 transition-all text-left"
            >
              🏃 Rapid Movement (Weight 2)
              <span className="block text-[10px] font-normal text-blue-500 mt-0.5">Running / panic speed</span>
            </button>
          </div>

          {/* Custom Event Builder */}
          <div className="pt-4 border-t border-gray-100 space-y-3">
            <span className="text-xs font-bold text-gray-700 block">Custom Event Injection</span>
            <div className="grid grid-cols-3 gap-2">
              <input
                type="text"
                placeholder="Event Type"
                value={eventType}
                onChange={(e) => setEventType(e.target.value)}
                className="input-field text-xs"
              />
              <input
                type="text"
                placeholder="Location"
                value={eventLocation}
                onChange={(e) => setEventLocation(e.target.value)}
                className="input-field text-xs"
              />
              <input
                type="text"
                placeholder="Timestamp"
                value={eventTimestamp}
                onChange={(e) => setEventTimestamp(e.target.value)}
                className="input-field text-xs"
              />
            </div>
            <button onClick={() => handleSimulateEvent()} className="btn-primary w-full text-xs">
              Inject Custom Event
            </button>
          </div>
        </div>

        {/* Clock Simulation Controller */}
        <div className="card space-y-4">
          <h2 className="text-base font-bold text-gray-900 flex items-center gap-2">
            <FastForward className="w-5 h-5 text-violet-600" />
            Timeline & Clock Controller
          </h2>
          <p className="text-xs text-gray-500">
            Advance the simulated time. The C++ engine checks verification timeouts (3 mins), check-in grace periods (5 mins), and contact acknowledgement deadlines (2 mins).
          </p>

          <div className="flex gap-2">
            <input
              type="text"
              placeholder="HH:MM:SS"
              value={simTime}
              onChange={(e) => setSimTime(e.target.value)}
              className="input-field flex-1"
            />
            <button onClick={() => handleAdvanceClock(simTime)} className="btn-primary text-xs shrink-0">
              Set Simulated Time
            </button>
          </div>

          {/* Quick Advancement Pills */}
          <div className="flex flex-wrap gap-2 text-xs">
            <span className="text-gray-500 self-center">Quick Step:</span>
            <button
              onClick={() => handleAdvanceClock('18:03:00')}
              className="px-3 py-1.5 bg-gray-100 hover:bg-violet-100 text-gray-800 rounded-lg font-mono"
            >
              18:03:00
            </button>
            <button
              onClick={() => handleAdvanceClock('18:08:00')}
              className="px-3 py-1.5 bg-gray-100 hover:bg-violet-100 text-gray-800 rounded-lg font-mono"
            >
              18:08:00 (+5m)
            </button>
            <button
              onClick={() => handleAdvanceClock('18:15:00')}
              className="px-3 py-1.5 bg-gray-100 hover:bg-violet-100 text-gray-800 rounded-lg font-mono"
            >
              18:15:00 (+12m)
            </button>
            <button
              onClick={() => handleAdvanceClock('18:30:00')}
              className="px-3 py-1.5 bg-gray-100 hover:bg-violet-100 text-gray-800 rounded-lg font-mono"
            >
              18:30:00 (Check-in miss)
            </button>
          </div>

          {/* SCORING BREAKDOWN */}
          <div className="pt-4 border-t border-gray-100">
            <h3 className="text-xs font-bold text-gray-700 mb-2">Live Scoring Mathematics</h3>
            <div className="p-3 bg-gray-50 rounded-xl text-xs space-y-1 font-mono">
              <div className="flex justify-between">
                <span>Base Severity Sum:</span>
                <span className="font-bold">{status?.baseScore ?? 0}</span>
              </div>
              <div className="flex justify-between">
                <span>Frequency Multiplier Bonus:</span>
                <span className="font-bold">+{status?.frequencyBonus ?? 0}</span>
              </div>
              <div className="flex justify-between">
                <span>Pattern Bonus:</span>
                <span className="font-bold">+{status?.patternBonus ?? 0}</span>
              </div>
              <div className="flex justify-between pt-1 border-t border-gray-200 text-violet-800 font-bold">
                <span>Total Composite Risk:</span>
                <span>{status?.riskScore ?? 0} / 100 ({status?.riskLevel || 'Normal'})</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
