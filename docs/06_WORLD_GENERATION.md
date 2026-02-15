# Atlas Engine — World Generation

## Core Principle

You do not edit the generated world.
You edit the **generator graph + parameters**.
The world is a cache, not the source of truth.

## World Layout Interface

The engine does not know if a world is 2D, 3D, voxel, planet, or graph-based.
Everything goes through the `WorldLayout` interface.

### Layout Types

| Layout             | Description       |
|--------------------|-------------------|
| `linear_1d`        | Side scroller     |
| `grid_2d`          | Zelda / Stardew   |
| `voxel_3d`         | Minecraft          |
| `cube_sphere`      | Planet surfaces    |
| `orbital_graph`    | Space systems      |
| `hierarchical_cells` | Strategy maps   |

## Planet Generation

- Cube-sphere mapping (6 faces, quadtree per face)
- LOD rings based on camera distance
- Generated via Graph VM
- Editable chunks with delta overlays
- Runtime deformation support

### Cube-Sphere Coordinate

```cpp
enum CubeFace { POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z };

struct CubeSphereCoord {
    CubeFace face;
    int x, y;
    int lod;
};
```

## Galaxy Generation

- Spiral galaxy templates
- Up to 100,000+ solar systems
- Systems generated on demand (seed + rules)
- Only seed, rules, and player changes are persisted

## Chunk Streaming

Worlds are streamed in chunks. You never load the whole planet.

```cpp
struct WorldChunk {
    ChunkCoord coord;
    HeightField height;
    MaterialMap materials;
    InstanceData instances;
};
```

### Generation Flow

```
Chunk Request → WorldSystem → GeneratorGraph.Execute(seed, params, coord) → ChunkData
```

## GameType Templates

Game types define how worlds are generated, structured, streamed, and simulated:

```json
{
  "id": "planet_demo",
  "type": "GameType",
  "data": {
    "world_layout": "cube_sphere",
    "chunk_schema": "planet_height_chunks",
    "generation_graph": "planet_height_v1",
    "streaming_policy": "planet_streaming",
    "camera_profile": "orbital_follow"
  }
}
```
