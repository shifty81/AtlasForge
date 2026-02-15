#include "../engine/sound/SoundGraph.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace atlas::sound;

void test_sound_add_node() {
    SoundGraph graph;
    graph.Init();

    uint32_t id = graph.AddNode(SoundNodeType::Generator, GeneratorType::Sine);
    assert(id > 0);
    assert(graph.GetNode(id) != nullptr);
    assert(graph.GetNode(id)->type == SoundNodeType::Generator);
    assert(graph.GetNode(id)->generatorType == GeneratorType::Sine);
    assert(graph.NodeCount() == 1);

    uint32_t id2 = graph.AddNode(SoundNodeType::Filter, GeneratorType::Saw);
    assert(id2 > id);
    assert(graph.NodeCount() == 2);

    graph.RemoveNode(id);
    assert(graph.GetNode(id) == nullptr);
    assert(graph.NodeCount() == 1);

    std::cout << "[PASS] test_sound_add_node" << std::endl;
}

void test_sound_set_param() {
    SoundGraph graph;
    graph.Init();

    uint32_t id = graph.AddNode(SoundNodeType::Generator, GeneratorType::Sine);
    graph.SetParam(id, "frequency", 440.0f);

    const SoundNode* node = graph.GetNode(id);
    assert(node != nullptr);
    assert(node->params.size() == 1);
    assert(node->params[0].name == "frequency");
    assert(std::abs(node->params[0].value - 440.0f) < 0.01f);

    graph.SetParam(id, "frequency", 880.0f);
    node = graph.GetNode(id);
    assert(node->params.size() == 1);
    assert(std::abs(node->params[0].value - 880.0f) < 0.01f);

    std::cout << "[PASS] test_sound_set_param" << std::endl;
}

void test_sound_bind_action() {
    SoundGraph graph;
    graph.Init();

    uint32_t id = graph.AddNode(SoundNodeType::Generator, GeneratorType::Square);
    graph.BindAction("jump", id);

    assert(graph.BindingCount() == 1);
    const SoundActionBinding* binding = graph.GetBinding("jump");
    assert(binding != nullptr);
    assert(binding->actionName == "jump");
    assert(binding->soundNodeId == id);

    assert(graph.GetBinding("attack") == nullptr);

    std::cout << "[PASS] test_sound_bind_action" << std::endl;
}

void test_sound_evaluate() {
    SoundGraph graph;
    graph.Init();

    uint32_t id = graph.AddNode(SoundNodeType::Generator, GeneratorType::Sine);
    graph.SetParam(id, "frequency", 440.0f);

    float val = graph.Evaluate(0.0f);
    assert(std::abs(val) < 0.01f);

    float val2 = graph.Evaluate(0.001f);
    assert(std::abs(val2) > 0.0f);

    std::cout << "[PASS] test_sound_evaluate" << std::endl;
}
