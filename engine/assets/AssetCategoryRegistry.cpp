#include "AssetCategoryRegistry.h"
#include <algorithm>

namespace atlas::asset {

void AssetCategoryRegistry::Register(AssetCategoryEntry entry) {
    // Replace existing entry for the same type.
    for (auto& e : m_entries) {
        if (e.type == entry.type) {
            e = std::move(entry);
            return;
        }
    }
    m_entries.push_back(std::move(entry));
}

bool AssetCategoryRegistry::Unregister(AssetType type) {
    auto it = std::remove_if(m_entries.begin(), m_entries.end(),
                             [type](const AssetCategoryEntry& e) { return e.type == type; });
    if (it == m_entries.end()) return false;
    m_entries.erase(it, m_entries.end());
    return true;
}

const AssetCategoryEntry* AssetCategoryRegistry::Find(AssetType type) const {
    for (const auto& e : m_entries) {
        if (e.type == type) return &e;
    }
    return nullptr;
}

const std::vector<AssetCategoryEntry>& AssetCategoryRegistry::Entries() const {
    return m_entries;
}

size_t AssetCategoryRegistry::EntryCount() const {
    return m_entries.size();
}

std::vector<AssetCategoryEntry> AssetCategoryRegistry::ByContent(ContentCategory category) const {
    std::vector<AssetCategoryEntry> result;
    for (const auto& e : m_entries) {
        if (e.content == category) result.push_back(e);
    }
    return result;
}

std::vector<AssetCategoryEntry> AssetCategoryRegistry::BySystem(SystemUsage system) const {
    std::vector<AssetCategoryEntry> result;
    for (const auto& e : m_entries) {
        if (e.system == system) result.push_back(e);
    }
    return result;
}

ContentCategory AssetCategoryRegistry::ContentFor(AssetType type) const {
    const auto* entry = Find(type);
    return entry ? entry->content : ContentCategory::Data;
}

SystemUsage AssetCategoryRegistry::SystemFor(AssetType type) const {
    const auto* entry = Find(type);
    return entry ? entry->system : SystemUsage::General;
}

void AssetCategoryRegistry::RegisterDefaults() {
    Register({AssetType::Mesh,        ContentCategory::Visual,    SystemUsage::Rendering,
              "Mesh",        "3D mesh geometry"});
    Register({AssetType::Texture,     ContentCategory::Visual,    SystemUsage::Rendering,
              "Texture",     "2D image / texture map"});
    Register({AssetType::Material,    ContentCategory::Visual,    SystemUsage::Rendering,
              "Material",    "Surface material definition"});
    Register({AssetType::Audio,       ContentCategory::Audio,     SystemUsage::General,
              "Audio",       "Sound effect or music track"});
    Register({AssetType::Font,        ContentCategory::Font,      SystemUsage::UI,
              "Font",        "TrueType or OpenType font"});
    Register({AssetType::Prefab,      ContentCategory::Composite, SystemUsage::General,
              "Prefab",      "Reusable entity composition"});
    Register({AssetType::Script,      ContentCategory::Data,      SystemUsage::Gameplay,
              "Script",      "Gameplay or behavior script"});
    Register({AssetType::Data,        ContentCategory::Data,      SystemUsage::General,
              "Data",        "Generic data asset"});
    Register({AssetType::SpriteSheet, ContentCategory::Visual,    SystemUsage::UI,
              "SpriteSheet", "2D sprite atlas with frame metadata"});
}

void AssetCategoryRegistry::Clear() {
    m_entries.clear();
}

}  // namespace atlas::asset
