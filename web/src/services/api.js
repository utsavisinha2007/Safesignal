const API_BASE = '/api';

async function request(endpoint, options = {}) {
  const url = `${API_BASE}${endpoint}`;
  const headers = {
    'Content-Type': 'application/json',
    ...(options.headers || {})
  };

  try {
    const res = await fetch(url, { ...options, headers });
    if (!res.ok) {
      const errData = await res.json().catch(() => ({}));
      throw new Error(errData.message || `HTTP ${res.status}: ${res.statusText}`);
    }
    return await res.json();
  } catch (err) {
    console.error(`API Error on ${url}:`, err);
    throw err;
  }
}

export const api = {
  // GET
  getHealth: () => request('/health'),
  getStatus: () => request('/status'),
  getJourney: () => request('/journey'),
  getContacts: () => request('/contacts'),
  getCheckins: () => request('/checkins'),
  getActiveIncidents: () => request('/incidents/active'),
  getEventsHistory: () => request('/history/events'),
  getAuditHistory: () => request('/history/audit'),
  getVerifications: () => request('/verifications'),

  // POST
  startJourney: (data) => request('/journey/start', { method: 'POST', body: JSON.stringify(data) }),
  updateLocation: (data) => request('/journey/location', { method: 'POST', body: JSON.stringify(data) }),
  endJourney: () => request('/journey/end', { method: 'POST' }),
  registerContact: (data) => request('/contacts', { method: 'POST', body: JSON.stringify(data) }),
  scheduleCheckin: (data) => request('/checkins/schedule', { method: 'POST', body: JSON.stringify(data) }),
  confirmCheckin: (data) => request('/checkins/confirm', { method: 'POST', body: JSON.stringify(data) }),
  confirmSafe: (requestID = -1) => request('/emergency/verify/confirm-safe', { method: 'POST', body: JSON.stringify({ requestID }) }),
  requestHelp: (requestID = -1) => request('/emergency/verify/request-help', { method: 'POST', body: JSON.stringify({ requestID }) }),
  triggerSOS: (data = {}) => request('/emergency/sos', { method: 'POST', body: JSON.stringify(data) }),
  acknowledgeIncident: (data) => request('/incidents/acknowledge', { method: 'POST', body: JSON.stringify(data) }),
  resolveIncident: (data) => request('/incidents/resolve', { method: 'POST', body: JSON.stringify(data) }),
  cancelIncident: (data) => request('/incidents/cancel', { method: 'POST', body: JSON.stringify(data) }),
  advanceTime: (currentTime) => request('/sim/time', { method: 'POST', body: JSON.stringify({ currentTime }) }),
  ingestSimEvent: (data) => request('/sim/event', { method: 'POST', body: JSON.stringify(data) })
};
