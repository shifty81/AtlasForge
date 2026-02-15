# Atlas Engine — Overview

Atlas is a modular, data-driven game engine and simulation platform designed to support:

- **Standalone Editor (Dev Client)** — Feature-rich, Blender-style authoring environment
- **Runtime Client** — Lean player runtime
- **Dedicated Server** — Headless authoritative simulation
- **Peer-to-Peer Networking** — Lockstep and rollback capable
- **Graph-based Gameplay VM** — Deterministic, hot-reloadable gameplay logic
- **Massive Procedural Worlds** — Planet-scale to galaxy-scale generation
- **Hot-Reloadable Assets** — Zero-rebuild iteration
- **AI-Assisted Generation** — In-editor content and mechanic generation

## Supported Game Types

Atlas is content-agnostic. All gameplay is implemented as data + graphs, not hardcoded logic. Supported game styles include:

- Space strategy (top-down / isometric overview)
- First-person interiors
- Planetary exploration
- Voxel worlds
- Top-down / adventure-style cameras
- Side-scrollers
- Large-scale simulations

## Core Philosophy

1. **Engine Is Not a Game** — The engine contains no gameplay logic
2. **Everything Is Data** — All behavior is authored as assets and executable graphs
3. **One Engine, Three Roles** — Editor, Client, and Server share the same engine core
4. **Editor Is Primary** — The editor is not a separate tool; it is a first-class engine runtime
5. **Determinism First** — All simulation is deterministic for networking and replay
