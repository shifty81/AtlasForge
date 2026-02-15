# Atlas Engine — Architecture

This document defines the **baseline architecture** for the Atlas Engine, establishing the
structural contracts and module boundaries that all development must follow.

## Overview

Atlas is a **deterministic, tool-first game engine** extracted from the EVEOFFLINE game project.
It separates reusable engine systems from game-specific content, enabling multiple projects to
share the same core technology.

```
Atlas/
├── engine/           # Atlas Engine (game-agnostic static library)
│   ├── core/         # Engine lifecycle, logging, capabilities
│   ├── ecs/          # Entity-Component-System framework
│   ├── graphvm/      # Deterministic bytecode graph VM, serialization, caching
│   ├── assets/       # Asset registry, binary format, hot reload
│   ├── net/          # Networking (client-server, P2P, lockstep/rollback)
│   ├── sim/          # Tick scheduler, deterministic simulation
│   ├── world/        # Procedural world generation, WorldGraph, heightfield meshing
│   ├── tile/         # TileGraph — 2D tile-based procedural generation
│   ├── strategygraph/# Strategy decision graphs (influence, threat, scoring)
│   ├── conversation/ # Dialogue + memory graphs (ConversationGraph)
│   ├── ai/           # AI signals, memory, relationships, BehaviorGraph
│   ├── character/    # CharacterGraph — modular character generation
│   ├── animation/    # AnimationGraph — animation state machines + modifiers
│   ├── weapon/       # WeaponGraph — weapon construction + wear
│   ├── sound/        # SoundGraph — procedural audio generation
│   ├── ui/           # UIGraph — UI composition system
│   ├── flow/         # GameFlowGraph — game flow state machine
│   ├── schema/       # Schema validation system
│   ├── camera/       # World modes and camera projection policies
│   ├── project/      # Project loading and validation (.atlas files)
│   ├── command/      # Undo/redo command system
│   ├── interaction/  # Unified intent/utterance system (voice, AI, console)
│   ├── voice/        # Voice command registry and matching
│   ├── plugin/       # Plugin validation, registry, and sandboxing
│   └── rules/        # Server rules system (live parameter tuning)
│
├── editor/           # Atlas Editor (authoring tool)
│   ├── panels/       # Inspector panels (ECS, console, network, project picker,
│   │                 #   world graph editor, voice commands, interaction debugger)
│   ├── ui/           # Dock layout, panel framework
│   ├── tools/        # Game packager, asset tools
│   └── ai/           # AI assistant aggregator
│
├── runtime/          # Atlas Runtime (standalone executable)
│
├── atlas_tests/      # Engine unit tests
│
├── schemas/          # Versioned schemas
│   ├── atlas.project.v1.json
│   ├── atlas.worldgraph.v1.json
│   ├── atlas.strategygraph.v1.json
│   └── atlas.conversation.v1.json
│
├── projects/         # External game projects (loaded via Plugin.toml / .atlas files)
│   ├── eveoffline/   # EVEOFFLINE game project
│   ├── arena2d/      # Arena2D reference project (2D scalability proof)
│   └── atlas-sample/ # Minimal sample project
│
├── cpp_client/       # EVEOFFLINE game client (links AtlasEngine)
├── cpp_server/       # EVEOFFLINE game server (links AtlasEngine)
├── data/             # Moddable game content (JSON)
├── tools/            # Modding utilities (Blender addon, validators)
└── docs/             # Design documents, guides, session logs
```

## Engine Modules

### Core (`engine/core/`)
- **Engine**: Lifecycle management, mode selection (Editor/Client/Server)
- **Logger**: Structured logging
- **Capabilities**: Role-based feature gating (AssetWrite, Rendering, NetAuthority, etc.)

### ECS (`engine/ecs/`)
- Type-safe entity creation and destruction
- Component storage via `std::any` (type-indexed)
- Per-tick update callbacks

### Graph VM (`engine/graphvm/`)
- Deterministic bytecode virtual machine
- Graph IR (nodes + edges) compiled to linear opcodes
- Arithmetic, comparison, branching, variables, events

### Assets (`engine/assets/`)
- Binary asset format (`.atlasb`) with header, type, version
- Registry with filesystem scanning
- Hot-reload via file timestamp polling

