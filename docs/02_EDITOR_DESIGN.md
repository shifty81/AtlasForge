# Atlas Engine — Editor Design (Dev Client)

The Editor is Blender-esque, dockable, and mode-agnostic.
It is a standalone application — not a mode of the client.

## Editor Principles

- Dockable panels (like Blender)
- Context-sensitive tools
- Data-driven editors
- Non-modal workflows
- Everything live-editable
- Undo/redo is core, not optional

## Editor Capabilities

- Scene editing
- Procedural world generation
- Graph-based mechanics authoring
- Live simulation
- Network testing (P2P, dedicated server)
- AI-assisted generation
- Hot reload everything

## Editor Modes (Not Separate Apps)

- World Editor
- Graph Editor
- Asset Browser
- Network Debugger
- Simulation View
- Strategy View
- FPS View

## Default Layout

```
┌────────────────────────────────────┐
│ Top Bar (Mode / Play / Net)        │
├──────────────┬─────────────────────┤
│              │                     │
│  Tool Shelf  │   3D / Scene View   │
│              │                     │
├──────────────┼─────────────────────┤
│ Properties   │ Outliner / ECS Tree │
├──────────────┴─────────────────────┤
│ Console / Timeline / Node Graph    │
└────────────────────────────────────┘
```

Every panel is an Atlas panel.
Everything can be rearranged and saved.

## Core Panels

- **Outliner** — Assets / Worlds / Entities
- **Properties** — Data only, schema-driven
- **Graph Editor** — Node-based gameplay logic
- **Viewport** — Render + simulation
- **Net Debug** — Network topology and stats
- **Profiler** — Graph VM, ECS, network timing
- **Console** — Command-line automation

## Play-In-Editor Modes

- Simulate (no player)
- Possess Entity
- Client-Server Loopback
- P2P Test Session
