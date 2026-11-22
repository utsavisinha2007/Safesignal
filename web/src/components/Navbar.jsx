import React from 'react';
import { RefreshCw, Activity, ShieldCheck, AlertOctagon } from 'lucide-react';

export default function Navbar({ status, onRefresh, refreshing, serverConnected }) {
  const riskScore = status?.riskScore ?? 0;
  const isHighRisk = status?.isRiskHigh ?? false;
  const hasVerification = status?.hasActiveVerification ?? false;

  return (
    <header className="h-16 bg-white border-b border-gray-200 px-6 flex items-center justify-between shrink-0">
      <div className="flex items-center gap-4">
        <div className="flex items-center gap-2">
          <span className={`w-2.5 h-2.5 rounded-full ${serverConnected ? 'bg-emerald-500 animate-pulse' : 'bg-red-500'}`} />
          <span className="text-xs font-medium text-gray-600">
            {serverConnected ? 'C++ Server Online (127.0.0.1:8080)' : 'Connecting to C++ Server...'}
          </span>
        </div>
      </div>

      <div className="flex items-center gap-4">
        {/* Risk Pill */}
        <div className={`flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold border ${
          hasVerification || isHighRisk
            ? 'bg-red-50 text-red-700 border-red-200 animate-pulse'
            : riskScore >= 20
            ? 'bg-amber-50 text-amber-700 border-amber-200'
            : 'bg-emerald-50 text-emerald-700 border-emerald-200'
        }`}>
          {hasVerification || isHighRisk ? (
            <AlertOctagon className="w-3.5 h-3.5 text-red-600" />
          ) : (
            <ShieldCheck className="w-3.5 h-3.5 text-emerald-600" />
          )}
          <span>
            Risk Score: <strong>{riskScore}/100</strong> ({status?.riskLevel || 'Normal'})
          </span>
        </div>

        {/* Refresh Button */}
        <button
          onClick={onRefresh}
          disabled={refreshing}
          className="p-2 text-gray-500 hover:text-violet-600 hover:bg-gray-100 rounded-lg transition-colors"
          title="Refresh Backend State"
        >
          <RefreshCw className={`w-4 h-4 ${refreshing ? 'animate-spin text-violet-600' : ''}`} />
        </button>
      </div>
    </header>
  );
}
