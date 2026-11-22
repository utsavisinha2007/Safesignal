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
  RefreshCw,
  ShieldAlert,
  Flame,
  Radio,
  Clock,
  Compass,
  PlayCircle,
  HelpCircle
} from 'lucide-react';
import { api } from '../services/api';

export default function DemonstrationLab({ status, onRefresh }) {
  const [feedback, setFeedback] = useState(null);
  const [simTime, setSimTime] = useState('18:05:00');
  const [scenarioRunning, setScenarioRunning] = useState(false);

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
        msg: `Ingested Event #${res.eventID} ("${type || eventType}") into std::deque. New Risk Score: ${res.newRiskScore}/100 (${res.newRiskScore >= 70 ? 'CRITICAL' : res.newRiskScore >= 40 ? 'ELEVATED' : 'NORMAL'}).`
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

  // Pre-configured Guided Demonstration Scenarios for Viva / Presentation
  const runDemonstrationScenario = async (scenarioType) => {
    setScenarioRunning(true);
    setFeedback({ type: 'info', msg: `Executing Demonstration Scenario: ${scenarioType}...` });
    try {
      if (scenarioType === 'VERIFICATION_ESCALATION') {
        // Step 1: Ingest Route Deviation
        await api.ingestSimEvent({
          type: 'Route Deviation',
          location: 'East Corridor Dark Alley',
          timestamp: '18:00:00'
        });
        // Step 2: Ingest Fall Detected (Bumps score into Elevated/Critical)
        await api.ingestSimEvent({
          type: 'Fall Detected',
          location: 'East Corridor Dark Alley',
          timestamp: '18:01:00'
        });
        // Step 3: Advance clock past 3-minute verification timeout to trigger Tier 1 dispatch
        await api.advanceTime('18:05:00');
        setFeedback({
          type: 'success',
          msg: 'Scenario Complete: Sensor events ingested into std::deque -> Verification triggered -> Clock advanced +4m -> Verification timed out -> Incident automatically pushed into std::priority_queue and dispatched to Tier 1 contacts!'
        });
      } else if (scenarioType === 'DIRECT_SOS') {
        // Direct critical panic
        await api.ingestSimEvent({
          type: 'SOS Button',
          location: 'Library Basement Corridor',
          timestamp: '18:00:00'
        });
        setFeedback({
          type: 'success',
          msg: 'Scenario Complete: SOS Button pressed -> Immediate severity 5 event ingested -> Critical triage bypasses verification and directly creates Priority 1 Incident!'
        });
      }
      onRefresh();
    } catch (err) {
      setFeedback({ type: 'error', msg: `Scenario error: ${err.message}` });
    } finally {
      setScenarioRunning(false);
    }
  };

  return (
    <div className="space-y-6">
      {/* PAGE HEADER */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
        <div>
          <div className="flex items-center gap-2 mb-1">
            <span className="px-2 py-0.5 rounded text-[10px] font-mono tracking-wider font-bold bg-cmd-primary/20 text-cmd-primaryLight border border-cmd-primary/40 uppercase">
              Evaluation Workbench
            </span>
            <span className="text-xs text-cmd-muted flex items-center gap-1 font-mono">
              <Cpu className="w-3.5 h-3.5 text-cmd-teal" />
              C++17 Engine Live Inspector
            </span>
          </div>
          <h1 className="text-2xl font-black text-cmd-text tracking-tight flex items-center gap-2">
            <Cpu className="w-6 h-6 text-cmd-primaryLight" />
            DSA Demonstration & Simulation Lab
          </h1>
          <p className="text-xs text-cmd-muted mt-1">
            Interactive evaluation interface demonstrating Data Structures & Algorithms, real-time telemetry ingestion, and state-machine transitions.
          </p>
        </div>

        <button
          onClick={onRefresh}
          className="btn-outline self-start md:self-auto flex items-center gap-2 text-xs py-1.5 px-3"
        >
          <RefreshCw className="w-3.5 h-3.5" />
          <span>Refresh State</span>
        </button>
      </div>

      {/* FEEDBACK BANNER */}
      {feedback && (
        <div
          className={`p-3.5 rounded-xl flex items-center justify-between text-xs font-mono border transition-all ${
            feedback.type === 'success'
              ? 'bg-cmd-safe/10 text-emerald-300 border-cmd-safe/30'
              : feedback.type === 'info'
              ? 'bg-cmd-primary/10 text-violet-300 border-cmd-primary/30'
              : 'bg-cmd-critical/10 text-rose-300 border-cmd-critical/30'
          }`}
        >
          <div className="flex items-center gap-2">
            {feedback.type === 'success' ? (
              <CheckCircle2 className="w-4 h-4 text-cmd-safe shrink-0" />
            ) : (
              <AlertTriangle className="w-4 h-4 text-cmd-critical shrink-0" />
            )}
            <span>{feedback.msg}</span>
          </div>
          <button
            onClick={() => setFeedback(null)}
            className="text-[10px] uppercase font-bold text-cmd-muted hover:text-cmd-text ml-4 underline"
          >
            Dismiss
          </button>
        </div>
      )}

      {/* CORE DSA ARCHITECTURE HERO TILES */}
      <div>
        <div className="flex items-center justify-between mb-3">
          <h2 className="text-xs font-mono font-bold tracking-wider uppercase text-cmd-muted flex items-center gap-1.5">
            <Layers className="w-3.5 h-3.5 text-cmd-primaryLight" />
            Active C++ Data Structures & Computational Complexity
          </h2>
          <span className="text-[10px] font-mono text-cmd-muted">Standard Template Library (STL)</span>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-4 gap-3">
          {/* 1. Deque */}
          <div className="cmd-card p-4 border-t-2 border-t-cmd-teal relative overflow-hidden group">
            <div className="flex items-center justify-between text-xs font-mono text-cmd-teal mb-1">
              <span className="font-bold">std::deque&lt;Event&gt;</span>
              <Layers className="w-4 h-4" />
            </div>
            <p className="text-[11px] text-cmd-muted font-sans font-medium">Sliding Time Window</p>
            <div className="text-2xl font-black font-mono mt-2 text-cmd-text">
              {status?.eventCount ?? 0}{' '}
              <span className="text-xs font-normal text-cmd-muted">Events</span>
            </div>
            <div className="mt-3 pt-2.5 border-t border-cmd-border/40 text-[10px] font-mono text-cmd-muted space-y-1">
              <div className="flex justify-between">
                <span>Push Complexity:</span>
                <span className="text-cmd-teal font-bold">O(1)</span>
              </div>
              <div className="flex justify-between">
                <span>Front Pruning:</span>
                <span className="text-cmd-teal font-bold">O(1) amortized</span>
              </div>
              <p className="text-[9px] text-cmd-muted/80 pt-1">
                Aged entries past 10 min popped sequentially.
              </p>
            </div>
          </div>

          {/* 2. Priority Queue */}
          <div className="cmd-card p-4 border-t-2 border-t-cmd-critical relative overflow-hidden group">
            <div className="flex items-center justify-between text-xs font-mono text-rose-400 mb-1">
              <span className="font-bold">std::priority_queue</span>
              <Activity className="w-4 h-4" />
            </div>
            <p className="text-[11px] text-cmd-muted font-sans font-medium">Emergency Max-Heap</p>
            <div className="text-2xl font-black font-mono mt-2 text-cmd-text">
              {status?.pendingIncidentCount ?? 0}{' '}
              <span className="text-xs font-normal text-cmd-muted">Queued</span>
            </div>
            <div className="mt-3 pt-2.5 border-t border-cmd-border/40 text-[10px] font-mono text-cmd-muted space-y-1">
              <div className="flex justify-between">
                <span>Top Extract:</span>
                <span className="text-rose-400 font-bold">O(1) peek</span>
              </div>
              <div className="flex justify-between">
                <span>Heap Rebalance:</span>
                <span className="text-rose-400 font-bold">O(log n)</span>
              </div>
              <p className="text-[9px] text-cmd-muted/80 pt-1">
                Comparator orders by Severity &gt; Timestamp.
              </p>
            </div>
          </div>

          {/* 3. Unordered Map */}
          <div className="cmd-card p-4 border-t-2 border-t-cmd-warning relative overflow-hidden group">
            <div className="flex items-center justify-between text-xs font-mono text-amber-400 mb-1">
              <span className="font-bold">std::unordered_map</span>
              <Zap className="w-4 h-4" />
            </div>
            <p className="text-[11px] text-cmd-muted font-sans font-medium">Incident Lifecycle Hash</p>
            <div className="text-2xl font-black font-mono mt-2 text-cmd-text">
              {status?.activeIncidentCount ?? 0}{' '}
              <span className="text-xs font-normal text-cmd-muted">Active</span>
            </div>
            <div className="mt-3 pt-2.5 border-t border-cmd-border/40 text-[10px] font-mono text-cmd-muted space-y-1">
              <div className="flex justify-between">
                <span>ID Lookup:</span>
                <span className="text-amber-400 font-bold">O(1) average</span>
              </div>
              <div className="flex justify-between">
                <span>State Updates:</span>
                <span className="text-amber-400 font-bold">O(1) in-place</span>
              </div>
              <p className="text-[9px] text-cmd-muted/80 pt-1">
                Tracks state from PENDING to RESOLVED.
              </p>
            </div>
          </div>

          {/* 4. Vector */}
          <div className="cmd-card p-4 border-t-2 border-t-cmd-primaryLight relative overflow-hidden group">
            <div className="flex items-center justify-between text-xs font-mono text-cmd-primaryLight mb-1">
              <span className="font-bold">std::vector&lt;Contact&gt;</span>
              <Code2 className="w-4 h-4" />
            </div>
            <p className="text-[11px] text-cmd-muted font-sans font-medium">Tiered Hierarchy Array</p>
            <div className="text-2xl font-black font-mono mt-2 text-cmd-text">
              {status?.contactCount ?? 0}{' '}
              <span className="text-xs font-normal text-cmd-muted">Contacts</span>
            </div>
            <div className="mt-3 pt-2.5 border-t border-cmd-border/40 text-[10px] font-mono text-cmd-muted space-y-1">
              <div className="flex justify-between">
                <span>Access:</span>
                <span className="text-cmd-primaryLight font-bold">O(1) index</span>
              </div>
              <div className="flex justify-between">
                <span>Tier Filtering:</span>
                <span className="text-cmd-primaryLight font-bold">O(n) linear</span>
              </div>
              <p className="text-[9px] text-cmd-muted/80 pt-1">
                Contiguous memory, cache-friendly iteration.
              </p>
            </div>
          </div>
        </div>
      </div>

      {/* SENSOR SIMULATION & TIMELINE CONTROLLER */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Sensor Simulation Lab */}
        <div className="cmd-card space-y-4">
          <div className="flex items-center justify-between pb-3 border-b border-cmd-border/60">
            <h2 className="text-sm font-bold text-cmd-text flex items-center gap-2">
              <Zap className="w-4 h-4 text-amber-400" />
              Hardware Telemetry Sensor Simulator
            </h2>
            <span className="text-[10px] font-mono text-cmd-muted uppercase bg-cmd-elevated px-2 py-0.5 rounded border border-cmd-border/40">
              Direct Injection
            </span>
          </div>

          <p className="text-xs text-cmd-muted">
            Simulate wearable/mobile device sensor alerts. High-severity events or repeated threshold patterns will automatically elevate risk score and trigger safety state transitions.
          </p>

          {/* Quick-Trigger Hazard Buttons */}
          <div className="grid grid-cols-2 gap-2">
            <button
              onClick={() => handleSimulateEvent('Fall Detected')}
              className="p-3 rounded-xl text-left bg-cmd-critical/10 hover:bg-cmd-critical/20 text-rose-300 border border-cmd-critical/30 transition-all group"
            >
              <div className="flex items-center justify-between">
                <span className="font-bold text-xs flex items-center gap-1.5">
                  <Flame className="w-3.5 h-3.5 text-rose-400" />
                  Fall Detected
                </span>
                <span className="text-[10px] font-mono bg-cmd-critical/30 px-1.5 py-0.5 rounded text-rose-200">
                  W=5
                </span>
              </div>
              <span className="block text-[10px] text-rose-400/80 mt-1 font-mono">
                Accelerometer shock signature
              </span>
            </button>

            <button
              onClick={() => handleSimulateEvent('Impact')}
              className="p-3 rounded-xl text-left bg-rose-950/20 hover:bg-rose-950/40 text-rose-200 border border-rose-800/40 transition-all group"
            >
              <div className="flex items-center justify-between">
                <span className="font-bold text-xs flex items-center gap-1.5">
                  <ShieldAlert className="w-3.5 h-3.5 text-rose-400" />
                  Severe Impact
                </span>
                <span className="text-[10px] font-mono bg-rose-900/40 px-1.5 py-0.5 rounded text-rose-200">
                  W=5
                </span>
              </div>
              <span className="block text-[10px] text-rose-300/70 mt-1 font-mono">
                Collision / violent jolt
              </span>
            </button>

            <button
              onClick={() => handleSimulateEvent('Route Deviation')}
              className="p-3 rounded-xl text-left bg-cmd-warning/10 hover:bg-cmd-warning/20 text-amber-300 border border-cmd-warning/30 transition-all group"
            >
              <div className="flex items-center justify-between">
                <span className="font-bold text-xs flex items-center gap-1.5">
                  <Compass className="w-3.5 h-3.5 text-amber-400" />
                  Route Deviation
                </span>
                <span className="text-[10px] font-mono bg-cmd-warning/30 px-1.5 py-0.5 rounded text-amber-200">
                  W=3
                </span>
              </div>
              <span className="block text-[10px] text-amber-400/80 mt-1 font-mono">
                GPS off registered path
              </span>
            </button>

            <button
              onClick={() => handleSimulateEvent('Rapid Movement')}
              className="p-3 rounded-xl text-left bg-cmd-teal/10 hover:bg-cmd-teal/20 text-teal-300 border border-cmd-teal/30 transition-all group"
            >
              <div className="flex items-center justify-between">
                <span className="font-bold text-xs flex items-center gap-1.5">
                  <Activity className="w-3.5 h-3.5 text-cmd-teal" />
                  Rapid Movement
                </span>
                <span className="text-[10px] font-mono bg-cmd-teal/30 px-1.5 py-0.5 rounded text-teal-200">
                  W=2
                </span>
              </div>
              <span className="block text-[10px] text-teal-400/80 mt-1 font-mono">
                Abnormal running speed
              </span>
            </button>
          </div>

          {/* Custom Event Builder */}
          <div className="pt-3 border-t border-cmd-border/60 space-y-3">
            <span className="text-xs font-mono font-bold text-cmd-muted uppercase block">
              Custom Event Payload Injection
            </span>
            <div className="grid grid-cols-3 gap-2 font-mono">
              <input
                type="text"
                placeholder="Event Type"
                value={eventType}
                onChange={(e) => setEventType(e.target.value)}
                className="input-field text-xs py-1.5"
              />
              <input
                type="text"
                placeholder="Location"
                value={eventLocation}
                onChange={(e) => setEventLocation(e.target.value)}
                className="input-field text-xs py-1.5 font-sans"
              />
              <input
                type="text"
                placeholder="HH:MM:SS"
                value={eventTimestamp}
                onChange={(e) => setEventTimestamp(e.target.value)}
                className="input-field text-xs py-1.5"
              />
            </div>
            <button
              onClick={() => handleSimulateEvent()}
              className="btn-primary w-full text-xs py-2 flex items-center justify-center gap-1.5"
            >
              <Send className="w-3.5 h-3.5" />
              <span>Ingest Custom Event into std::deque</span>
            </button>
          </div>
        </div>

        {/* Clock Simulation & Scoring HUD */}
        <div className="space-y-6">
          {/* Clock Advancement Controller */}
          <div className="cmd-card space-y-4">
            <div className="flex items-center justify-between pb-3 border-b border-cmd-border/60">
              <h2 className="text-sm font-bold text-cmd-text flex items-center gap-2">
                <FastForward className="w-4 h-4 text-cmd-primaryLight" />
                Simulated Clock & Time Machine
              </h2>
              <span className="text-[10px] font-mono text-cmd-teal bg-cmd-teal/10 px-2 py-0.5 rounded border border-cmd-teal/30">
                Discrete Clock Engine
              </span>
            </div>

            <p className="text-xs text-cmd-muted">
              Advance the simulated system clock. The backend immediately evaluates verification timeouts (3 mins), check-in grace expiry (5 mins), and contact escalation windows (2 mins).
            </p>

            <div className="flex gap-2">
              <input
                type="text"
                placeholder="HH:MM:SS"
                value={simTime}
                onChange={(e) => setSimTime(e.target.value)}
                className="input-field flex-1 font-mono text-sm py-2"
              />
              <button
                onClick={() => handleAdvanceClock(simTime)}
                className="btn-primary text-xs shrink-0 py-2 px-4"
              >
                Advance Clock
              </button>
            </div>

            {/* Quick Step Buttons */}
            <div className="space-y-1.5">
              <span className="text-[11px] font-mono text-cmd-muted block">
                Quick Test Timelines:
              </span>
              <div className="flex flex-wrap gap-2 text-xs font-mono">
                <button
                  onClick={() => handleAdvanceClock('18:03:00')}
                  className="px-2.5 py-1.5 bg-cmd-elevated hover:bg-cmd-card border border-cmd-border/60 hover:border-cmd-primary/60 text-cmd-text rounded-lg transition-all"
                >
                  18:03:00 (Near verification)
                </button>
                <button
                  onClick={() => handleAdvanceClock('18:08:00')}
                  className="px-2.5 py-1.5 bg-cmd-elevated hover:bg-cmd-card border border-cmd-border/60 hover:border-cmd-warning/60 text-amber-300 rounded-lg transition-all"
                >
                  18:08:00 (+5m Check-in Grace)
                </button>
                <button
                  onClick={() => handleAdvanceClock('18:15:00')}
                  className="px-2.5 py-1.5 bg-cmd-elevated hover:bg-cmd-card border border-cmd-border/60 hover:border-cmd-critical/60 text-rose-300 rounded-lg transition-all"
                >
                  18:15:00 (+12m Pruning)
                </button>
              </div>
            </div>
          </div>

          {/* Mathematical Scoring Diagnostics HUD */}
          <div className="cmd-card p-4 space-y-3">
            <div className="flex items-center justify-between pb-2 border-b border-cmd-border/60">
              <h3 className="text-xs font-mono font-bold uppercase text-cmd-muted">
                Real-Time Risk Scoring Formula
              </h3>
              <span className="text-[10px] font-mono text-cmd-teal">Bounded [0, 100]</span>
            </div>

            <div className="p-2.5 rounded bg-cmd-elevated/70 border border-cmd-border/40 font-mono text-[11px] text-cmd-text text-center">
              Risk = min(100, BaseSeveritySum × Multiplier + PatternBonus)
            </div>

            <div className="space-y-2 text-xs font-mono">
              <div className="flex justify-between items-center text-cmd-muted">
                <span>Base Severity Sum:</span>
                <span className="font-bold text-cmd-text">{status?.baseScore ?? 0}</span>
              </div>
              <div className="flex justify-between items-center text-cmd-muted">
                <span>Frequency Multiplier Bonus:</span>
                <span className="font-bold text-cmd-warning">+{status?.frequencyBonus ?? 0}</span>
              </div>
              <div className="flex justify-between items-center text-cmd-muted">
                <span>Recognized Pattern Bonus:</span>
                <span className="font-bold text-rose-400">+{status?.patternBonus ?? 0}</span>
              </div>
              <div className="flex justify-between items-center pt-2 border-t border-cmd-border/60 text-sm font-bold">
                <span className="text-cmd-primaryLight">Composite System Risk:</span>
                <span className={`px-2 py-0.5 rounded text-xs ${
                  (status?.riskScore ?? 0) >= 70
                    ? 'bg-cmd-critical/20 text-rose-300 border border-cmd-critical/40'
                    : (status?.riskScore ?? 0) >= 40
                    ? 'bg-cmd-warning/20 text-amber-300 border border-cmd-warning/40'
                    : 'bg-cmd-teal/20 text-teal-300 border border-cmd-teal/40'
                }`}>
                  {status?.riskScore ?? 0} / 100 ({status?.riskLevel || 'Normal'})
                </span>
              </div>
            </div>
          </div>
        </div>
      </div>

      {/* GUIDED COLLEGE PRESENTATION SCENARIO LAUNCHER */}
      <div className="cmd-card p-5 border border-cmd-primary/30 relative overflow-hidden bg-gradient-to-r from-cmd-card via-cmd-elevated to-cmd-card">
        <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
          <div className="space-y-1">
            <div className="flex items-center gap-2">
              <PlayCircle className="w-5 h-5 text-cmd-primaryLight animate-pulse" />
              <h3 className="font-bold text-cmd-text text-sm">
                One-Click Viva & Demonstration Scenarios
              </h3>
            </div>
            <p className="text-xs text-cmd-muted">
              Instantly execute pre-orchestrated test suites showing C++ sliding window pruning, automated verification trigger, and priority queue dispatch.
            </p>
          </div>

          <div className="flex flex-wrap gap-3">
            <button
              onClick={() => runDemonstrationScenario('VERIFICATION_ESCALATION')}
              disabled={scenarioRunning}
              className="btn-primary text-xs py-2 px-3.5 flex items-center gap-1.5"
            >
              <Flame className="w-3.5 h-3.5" />
              <span>Run Scenario: Deviation $\to$ Fall $\to$ Escalation</span>
            </button>

            <button
              onClick={() => runDemonstrationScenario('DIRECT_SOS')}
              disabled={scenarioRunning}
              className="btn-danger text-xs py-2 px-3.5 flex items-center gap-1.5"
            >
              <ShieldAlert className="w-3.5 h-3.5" />
              <span>Run Scenario: Direct SOS Triage</span>
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}
