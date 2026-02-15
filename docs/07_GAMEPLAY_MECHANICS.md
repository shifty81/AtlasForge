# Atlas Engine — Gameplay Mechanics

## Core Principle

Gameplay mechanics are **interpreted, not compiled**.
The editor creates mechanic assets. The client loads and runs them.

## Mechanic Representation

| Mechanic Type | Representation          |
|---------------|------------------------|
| Movement      | Component + node graph |
| Combat        | Data + state machine   |
| AI            | Behavior graph         |
| Economy       | Formula graphs         |
| UI logic      | Event graph            |
| Proc-gen      | Generator graph        |

## The Gameplay Stack

```
[ Editor Tools ]
       ↓
[ Gameplay Assets ]
       ↓
[ Runtime Interpreter (Graph VM) ]
       ↓
[ ECS + Net Sync ]
```

## ECS Binding

Mechanics bind to components, not entities:

```cpp
struct WeaponComponent {
    MechanicHandle fireLogic;
};

// Engine system
for (auto e : ECS.View<WeaponComponent>()) {
    runtime.Execute(e.fireLogic, e);
}
```

## Editor → Client Workflow

1. Open Mechanics Editor
2. Create or modify a graph
3. Bind graph to a component
4. Press **Play In Client**
5. Client hot-reloads mechanic
6. Test live
7. Adjust values
8. Repeat

No restart. No rebuild.

## Composable Game Mechanics

Each mechanic is:
- A Graph (logic)
- ECS bindings (data)
- Input mapping (controls)
- Camera behavior (view)

### Supported Mechanics

- Ship flight
- Strategy overview
- Docking transitions
- FPS interiors
- EVA (spacewalk)
- Station exploration
- Planetary building
- Combat systems

## Network Safety

Because mechanics are assets, deterministic, and graph-based:
- Run on server only
- Mirror on client
- Lockstep in P2P
- Validate outputs
