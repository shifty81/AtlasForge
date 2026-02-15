#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace atlas::tile {

enum class TileType {
    Empty,
    Ground,
    Wall,
    Water,
    Lava,
    Custom
};

struct TileFlags {
    bool walkable = false;
    bool transparent = false;
    bool destructible = false;
};

struct TileDefinition {
    uint32_t id;
    std::string name;
    TileType type;
    TileFlags flags;
};

struct TileInstance {
    uint32_t defId = 0;
    float damage = 0.0f;
    uint8_t variant = 0;
};

class TileMap {
public:
    void Init(uint32_t width, uint32_t height);
    uint32_t RegisterTile(const std::string& name, TileType type, TileFlags flags);
    void SetTile(uint32_t x, uint32_t y, uint32_t defId);
    const TileInstance* GetTile(uint32_t x, uint32_t y) const;
    const TileDefinition* GetDefinition(uint32_t defId) const;
    size_t DefinitionCount() const;
    uint32_t Width() const;
    uint32_t Height() const;
    bool IsValid(uint32_t x, uint32_t y) const;
    void DamageTile(uint32_t x, uint32_t y, float amount);

private:
    std::map<uint32_t, TileDefinition> m_defs;
    std::vector<std::vector<TileInstance>> m_grid;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_nextDefId = 1;
};

} // namespace atlas::tile
