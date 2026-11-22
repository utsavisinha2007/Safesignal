import React from 'react';
import { AlertCircle, ShieldAlert } from 'lucide-react';

export default function DisclosureBanner() {
  return (
    <div className="bg-[#0D1220] border-b border-[rgba(148,163,184,0.12)] px-4 py-2 text-xs text-amber-300/90 flex items-center justify-center gap-2.5">
      <div className="flex items-center gap-1.5 font-medium">
        <span className="w-1.5 h-1.5 rounded-full bg-amber-400 animate-pulse" />
        <span className="font-semibold text-amber-200">ACADEMIC SIMULATION:</span>
      </div>
      <span className="text-slate-400">
        GPS waypoints, notification broadcasts, and emergency dispatches are simulated within local C++ memory. Does not route calls to physical 911 or live emergency services.
      </span>
    </div>
  );
}
