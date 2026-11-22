import React from 'react';

export default function StatusBadge({ status }) {
  const s = (status || '').toUpperCase();
  let color = 'bg-gray-100 text-gray-700 border-gray-200';

  if (s === 'PENDING' || s === 'AWAITING_ACKNOWLEDGEMENT') {
    color = 'bg-amber-50 text-amber-800 border-amber-200';
  } else if (s === 'COMPLETED' || s === 'CONFIRMED_SAFE' || s === 'RESOLVED') {
    color = 'bg-emerald-50 text-emerald-800 border-emerald-200';
  } else if (s === 'HELP_REQUESTED' || s === 'TIMED_OUT' || s === 'MISSED' || s === 'DISPATCHING') {
    color = 'bg-red-50 text-red-800 border-red-200';
  } else if (s === 'LATE_CONFIRMATION' || s === 'CANCELLED') {
    color = 'bg-purple-50 text-purple-800 border-purple-200';
  }

  return (
    <span className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-semibold border ${color}`}>
      {status}
    </span>
  );
}
