import React from 'react';

export default function StatusBadge({ status }) {
  const s = (status || '').toUpperCase();
  let color = 'bg-slate-800/60 text-slate-300 border-slate-700/60';
  let dotColor = 'bg-slate-400';

  if (s === 'PENDING' || s === 'AWAITING_ACKNOWLEDGEMENT') {
    color = 'bg-amber-500/10 text-amber-300 border-amber-500/30 shadow-[0_0_12px_-3px_rgba(251,191,36,0.2)]';
    dotColor = 'bg-amber-400 animate-pulse';
  } else if (s === 'COMPLETED' || s === 'CONFIRMED_SAFE' || s === 'RESOLVED') {
    color = 'bg-emerald-500/10 text-emerald-300 border-emerald-500/30 shadow-[0_0_12px_-3px_rgba(52,211,153,0.2)]';
    dotColor = 'bg-emerald-400';
  } else if (s === 'HELP_REQUESTED' || s === 'TIMED_OUT' || s === 'MISSED' || s === 'DISPATCHING') {
    color = 'bg-rose-500/15 text-rose-300 border-rose-500/40 shadow-[0_0_14px_-3px_rgba(251,113,133,0.3)]';
    dotColor = 'bg-rose-400 animate-ping';
  } else if (s === 'LATE_CONFIRMATION' || s === 'CANCELLED') {
    color = 'bg-purple-500/15 text-purple-300 border-purple-500/30';
    dotColor = 'bg-purple-400';
  }

  return (
    <span className={`inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full text-[11px] font-semibold tracking-wide border ${color}`}>
      <span className={`w-1.5 h-1.5 rounded-full ${dotColor}`} />
      <span>{status || 'UNKNOWN'}</span>
    </span>
  );
}
