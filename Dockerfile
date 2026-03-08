# Atlas Engine — Headless Server Dockerfile
#
# Builds the AtlasServer binary in a multi-stage Docker image.
# The final image is a minimal Debian runtime with only the server binary.
#
# Usage:
#   docker build -t atlas-server .
#   docker run -p 7777:7777 atlas-server
#
# Build arguments:
#   BUILD_TYPE   Debug|Release|Development (default: Release)
#   JOBS         Parallel build jobs        (default: auto)

# ── Stage 1: Build ──────────────────────────────────────────────────
FROM debian:bookworm-slim AS builder

ARG BUILD_TYPE=Release
ARG JOBS=0

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        g++ \
        git \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

# Detect core count if JOBS=0
RUN if [ "$JOBS" = "0" ]; then JOBS=$(nproc); fi \
    && mkdir -p build \
    && cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DATLAS_EDITOR_TOOLS=OFF \
    && cmake --build build --target AtlasServer -j ${JOBS}

# ── Stage 2: Runtime ────────────────────────────────────────────────
FROM debian:bookworm-slim AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd --system atlas \
    && useradd --system --gid atlas --no-create-home atlas

WORKDIR /app

COPY --from=builder /src/build/server/AtlasServer /app/AtlasServer
COPY --from=builder /src/assets /app/assets

USER atlas

EXPOSE 7777/udp
EXPOSE 7778/tcp

ENTRYPOINT ["/app/AtlasServer"]
