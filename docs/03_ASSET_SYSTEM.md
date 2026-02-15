# Atlas Engine — Asset System

## Asset Rules

1. One format (`.atlas` / `.atlasb`)
2. No editor-only data
3. Deterministic loading
4. Graph-backed execution
5. Hot-reloadable
6. Same format for editor, client, and server

## Asset Types

```cpp
enum class AssetType : uint16_t {
    Graph,
    World,
    Planet,
    Galaxy,
    Mesh,
    Material,
    Mechanic,
    VoxelSchema,
    GameType,
    EditorProfile,
};
```

## Binary Layout

```
[AssetHeader]
[DependencyTable]
[SerializedData]
[Checksum]
```

### Asset Header

```cpp
struct AssetHeader {
    uint32_t magic;        // 'ATLS' (0x41544C53)
    uint16_t version;
    AssetType type;
    uint32_t size;
    uint64_t hash;
};
```

## Save / Load Path

### Editor Save
```
Editor → AssetCompiler → Binary Asset → AssetRegistry → Live Reload
```

### Runtime Load
```
Engine → AssetRegistry → Deserialize → Graph VM Compile → Runtime Instance
```

## Hot Reload

The engine includes a file watcher that monitors the assets directory:

```
File Change → AssetWatcher → AssetRegistry::Reload(id) → GraphVM::Rebind() → Runtime continues
```

### Safe Reload Rules
- Graphs replace atomically
- Old instances finish tick
- New instances use new data
- State migrated if compatible

## Multiplayer Constraint
- Server reloads first
- Clients receive updated asset
- Checksum mismatch → client resync
