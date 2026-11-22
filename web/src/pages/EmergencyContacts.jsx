import React, { useState, useEffect } from 'react';
import { Users, UserPlus, Phone, ShieldCheck, CheckCircle2, AlertTriangle, Shield, Radio, ShieldAlert } from 'lucide-react';
import { api } from '../services/api';

export default function EmergencyContacts({ onRefresh }) {
  const [contacts, setContacts] = useState([]);
  const [loading, setLoading] = useState(true);
  const [feedback, setFeedback] = useState(null);

  // Form State
  const [name, setName] = useState('');
  const [phone, setPhone] = useState('');
  const [relation, setRelation] = useState('');
  const [priority, setPriority] = useState(1);
  const [customId, setCustomId] = useState('');

  const fetchContacts = async () => {
    try {
      setLoading(true);
      const res = await api.getContacts();
      setContacts(res.contacts || []);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchContacts();
  }, []);

  const handleAddContact = async (e) => {
    e.preventDefault();
    if (!name || !phone || !relation) {
      setFeedback({ type: 'error', msg: 'Please fill in all contact details.' });
      return;
    }

    const id = customId ? parseInt(customId, 10) : Math.floor(100 + Math.random() * 900);

    try {
      const res = await api.registerContact({
        id,
        name,
        phone,
        relationship: relation,
        priority: parseInt(priority, 10)
      });
      if (res.success) {
        setFeedback({ type: 'success', msg: `Emergency Contact "${name}" registered into C++ Directory.` });
        setName('');
        setPhone('');
        setRelation('');
        setCustomId('');
        fetchContacts();
        onRefresh();
      }
    } catch (err) {
      setFeedback({ type: 'error', msg: err.message });
    }
  };

  const tier1 = contacts.filter((c) => c.priority === 1);
  const tier2 = contacts.filter((c) => c.priority === 2);
  const tier3 = contacts.filter((c) => c.priority === 3);

  return (
    <div className="space-y-6">
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 pb-2 border-b border-[rgba(148,163,184,0.1)]">
        <div>
          <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-wider text-violet-400 mb-1">
            <Users className="w-3.5 h-3.5" />
            <span>Multi-Tier Response Protocol</span>
          </div>
          <h1 className="page-title">Emergency Contacts Directory</h1>
          <p className="page-subtitle">
            3-Tier Escalation Hierarchy. SafeSignal alerts Tier 1 contacts immediately at $T=0$, escalates to Tier 2 after 2 minutes, and alerts Tier 3 (Campus Security) after 4 minutes.
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

      {/* THREE PRIORITY TIERS SECTION */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* TIER 1: PRIMARY */}
        <div className="cmd-card border-l-4 border-l-emerald-500 space-y-4">
          <div className="flex items-center justify-between border-b border-[rgba(148,163,184,0.1)] pb-3">
            <div>
              <span className="cmd-badge bg-emerald-500/15 text-emerald-300 border-emerald-500/30">
                PRIORITY 1 (PRIMARY)
              </span>
              <h3 className="text-sm font-bold text-white mt-1">Immediate First Responders</h3>
            </div>
            <span className="text-xs font-mono font-bold text-slate-400">{tier1.length} Contacts</span>
          </div>
          <p className="text-xs text-slate-400">
            Alerted immediately at $T=0$ when an emergency incident or SOS is triggered (e.g. Family, Roommates).
          </p>

          <div className="space-y-2">
            {tier1.length === 0 ? (
              <p className="text-xs italic text-slate-500 py-4 text-center">No Priority 1 contacts registered.</p>
            ) : (
              tier1.map((c) => (
                <div key={c.id} className="cmd-card-elevated space-y-1">
                  <div className="flex items-center justify-between">
                    <span className="font-bold text-white text-sm">{c.name}</span>
                    <span className="text-[10px] font-mono text-emerald-400 font-semibold">TIER 1</span>
                  </div>
                  <div className="flex items-center justify-between text-xs text-slate-400 font-mono">
                    <span>{c.relationship}</span>
                    <span className="flex items-center gap-1 text-slate-300">
                      <Phone className="w-3 h-3 text-slate-500" />
                      {c.phone}
                    </span>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>

        {/* TIER 2: SECONDARY */}
        <div className="cmd-card border-l-4 border-l-violet-500 space-y-4">
          <div className="flex items-center justify-between border-b border-[rgba(148,163,184,0.1)] pb-3">
            <div>
              <span className="cmd-badge bg-violet-500/15 text-violet-300 border-violet-500/30">
                PRIORITY 2 (SECONDARY)
              </span>
              <h3 className="text-sm font-bold text-white mt-1">First Escalation Tier</h3>
            </div>
            <span className="text-xs font-mono font-bold text-slate-400">{tier2.length} Contacts</span>
          </div>
          <p className="text-xs text-slate-400">
            Alerted after 2 minutes if Priority 1 contacts have not acknowledged the distress signal.
          </p>

          <div className="space-y-2">
            {tier2.length === 0 ? (
              <p className="text-xs italic text-slate-500 py-4 text-center">No Priority 2 contacts registered.</p>
            ) : (
              tier2.map((c) => (
                <div key={c.id} className="cmd-card-elevated space-y-1">
                  <div className="flex items-center justify-between">
                    <span className="font-bold text-white text-sm">{c.name}</span>
                    <span className="text-[10px] font-mono text-violet-400 font-semibold">TIER 2</span>
                  </div>
                  <div className="flex items-center justify-between text-xs text-slate-400 font-mono">
                    <span>{c.relationship}</span>
                    <span className="flex items-center gap-1 text-slate-300">
                      <Phone className="w-3 h-3 text-slate-500" />
                      {c.phone}
                    </span>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>

        {/* TIER 3: CAMPUS SECURITY */}
        <div className="cmd-card border-l-4 border-l-rose-500 space-y-4">
          <div className="flex items-center justify-between border-b border-[rgba(148,163,184,0.1)] pb-3">
            <div>
              <span className="cmd-badge bg-rose-500/15 text-rose-300 border-rose-500/30">
                PRIORITY 3 (SECURITY)
              </span>
              <h3 className="text-sm font-bold text-white mt-1">Campus Security / Police</h3>
            </div>
            <span className="text-xs font-mono font-bold text-slate-400">{tier3.length} Contacts</span>
          </div>
          <p className="text-xs text-slate-400">
            Final escalation tier: Campus Police, Resident Warden, or Emergency Desk if Tier 2 times out.
          </p>

          <div className="space-y-2">
            {tier3.length === 0 ? (
              <p className="text-xs italic text-slate-500 py-4 text-center">No Priority 3 contacts registered.</p>
            ) : (
              tier3.map((c) => (
                <div key={c.id} className="cmd-card-elevated space-y-1">
                  <div className="flex items-center justify-between">
                    <span className="font-bold text-white text-sm">{c.name}</span>
                    <span className="text-[10px] font-mono text-rose-400 font-semibold">TIER 3</span>
                  </div>
                  <div className="flex items-center justify-between text-xs text-slate-400 font-mono">
                    <span>{c.relationship}</span>
                    <span className="flex items-center gap-1 text-slate-300">
                      <Phone className="w-3 h-3 text-slate-500" />
                      {c.phone}
                    </span>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>
      </div>

      {/* REGISTER NEW CONTACT CARD */}
      <div className="cmd-card space-y-4">
        <div className="flex items-center gap-2 border-b border-[rgba(148,163,184,0.12)] pb-4">
          <UserPlus className="w-5 h-5 text-violet-400" />
          <div>
            <h2 className="text-base font-bold text-white">Register New Emergency Contact</h2>
            <p className="text-xs text-slate-400">Add trusted contacts into the C++ `std::vector&lt;Contact&gt;` directory</p>
          </div>
        </div>

        <form onSubmit={handleAddContact} className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5">Full Name</label>
            <input
              type="text"
              placeholder="e.g. Sarah Connor"
              value={name}
              onChange={(e) => setName(e.target.value)}
              className="input-field"
              required
            />
          </div>

          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5">Phone Number</label>
            <input
              type="text"
              placeholder="e.g. 555-0199"
              value={phone}
              onChange={(e) => setPhone(e.target.value)}
              className="input-field font-mono"
              required
            />
          </div>

          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5">Relationship</label>
            <input
              type="text"
              placeholder="e.g. Mother, Roommate, Campus Police"
              value={relation}
              onChange={(e) => setRelation(e.target.value)}
              className="input-field"
              required
            />
          </div>

          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5">Priority Tier</label>
            <select
              value={priority}
              onChange={(e) => setPriority(parseInt(e.target.value, 10))}
              className="input-field bg-[#0D1220]"
            >
              <option value={1}>Tier 1 — Primary (Notified 1st)</option>
              <option value={2}>Tier 2 — Secondary (Notified 2nd)</option>
              <option value={3}>Tier 3 — Emergency Security (Notified 3rd)</option>
            </select>
          </div>

          <div className="sm:col-span-2 lg:col-span-3">
            <label className="block text-xs font-semibold text-slate-300 mb-1.5">Contact ID (Optional)</label>
            <input
              type="number"
              placeholder="Auto-generated unique numeric ID if empty"
              value={customId}
              onChange={(e) => setCustomId(e.target.value)}
              className="input-field font-mono"
            />
          </div>

          <div className="flex items-end">
            <button type="submit" className="btn-primary w-full text-xs py-2.5">
              <UserPlus className="w-4 h-4" />
              <span>Register Contact</span>
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
