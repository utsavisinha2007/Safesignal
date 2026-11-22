/** @type {import('tailwindcss').Config} */
export default {
  content: ["./index.html", "./src/**/*.{js,ts,jsx,tsx}"],
  theme: {
    extend: {
      colors: {
        cmd: {
          bg: '#080B14',
          sub: '#0D1220',
          card: '#111827',
          elevated: '#172033',
          border: 'rgba(148, 163, 184, 0.14)',
          borderHover: 'rgba(167, 139, 250, 0.35)',
          text: '#F8FAFC',
          muted: '#94A3B8'
        },
        violet: {
          400: '#A78BFA',
          500: '#8B5CF6',
          600: '#7C3AED',
          700: '#6D28D9',
          900: '#4C1D95',
          950: '#2E1065'
        },
        teal: {
          400: '#2DD4BF',
          500: '#14B8A6'
        },
        emerald: {
          400: '#34D399',
          500: '#10B981'
        },
        amber: {
          400: '#FBBF24',
          500: '#F59E0B'
        },
        rose: {
          400: '#FB7185',
          500: '#F43F5E',
          600: '#E11D48'
        }
      },
      fontFamily: {
        sans: ['Inter', 'system-ui', '-apple-system', 'sans-serif'],
        mono: ['JetBrains Mono', 'Fira Code', 'monospace']
      },
      boxShadow: {
        'cmd-card': '0 4px 20px -2px rgba(0, 0, 0, 0.4), 0 2px 6px -1px rgba(0, 0, 0, 0.2)',
        'cmd-glow': '0 0 25px -5px rgba(139, 92, 246, 0.25)',
        'danger-glow': '0 0 30px -5px rgba(244, 63, 94, 0.4)'
      }
    }
  },
  plugins: []
}