### Networking (`engine/net/`)
- Modes: Standalone, Client, Server, P2P_Host, P2P_Peer
- Packet-based send/receive with tick stamping
- Peer management (add, remove, RTT tracking)
- Snapshot save/rollback/replay for lockstep networking

### Simulation (`engine/sim/`)
- Fixed-rate tick scheduler
- Deterministic time stepping

### World Generation (`engine/world/`)
- Cube-sphere planetary projection with LOD
- Voxel grid chunked layout
- Perlin/FBM noise generation (seeded, deterministic)
- Terrain mesh generation with normals
- World streaming with disk cache
- Galaxy generation (star systems, regions, star classes)
- **WorldGraph**: DAG-based world generation graph with compile/execute pipeline
  - Typed value system (Float, HeightField, Seed, Mask, etc.)
  - Topological sort compilation with cycle detection
  - Deterministic per-chunk execution
  - Concrete nodes: Seed, Noise (FBM), Blend, Clamp, Constant
- **HeightfieldMesher**: Heightfield → mesh pipeline with LOD support

### Project System (`engine/project/`)
- Load and validate `.atlas` project files
- Schema-versioned project format (`atlas.project.v1`)
- Module references (world graphs, strategy graphs, AI)
- Runtime configuration (entry world, tick rate)

### Command System (`engine/command/`)
- Abstract `Command` base with Execute/Undo
- `CommandHistory` for undo/redo stacks
- Foundation for editor mutations and multiplayer sync

### Interaction System (`engine/interaction/`)
- **Utterance**: Raw text with speaker/listener context
- **Intent**: Resolved action with parameters and confidence score
- **IntentResolver**: Abstract resolver interface (rule-based, ML, hybrid)
- **PatternIntentResolver**: Case-insensitive phrase matching
- **IntentRegistry**: Central dispatch for intent handlers
- Unified pipeline: Voice → Text → AI → Intent → Command → System

### Voice Commands (`engine/voice/`)
- **VoiceCommandRegistry**: Register and match voice phrases to intents
- Context-aware filtering (game, editor, or both)
- Longest-match phrase resolution
- Bridge between voice input and the intent system

### Plugin System (`engine/plugin/`)
- **PluginDescriptor**: Name, version, compatibility, determinism flag
- **PluginValidator**: Validates descriptor, version compatibility, determinism
- **PluginRegistry**: Register, find, filter plugins by type
- Enforcement: plugins must be deterministic, version-compatible
- Plugin types: graph-extension, editor-panel, asset-importer

### Strategy Graphs (`engine/strategygraph/`)
- DAG-based strategy decision graph with compile/execute pipeline
- **StrategyPinType**: ScalarField, VectorField, Mask, Influence, Heatmap
- **StrategyNode**: Abstract node base with typed ports and evaluation
- **StrategyGraph**: DAG container with topological sort, cycle detection, type validation
- Concrete nodes: InfluenceRing, ThreatField, ObjectiveScore, Constant, ReadAISignal, EmitAction
- Deterministic execution for multiplayer-safe AI decisions
- StrategyGraphs are advisory — they never affect physics or spawn entities

### Server Rules (`engine/rules/`)
- **RuleDescriptor**: Name, base value, multiplier, bounds, hot-reload flag, replay impact
- **ServerRules**: Singleton registry for live parameter tuning
- Multiplier-based: `final_value = base_value * multiplier`
- Clamped to declared min/max bounds
- Hot-reloadable rules apply at tick boundaries
- Replay-aware: rules affecting replay are explicitly marked

### Conversation Graphs (`engine/conversation/`)
- DAG-based dialogue and memory graph with compile/execute pipeline
- **ConversationPinType**: Dialogue, Choice, Memory, Relationship, Signal
- **ConversationNode**: Abstract node base with typed ports and evaluation
- Concrete nodes: Dialogue, PlayerChoice, MemoryWrite, RelationshipDelta
- ConversationGraphs emit state — they never execute actions or access ECS
- Authority model: ConversationGraph → emits state → AISignals → StrategyGraph → decides

