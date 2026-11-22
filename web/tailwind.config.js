/** @type {import('tailwindcss').Config} */
export default {
  content: ["./index.html", "./src/**/*.{js,ts,jsx,tsx}"],
  theme: {
    extend: {
      colors: {
        navy: { 800: '#1e2a4a', 900: '#141d35', 950: '#0c1322' },
        brand: { 400: '#a78bfa', 500: '#8b5cf6', 600: '#7c3aed' }
      }
    }
  },
  plugins: []
}
