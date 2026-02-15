#include "TileGraph.h"

namespace atlas::tile {

void TileMap::Init(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_defs.clear();
    m_nextDefId = 1;
    m_grid.assign(height, std::vector<TileInstance>(width));
}

uint32_t TileMap::RegisterTile(const std::string& name, TileType type, TileFlags flags) {
    uint32_t id = m_nextDefId++;
    m_defs[id] = {id, name, type, flags};
    return id;
}

void TileMap::SetTile(uint32_t x, uint32_t y, uint32_t defId) {
    if (IsValid(x, y)) {
        m_grid[y][x].defId = defId;
    }
}

const TileInstance* TileMap::GetTile(uint32_t x, uint32_t y) const {
    if (IsValid(x, y)) {
        return &m_grid[y][x];
    }
    return nullptr;
}

const TileDefinition* TileMap::GetDefinition(uint32_t defId) const {
    auto it = m_defs.find(defId);
    if (it != m_defs.end()) {
        return &it->second;
    }
    return nullptr;
}

size_t TileMap::DefinitionCount() const {
    return m_defs.size();
}

uint32_t TileMap::Width() const {
    return m_width;
}

uint32_t TileMap::Height() const {
    return m_height;
}

bool TileMap::IsValid(uint32_t x, uint32_t y) const {
    return x < m_width && y < m_height;
}

void TileMap::DamageTile(uint32_t x, uint32_t y, float amount) {
    if (IsValid(x, y)) {
        m_grid[y][x].damage += amount;
    }
}

} // namespace atlas::tile
