#include "GameFlowGraph.h"

namespace atlas::flow {

void GameFlowGraph::Init() {
    m_nodes.clear();
    m_currentId = 0;
    m_nextId = 1;
}

uint32_t GameFlowGraph::AddNode(FlowNodeType type, const std::string& name) {
    uint32_t id = m_nextId++;
    FlowNode node;
    node.id = id;
    node.type = type;
    node.name = name;
    m_nodes[id] = node;
    return id;
}

void GameFlowGraph::RemoveNode(uint32_t id) {
    m_nodes.erase(id);
}

const FlowNode* GameFlowGraph::GetNode(uint32_t id) const {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        return &it->second;
    }
    return nullptr;
}

size_t GameFlowGraph::NodeCount() const {
    return m_nodes.size();
}

void GameFlowGraph::AddTransition(uint32_t fromId, uint32_t toId) {
    auto it = m_nodes.find(fromId);
    if (it != m_nodes.end()) {
        it->second.nextNodes.push_back(toId);
    }
}

std::vector<uint32_t> GameFlowGraph::GetTransitions(uint32_t fromId) const {
    auto it = m_nodes.find(fromId);
    if (it != m_nodes.end()) {
        return it->second.nextNodes;
    }
    return {};
}

void GameFlowGraph::SetCurrentNode(uint32_t id) {
    m_currentId = id;
}

uint32_t GameFlowGraph::GetCurrentNode() const {
    return m_currentId;
}

void GameFlowGraph::Advance() {
    auto it = m_nodes.find(m_currentId);
    if (it != m_nodes.end() && !it->second.nextNodes.empty()) {
        m_currentId = it->second.nextNodes[0];
    }
}

void GameFlowGraph::SetScreenRef(uint32_t id, const std::string& screenName) {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        it->second.screenRef = screenName;
    }
}

} // namespace atlas::flow