### AI Signals (`engine/ai/`)
- **AISignalRegistry**: Singleton registry for namespaced numeric AI inputs
- Signals are registered with namespace + name (e.g. `faction.morale`)
- Read/write with tick tracking for temporal awareness
- Used as inputs to StrategyGraph decision nodes
- Mods may register namespaced signals but may not override core signals

### AI Memory (`engine/ai/`)
- **AIMemory**: Key-value memory storage with tick-based decay
- Memories have strength values that decay each tick
- Permanent memories (decay rate = 0) persist indefinitely
- Expired memories (strength ≤ 0) are automatically removed on tick
- Used by NPC AI for persistent state across encounters

### Relationship Model (`engine/ai/`)
- **RelationshipModel**: Symmetric relationship tracking between entity pairs
- Entity pair → float value (symmetric: A→B == B→A)
- Modify relationships with delta adjustments
- Foundation for NPC faction standing and social dynamics

### TileGraph (`engine/tile/`)
- DAG-based 2D tile-map generation graph with compile/execute pipeline
- **TilePinType**: TileID, TileMap, Float, Mask, Seed, Metadata
- Concrete nodes: BaseTile, NoiseField, TileSelect, BiomeGate
- Deterministic procedural 2D worlds (roguelikes, tactics, side-scrollers)

### SoundGraph (`engine/sound/`)
- DAG-based procedural audio generation graph
- **SoundPinType**: AudioBuffer, Float, Seed, Trigger, Envelope
- Concrete nodes: Oscillator, Gain, Mix, Envelope
- Deterministic sound synthesis without external audio libraries

### CharacterGraph (`engine/character/`)
- DAG-based modular character generation graph
- **CharacterPinType**: Float, Mesh, Material, Skeleton, Seed, Equipment
- Concrete nodes: BaseBody, Skeleton, Material, Equipment
- Characters generated from seed + faction + parameters

### AnimationGraph (`engine/animation/`)
- DAG-based animation state machine with modifier system
- **AnimPinType**: Float, Pose, Modifier, Trigger, Mask
- Concrete nodes: Clip, Blend, Modifier (damage/skill/emotion), StateMachine
- Modifiers are additive layers (limp, recoil, tremor) that warp poses

### WeaponGraph (`engine/weapon/`)
- DAG-based weapon construction graph
- **WeaponPinType**: Float, Stats, Seed, Component, Profile
- Concrete nodes: Receiver, Barrel, RecoilProfile, WeaponStats
- Weapon wear degrades performance; faction identity drives variation

### BehaviorGraph (`engine/ai/`)
- DAG-based authorable AI behavior graph
- **BehaviorPinType**: Float, Bool, Action, Perception, EmotionState
- Concrete nodes: ThreatAssessment, UtilityScore, ActionSelector, EmotionUpdate
- AI behavior becomes content, not code — same graph works for FPS, RTS, NPCs

### UIGraph (`engine/ui/`)
- DAG-based UI composition graph system
- **UIPinType**: Float, Bool, String, Layout, Action
- Concrete nodes: Panel, Button, Text, List
- Title screens, inventories, HUDs authored as graphs

### GameFlowGraph (`engine/flow/`)
- DAG-based game flow state machine
- **FlowPinType**: Trigger, State, Bool, Float, String
- Concrete nodes: State, Transition, Timer, Condition
- Full game flow (Boot → Menu → Gameplay → Credits) as a graph

### Schema Validator (`engine/schema/`)
- **SchemaDefinition**: ID, version, fields, node definitions
- **SchemaValidator**: Validates schemas (unique IDs, valid versions, field integrity)
- Engine-level enforcement — schemas validated before load

### Graph Cache (`engine/graphvm/`)
- **GraphCache**: Execution result caching with tick-based invalidation
- Cache key = hash(graphID, seed, lod) for deterministic lookup
- EvictBefore(tick) removes stale entries
- Prevents redundant graph re-evaluation

### Graph Serialization (`engine/graphvm/`)
- **JsonBuilder**: Minimal JSON writer for graph persistence
- **JsonReader**: Minimal JSON parser for graph loading
- No external JSON library dependency — pure C++ standard library

### Camera / World Modes (`engine/camera/`)
- **WorldMode**: SideScroller2D, TopDown2D, TopDownOrbit2_5D, Isometric2_5D
- **CameraProjectionPolicy**: Mode-specific projection parameters
- Gameplay logic must never depend on projection
- Camera defines perception, not simulation

