import React from 'react';
import { AlertCircle } from 'lucide-react';

export default function DisclosureBanner() {
  return (
    <div className="bg-amber-50 border-b border-amber-200 px-4 py-2 text-xs text-amber-800 flex items-center justify-center gap-2">
      <AlertCircle className="w-4 h-4 text-amber-600 shrink-0" />
      <span>
        <strong>Academic Demonstration:</strong> GPS updates, contact notifications, and emergency dispatches are simulated for safety education and testing. This software does not contact actual 911/emergency services.
      </span>
    </div>
  );
}
