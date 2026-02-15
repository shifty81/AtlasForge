#include "ECS.h"
#include <algorithm>

namespace atlas::ecs {

EntityID World::CreateEntity() {
    EntityID id = m_nextID++;
    m_entities.push_back(id);
    return id;
}

void World::DestroyEntity(EntityID id) {
    m_entities.erase(
        std::remove(m_entities.begin(), m_entities.end(), id),
        m_entities.end()
    );
}

bool World::IsAlive(EntityID id) const {
    return std::find(m_entities.begin(), m_entities.end(), id) != m_entities.end();
}

std::vector<EntityID> World::GetEntities() const {
    return m_entities;
}

size_t World::EntityCount() const {
    return m_entities.size();
}

void World::Update(float dt) {
    if (m_tickCallback) {
        m_tickCallback(dt);
    }
}

void World::SetTickCallback(std::function<void(float)> cb) {
    m_tickCallback = std::move(cb);
}

}
