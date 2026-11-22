import React from 'react';
import { NavLink } from 'react-router-dom';
import {
  ShieldAlert,
  Navigation,
  Users,
  Clock,
  AlertTriangle,
  History,
  Cpu,
  Radio,
  Activity,
  Terminal,
  ChevronLeft,
  ChevronRight,
  Database
} from 'lucide-react';

export default function Sidebar({
  activeVerification,
  activeIncidents,
  serverConnected,
  collapsed,
  setCollapsed,
  mobileOpen,
  setMobileOpen
}) {
  const navItems = [
    { to: '/', label: 'Command Centre', icon: Activity, exact: true },
    { to: '/journey', label: 'Journey Monitoring', icon: Navigation },
    { to: '/contacts', label: 'Emergency Contacts', icon: Users },
    { to: '/checkins', label: 'Safety Check-ins', icon: Clock },
    {
      to: '/emergency',
      label: 'Emergency Response',
      icon: AlertTriangle,
      badge: activeVerification || activeIncidents > 0 ? 'CRITICAL' : null,
      badgeColor: 'bg-rose-500/20 text-rose-300 border-rose-500/40'
    },
    { to: '/history', label: 'Activity History', icon: History },
    { to: '/demo', label: 'DSA Demonstration Lab', icon: Cpu }
  ];

  return (
    <>
      {/* Mobile Backdrop */}
      {mobileOpen && (
        <div
          onClick={() => setMobileOpen(false)}
          className="fixed inset-0 bg-black/70 backdrop-blur-sm z-40 lg:hidden"
        />
      )}

      <aside
        className={`fixed lg:static inset-y-0 left-0 z-50 flex flex-col bg-[#0D1220] border-r border-[rgba(148,163,184,0.12)] transition-all duration-300 ${
          collapsed ? 'w-20' : 'w-64'
        } ${mobileOpen ? 'translate-x-0' : '-translate-x-full lg:translate-x-0'}`}
      >
        {/* Brand Header */}
        <div className="p-4 border-b border-[rgba(148,163,184,0.12)] flex items-center justify-between">
          <div className="flex items-center gap-3 overflow-hidden">
            <div className="w-10 h-10 rounded-xl bg-gradient-to-tr from-violet-700 to-violet-500 flex items-center justify-center shrink-0 shadow-cmd-glow border border-violet-400/30">
              <Radio className="w-5 h-5 text-white animate-pulse" />
            </div>
            {!collapsed && (
              <div className="min-w-0">
                <div className="flex items-center gap-1.5">
                  <h1 className="text-base font-extrabold tracking-tight text-white truncate">
                    SafeSignal
                  </h1>
                  <span className="px-1.5 py-0.2 rounded text-[9px] font-bold bg-violet-500/20 text-violet-300 border border-violet-500/30">
                    C++17
                  </span>
                </div>
                <p className="text-[10px] text-slate-400 tracking-wider uppercase font-semibold">
                  OPS COMMAND
                </p>
              </div>
            )}
          </div>

          <button
            onClick={() => setCollapsed(!collapsed)}
            className="hidden lg:flex p-1.5 rounded-lg text-slate-400 hover:text-white hover:bg-slate-800/60 transition-colors"
            title={collapsed ? 'Expand sidebar' : 'Collapse sidebar'}
          >
            {collapsed ? <ChevronRight className="w-4 h-4" /> : <ChevronLeft className="w-4 h-4" />}
          </button>
        </div>

        {/* Navigation Items */}
        <nav className="flex-1 p-3 space-y-1.5 overflow-y-auto">
          {navItems.map((item) => {
            const Icon = item.icon;
            return (
              <NavLink
                key={item.to}
                to={item.to}
                end={item.exact}
                onClick={() => setMobileOpen(false)}
                className={({ isActive }) =>
                  `flex items-center gap-3 px-3 py-2.5 rounded-xl text-xs font-semibold tracking-wide transition-all ${
                    isActive
                      ? 'bg-violet-600/20 text-violet-200 border border-violet-500/30 shadow-[0_0_15px_-3px_rgba(139,92,246,0.25)]'
                      : 'text-slate-400 hover:text-slate-100 hover:bg-[#172033]/60'
                  } ${collapsed ? 'justify-center px-0' : ''}`
                }
                title={collapsed ? item.label : undefined}
              >
                <Icon className={`w-4 h-4 shrink-0 transition-transform ${collapsed ? 'w-5 h-5' : ''}`} />
                {!collapsed && (
                  <span className="flex-1 truncate">{item.label}</span>
                )}
                {!collapsed && item.badge && (
                  <span className={`px-2 py-0.5 rounded-full text-[9px] font-extrabold border ${item.badgeColor} animate-pulse`}>
                    {item.badge}
                  </span>
                )}
              </NavLink>
            );
          })}
        </nav>

        {/* System & Backend Telemetry Status Card (Bottom) */}
        {!collapsed ? (
          <div className="p-3 border-t border-[rgba(148,163,184,0.12)] space-y-2">
            <div className="p-3 rounded-xl bg-[#111827] border border-[rgba(148,163,184,0.12)] space-y-2">
              <div className="flex items-center justify-between text-[11px]">
                <span className="text-slate-400 font-medium flex items-center gap-1.5">
                  <Database className="w-3.5 h-3.5 text-violet-400" />
                  C++ Core Engine
                </span>
                <span className="font-mono text-[10px] text-slate-300">127.0.0.1:8080</span>
              </div>

              <div className="flex items-center justify-between text-[11px] pt-1.5 border-t border-slate-800/80">
                <span className="text-slate-400">Server Link:</span>
                <span className="flex items-center gap-1.5 font-bold">
                  <span
                    className={`w-2 h-2 rounded-full ${
                      serverConnected
                        ? 'bg-emerald-400 shadow-[0_0_8px_rgba(52,211,153,0.8)] animate-pulse'
                        : 'bg-rose-500'
                    }`}
                  />
                  <span className={serverConnected ? 'text-emerald-400' : 'text-rose-400'}>
                    {serverConnected ? 'ONLINE' : 'OFFLINE'}
                  </span>
                </span>
              </div>

              <div className="text-[10px] text-slate-500 font-mono pt-1">
                STL Deque • Priority Queue • Mutex
              </div>
            </div>
          </div>
        ) : (
          <div className="p-3 border-t border-[rgba(148,163,184,0.12)] flex justify-center">
            <span
              className={`w-2.5 h-2.5 rounded-full ${
                serverConnected
                  ? 'bg-emerald-400 shadow-[0_0_8px_rgba(52,211,153,0.8)] animate-pulse'
                  : 'bg-rose-500'
              }`}
              title={serverConnected ? 'C++ Server Online' : 'C++ Server Offline'}
            />
          </div>
        )}
      </aside>
    </>
  );
}
