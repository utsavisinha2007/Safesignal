// Resolve the API base URL from the Vite environment variable.
// VITE_API_BASE_URL should be set to the full Render root URL, e.g.:
//   https://safesignal-rc3b.onrender.com
// The /api prefix is appended here automatically.
// In local dev (no env var), falls back to the Vite proxy path '/api'.

function buildApiBase() {
  const raw = (import.meta.env.VITE_API_BASE_URL || '').trim().replace(/\/+$/, '');

  // No env var set → local dev mode, use Vite proxy ('/api' → localhost:8080/api)
  if (!raw) return '/api';

  // Guard: if the value is missing the protocol, it is a misconfiguration.
  // Treat it as a relative path fragment to prevent silent wrong-host fetches.
  if (!raw.startsWith('http://') && !raw.startsWith('https://')) {
    console.warn(
      '[SafeSignal] VITE_API_BASE_URL is missing the protocol (https://). ' +
      'Set it to the full Render URL, e.g. https://safesignal-rc3b.onrender.com'
    );
    // Best-effort: prepend https://
    return `https://${raw}/api`;
  }

  // If the caller already appended /api, keep it; otherwise append it.
  return raw.endsWith('/api') ? raw : `${raw}/api`;
}

const API_BASE = buildApiBase();

// Request timeout: 10 s for normal calls.
// This prevents the browser from hanging indefinitely during a Render cold start
// and allows the 3.5-second poller in App.jsx to retry on the next tick.
const REQUEST_TIMEOUT_MS = 10000;

async function request(endpoint, options = {}) {
  const cleanEndpoint = endpoint.startsWith('/') ? endpoint : `/${endpoint}`;
  const url = `${API_BASE}${cleanEndpoint}`;

  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), REQUEST_TIMEOUT_MS);

  const headers = {
    'Content-Type': 'application/json',
    ...(options.headers || {})
  };

  try {
    const res = await fetch(url, { ...options, headers, signal: controller.signal });
    clearTimeout(timer);
    if (!res.ok) {
      const errData = await res.json().catch(() => ({}));
      throw new Error(errData.message || `HTTP ${res.status}: ${res.statusText}`);
    }
    return await res.json();
  } catch (err) {
    clearTimeout(timer);
    if (err.name === 'AbortError') {
      throw new Error(`Request to ${url} timed out after ${REQUEST_TIMEOUT_MS / 1000}s`);
    }
    console.error(`API Error on ${url}:`, err);
    throw err;
  }
}

export const api = {
  // Expose the resolved base so the UI can display it in the offline banner
  baseUrl: API_BASE,

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
