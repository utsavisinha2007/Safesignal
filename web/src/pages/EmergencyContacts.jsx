import React, { useState, useEffect } from 'react';
import { Users, UserPlus, Phone, Shield, CheckCircle2, AlertTriangle, AlertCircle } from 'lucide-react';
import { api } from '../services/api';

export default function EmergencyContacts({ onRefresh }) {
  const [contacts, setContacts] = useState([]);
  const [loading, setLoading] = useState(true);
  const [feedback, setFeedback] = useState(null);

  // Form state
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
        setFeedback({ type: 'success', msg: `Emergency Contact "${name}" registered successfully!` });
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
      <div>
        <h1 className="page-title">Emergency Contacts Directory</h1>
        <p className="page-subtitle">
          Organized by priority tiers. In an emergency, SafeSignal dispatches alerts progressively from Priority 1 to Priority 3.
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

      {/* TIER EXPLANATION CARDS */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* TIER 1 */}
        <div className="card border-l-4 border-l-emerald-500">
          <div className="flex items-center justify-between mb-2">
            <span className="badge bg-emerald-100 text-emerald-800">PRIORITY 1 (PRIMARY)</span>
            <span className="text-xs text-gray-500 font-bold">{tier1.length} Contact(s)</span>
          </div>
          <p className="text-xs text-gray-600 mb-4">
            Alerted immediately at $T=0$ when an emergency is dispatched (e.g. Family, Roommate).
          </p>
          <div className="space-y-2">
            {tier1.length === 0 ? (
              <p className="text-xs italic text-gray-400">No Priority 1 contacts registered.</p>
            ) : (
              tier1.map((c) => (
                <div key={c.id} className="p-3 bg-gray-50 rounded-xl border border-gray-100 text-xs">
                  <div className="font-bold text-gray-900">{c.name}</div>
                  <div className="text-gray-500 flex items-center justify-between mt-1">
                    <span>{c.relationship}</span>
                    <span className="font-mono text-gray-700">{c.phone}</span>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>

        {/* TIER 2 */}
        <div className="card border-l-4 border-l-violet-500">
          <div className="flex items-center justify-between mb-2">
            <span className="badge bg-violet-100 text-violet-800">PRIORITY 2 (SECONDARY)</span>
            <span className="text-xs text-gray-500 font-bold">{tier2.length} Contact(s)</span>
          </div>
          <p className="text-xs text-gray-600 mb-4">
            Escalated to after 2 minutes if Priority 1 contacts do not acknowledge the distress signal.
          </p>
          <div className="space-y-2">
            {tier2.length === 0 ? (
              <p className="text-xs italic text-gray-400">No Priority 2 contacts registered.</p>
            ) : (
              tier2.map((c) => (
                <div key={c.id} className="p-3 bg-gray-50 rounded-xl border border-gray-100 text-xs">
                  <div className="font-bold text-gray-900">{c.name}</div>
                  <div className="text-gray-500 flex items-center justify-between mt-1">
                    <span>{c.relationship}</span>
                    <span className="font-mono text-gray-700">{c.phone}</span>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>

        {/* TIER 3 */}
        <div className="card border-l-4 border-l-red-500">
          <div className="flex items-center justify-between mb-2">
            <span className="badge bg-red-100 text-red-800">PRIORITY 3 (SECURITY / 911)</span>
            <span className="text-xs text-gray-500 font-bold">{tier3.length} Contact(s)</span>
          </div>
          <p className="text-xs text-gray-600 mb-4">
            Final escalation tier: Campus Police, Dorm Warden, or Security Desk if earlier tiers time out.
          </p>
          <div className="space-y-2">
            {tier3.length === 0 ? (
              <p className="text-xs italic text-gray-400">No Priority 3 contacts registered.</p>
            ) : (
              tier3.map((c) => (
                <div key={c.id} className="p-3 bg-gray-50 rounded-xl border border-gray-100 text-xs">
                  <div className="font-bold text-gray-900">{c.name}</div>
                  <div className="text-gray-500 flex items-center justify-between mt-1">
                    <span>{c.relationship}</span>
                    <span className="font-mono text-gray-700">{c.phone}</span>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>
      </div>

      {/* REGISTER NEW CONTACT FORM */}
      <div className="card">
        <h2 className="text-lg font-bold text-gray-900 mb-4 flex items-center gap-2">
          <UserPlus className="w-5 h-5 text-violet-600" />
          Add a New Emergency Contact
        </h2>

        <form onSubmit={handleAddContact} className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div>
            <label className="block text-xs font-semibold text-gray-700 mb-1">Full Name</label>
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
            <label className="block text-xs font-semibold text-gray-700 mb-1">Phone Number</label>
            <input
              type="text"
              placeholder="e.g. 555-0199"
              value={phone}
              onChange={(e) => setPhone(e.target.value)}
              className="input-field"
              required
            />
          </div>

          <div>
            <label className="block text-xs font-semibold text-gray-700 mb-1">Relationship</label>
            <input
              type="text"
              placeholder="e.g. Mother, Roommate, Campus Security"
              value={relation}
              onChange={(e) => setRelation(e.target.value)}
              className="input-field"
              required
            />
          </div>

          <div>
            <label className="block text-xs font-semibold text-gray-700 mb-1">Priority Tier</label>
            <select
              value={priority}
              onChange={(e) => setPriority(parseInt(e.target.value, 10))}
              className="input-field bg-white"
            >
              <option value={1}>Tier 1 — Primary (Notified 1st)</option>
              <option value={2}>Tier 2 — Secondary (Notified 2nd)</option>
              <option value={3}>Tier 3 — Emergency / Campus Security (Notified 3rd)</option>
            </select>
          </div>

          <div>
            <label className="block text-xs font-semibold text-gray-700 mb-1">Contact ID (Optional)</label>
            <input
              type="number"
              placeholder="Auto-generated if empty"
              value={customId}
              onChange={(e) => setCustomId(e.target.value)}
              className="input-field"
            />
          </div>

          <div className="flex items-end">
            <button type="submit" className="btn-primary w-full">
              Save Emergency Contact
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
