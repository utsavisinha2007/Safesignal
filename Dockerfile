# ==============================================================================
# SafeSignal C++ REST API Server — Production Multi-Stage Dockerfile
# Optimized for Linux Containers & Render Web Service Deployment
# ==============================================================================

# --- Stage 1: Build Environment ---
FROM gcc:12-bookworm AS builder

WORKDIR /build

# Copy source trees and headers
COPY include/ include/
COPY src/ src/
COPY server/ server/

# Compile SafeSignal C++17 REST API Server with POSIX pthread support
RUN g++ -std=c++17 -O2 \
    server/server_main.cpp \
    server/api_handlers.cpp \
    src/CheckInManager.cpp \
    src/Contact.cpp \
    src/Event.cpp \
    src/Incident.cpp \
    src/JourneyManager.cpp \
    src/ResponseManager.cpp \
    src/RiskAnalyzer.cpp \
    src/SafetySystem.cpp \
    src/VerificationManager.cpp \
    -Iinclude \
    -Iserver \
    -lpthread \
    -o SafeSignalServer

# --- Stage 2: Minimal Runtime Environment ---
FROM debian:bookworm-slim AS runner

WORKDIR /app

# Install standard C++ runtime libraries and curl for cloud health checking
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        libstdc++6 \
        curl && \
    rm -rf /var/lib/apt/lists/*

# Security: Create non-root system user and group
RUN groupadd -r safesignal && useradd -r -g safesignal -s /bin/false safesignal

# Copy compiled binary from builder stage
COPY --from=builder /build/SafeSignalServer /app/SafeSignalServer

# Set execution permissions and assign ownership
RUN chmod +x /app/SafeSignalServer && \
    chown -R safesignal:safesignal /app

# Switch to unprivileged user
USER safesignal

# Default environment variables (Render automatically injects PORT at runtime)
ENV HOST=0.0.0.0
ENV PORT=8080
ENV ALLOWED_ORIGIN=*

# Expose default listening port
EXPOSE 8080

# Cloud container healthcheck
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD curl -f http://127.0.0.1:${PORT}/health || exit 1

# Launch the SafeSignal C++ REST API Server
CMD ["/app/SafeSignalServer"]
