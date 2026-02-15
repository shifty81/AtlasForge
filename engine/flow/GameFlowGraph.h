#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace atlas::flow {

enum class FlowNodeType : uint8_t {
    Boot,
    Splash,
    MainMenu,
    NewGame,
    CharacterSelect,
    Loading,
    Gameplay,
    Pause,
    GameOver,
    Credits
};

struct FlowNode {
    uint32_t id = 0;
    FlowNodeType type = FlowNodeType::Boot;
    std::string name;
    std::vector<uint32_t> nextNodes;
    std::string screenRef;
};

class GameFlowGraph {
public:
    void Init();
    uint32_t AddNode(FlowNodeType type, const std::string& name);
    void RemoveNode(uint32_t id);
    const FlowNode* GetNode(uint32_t id) const;
    size_t NodeCount() const;

    void AddTransition(uint32_t fromId, uint32_t toId);
    std::vector<uint32_t> GetTransitions(uint32_t fromId) const;

    void SetCurrentNode(uint32_t id);
    uint32_t GetCurrentNode() const;
    void Advance();

    void SetScreenRef(uint32_t id, const std::string& screenName);

private:
    std::unordered_map<uint32_t, FlowNode> m_nodes;
    uint32_t m_currentId = 0;
    uint32_t m_nextId = 1;
};

} // namespace atlas::flow