## Runtime Modes

| Mode   | Rendering | Networking | Asset Write | Graph Edit |
|--------|-----------|------------|-------------|------------|
| Editor | ✅         | Optional   | ✅           | ✅          |
| Client | ✅         | ✅          | ❌           | ❌          |
| Server | ❌         | ✅          | ❌           | ❌          |

## Determinism Contract

All simulation systems must satisfy:

> **Same project + same seed + same inputs = same outputs (always)**

Rules:
- No system clock access in simulation
- No global RNG — use injected `DeterministicRNG` only
- No threading outside the tick scheduler
- Fixed-point or deterministic floating-point only

## Project Schema

Projects are defined by a single `project.atlas` JSON file conforming to
`schemas/atlas.project.v1.json`. See the schema file for the full specification.

### Graph Schemas

- **WorldGraph** files (`.worldgraph`) conform to `schemas/atlas.worldgraph.v1.json`
- **StrategyGraph** files (`.strategygraph`) conform to `schemas/atlas.strategygraph.v1.json`
- **ConversationGraph** files (`.conversation`) conform to `schemas/atlas.conversation.v1.json`
- **TileGraph** files (`.tilegraph`) for 2D procedural generation
- **BehaviorGraph** files (`.behaviorgraph`) for AI behavior authoring

### Project Directory

External game projects live under `projects/` and are loaded via their Plugin.toml + `.atlas` manifest:

```
projects/
├── eveoffline/           # EVEOFFLINE reference implementation
│   ├── eveoffline.atlas  # Project manifest
│   ├── Plugin.toml       # Plugin descriptor
│   ├── worlds/           # WorldGraph + TileGraph files
│   ├── strategy/         # StrategyGraph files
│   ├── conversations/    # ConversationGraph files
│   ├── ai/               # AI configuration
│   ├── data/             # Data manifest
│   ├── config/           # Runtime configuration
│   └── assets/           # Art assets
├── arena2d/              # Arena2D reference project (2D scalability proof)
│   ├── arena2d.atlas
│   ├── Plugin.toml
│   ├── worlds/
│   ├── ai/
│   ├── data/
│   └── config/
└── atlas-sample/         # Minimal sample project
    ├── sample.atlas
    └── worlds/
```

See [Project Guidelines](docs/PROJECT_GUIDELINES.md) for complete rules.

## Editor Flow

1. Atlas Editor launches
2. **Project Picker** modal appears — user selects a `.atlas` file
3. Project is mounted and validated
4. Declared modules are initialized (WorldGraphs, StrategyGraphs, AI)
5. Editor enters Project Context — tools unlock
6. User can press ▶ Play to launch runtime for live testing

### Editor Panels

| Panel | Purpose |
|-------|---------|
| ECS Inspector | Entity and component exploration |
| Console | Command execution and logging |
| Network Inspector | Network state debugging |
| **Project Picker** | Project selection and recent projects |
| **World Graph Editor** | Visual WorldGraph authoring and preview |
| **Voice Commands** | Voice command testing and monitoring |
| **Interaction Debugger** | AI interaction logging and analysis |
| Game Packager | Build configuration and packaging |

## Build System

The root `CMakeLists.txt` controls all build targets. **All targets are ON by default**
so a single build produces every executable, making everything debuggable at the same time.

| Option              | Default | Description                     |
|---------------------|---------|---------------------------------|
| BUILD_ATLAS_ENGINE  | ON      | Atlas Engine static library     |
| BUILD_ATLAS_TESTS   | ON      | Engine unit tests               |
| BUILD_ATLAS_EDITOR  | ON      | Atlas Editor application        |
| BUILD_ATLAS_RUNTIME | ON      | Atlas Runtime application       |
| BUILD_CLIENT        | ON      | EVEOFFLINE game client          |
| BUILD_SERVER        | ON      | EVEOFFLINE game server          |

Individual targets can be disabled when only a subset is needed (e.g. CI engine-only builds).

## Testing

All engine modules have corresponding test files in `atlas_tests/`.
Tests use `assert()` with `[PASS]` output and are registered in `main.cpp`.
Run with: `make test-engine`
