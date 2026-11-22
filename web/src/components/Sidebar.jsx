import React from 'react';
import { NavLink } from 'react-router-dom';
import {
  Shield,
  MapPin,
  Users,
  Clock,
  AlertTriangle,
  History,
  Cpu,
  Radio
} from 'lucide-react';

export default function Sidebar({ activeVerification, activeIncidents }) {
  const navItems = [
    { to: '/', label: 'Dashboard', icon: Shield },
    { to: '/journey', label: 'Journey Manager', icon: MapPin },
    { to: '/contacts', label: 'Emergency Contacts', icon: Users },
    { to: '/checkins', label: 'Safety Check-ins', icon: Clock },
    {
      to: '/emergency',
      label: 'Emergency Center',
      icon: AlertTriangle,
      badge: activeVerification || activeIncidents > 0 ? 'ALERT' : null,
      badgeColor: 'bg-red-500'
    },
    { to: '/history', label: 'Activity History', icon: History },
    { to: '/demo', label: 'Demonstration Lab', icon: Cpu }
  ];

  return (
    <aside className="w-64 bg-navy-950 text-white flex flex-col shrink-0 min-h-screen border-r border-navy-900">
      {/* Brand Header */}
      <div className="p-6 border-b border-navy-900">
        <div className="flex items-center gap-3">
          <div className="w-10 h-10 rounded-xl bg-violet-600 flex items-center justify-center shadow-lg shadow-violet-600/30">
            <Radio className="w-6 h-6 text-white animate-pulse" />
          </div>
          <div>
            <h1 className="text-xl font-bold tracking-tight text-white flex items-center gap-1.5">
              SafeSignal
            </h1>
            <p className="text-xs text-violet-300 font-medium">Safety Engine Connected</p>
          </div>
        </div>
        <p className="text-[11px] text-gray-400 mt-3 leading-tight italic">
          "Your Safety. Your Signal. Your Control."
        </p>
      </div>

      {/* Navigation Links */}
      <nav className="flex-1 p-4 space-y-1.5">
        {navItems.map((item) => {
          const Icon = item.icon;
          return (
            <NavLink
              key={item.to}
              to={item.to}
              className={({ isActive }) =>
                `flex items-center justify-between px-4 py-3 rounded-xl text-sm font-medium transition-all ${
                  isActive
                    ? 'bg-violet-600/20 text-violet-300 border border-violet-500/30 shadow-inner'
                    : 'text-gray-300 hover:bg-navy-900 hover:text-white'
                }`
              }
            >
              <div className="flex items-center gap-3">
                <Icon className="w-5 h-5 shrink-0" />
                <span>{item.label}</span>
              </div>
              {item.badge && (
                <span className={`px-2 py-0.5 rounded-full text-[10px] font-bold text-white ${item.badgeColor} animate-pulse`}>
                  {item.badge}
                </span>
              )}
            </NavLink>
          );
        })}
      </nav>

      {/* C++ Backend Tech Badge */}
      <div className="p-4 m-4 rounded-xl bg-navy-900/80 border border-navy-800 text-xs">
        <div className="flex items-center gap-2 text-violet-400 font-semibold mb-1">
          <Cpu className="w-3.5 h-3.5" />
          <span>C++ DSA Engine</span>
        </div>
        <p className="text-[11px] text-gray-400 leading-normal">
          Powered by C++17 sliding-window <code className="text-violet-300">std::deque</code> and <code className="text-violet-300">std::priority_queue</code> triage.
        </p>
      </div>
    </aside>
  );
}
