#include "../engine/flow/GameFlowGraph.h"
#include <iostream>
#include <cassert>

using namespace atlas::flow;

void test_flow_add_node() {
    GameFlowGraph flow;
    flow.Init();

    uint32_t id = flow.AddNode(FlowNodeType::MainMenu, "main_menu");
    assert(id > 0);
    assert(flow.GetNode(id) != nullptr);
    assert(flow.GetNode(id)->type == FlowNodeType::MainMenu);
    assert(flow.GetNode(id)->name == "main_menu");
    assert(flow.NodeCount() == 1);

    flow.RemoveNode(id);
    assert(flow.GetNode(id) == nullptr);
    assert(flow.NodeCount() == 0);

    std::cout << "[PASS] test_flow_add_node" << std::endl;
}

void test_flow_transitions() {
    GameFlowGraph flow;
    flow.Init();

    uint32_t menu = flow.AddNode(FlowNodeType::MainMenu, "menu");
    uint32_t game = flow.AddNode(FlowNodeType::Gameplay, "gameplay");
    uint32_t pause = flow.AddNode(FlowNodeType::Pause, "pause");

    flow.AddTransition(menu, game);
    flow.AddTransition(game, pause);

    auto trans = flow.GetTransitions(menu);
    assert(trans.size() == 1);
    assert(trans[0] == game);

    auto trans2 = flow.GetTransitions(game);
    assert(trans2.size() == 1);
    assert(trans2[0] == pause);

    std::cout << "[PASS] test_flow_transitions" << std::endl;
}

void test_flow_advance() {
    GameFlowGraph flow;
    flow.Init();

    uint32_t boot = flow.AddNode(FlowNodeType::Boot, "boot");
    uint32_t splash = flow.AddNode(FlowNodeType::Splash, "splash");
    uint32_t menu = flow.AddNode(FlowNodeType::MainMenu, "menu");

    flow.AddTransition(boot, splash);
    flow.AddTransition(splash, menu);

    flow.SetCurrentNode(boot);
    assert(flow.GetCurrentNode() == boot);

    flow.Advance();
    assert(flow.GetCurrentNode() == splash);

    flow.Advance();
    assert(flow.GetCurrentNode() == menu);

    std::cout << "[PASS] test_flow_advance" << std::endl;
}

void test_flow_screen_ref() {
    GameFlowGraph flow;
    flow.Init();

    uint32_t id = flow.AddNode(FlowNodeType::MainMenu, "menu");
    flow.SetScreenRef(id, "MainMenuScreen");

    const FlowNode* node = flow.GetNode(id);
    assert(node != nullptr);
    assert(node->screenRef == "MainMenuScreen");

    std::cout << "[PASS] test_flow_screen_ref" << std::endl;
}
