#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>

namespace atlas::ecs {

using EntityID = uint32_t;
using ComponentTypeID = uint32_t;

struct ComponentData {
    std::vector<uint8_t> data;
    size_t elementSize = 0;
};

class World {
public:
    EntityID CreateEntity();
    void DestroyEntity(EntityID id);

    bool IsAlive(EntityID id) const;
    std::vector<EntityID> GetEntities() const;
    size_t EntityCount() const;

    void Update(float dt);

    void SetTickCallback(std::function<void(float)> cb);

private:
    EntityID m_nextID = 1;
    std::vector<EntityID> m_entities;
    std::function<void(float)> m_tickCallback;
};

}
