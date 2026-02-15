#include "../engine/world/WorldGraph.h"
#include "../engine/world/WorldNodes.h"
#include <iostream>
#include <cassert>

void test_worldgraph_add_nodes() {
    atlas::world::WorldGraph graph;
    auto node = std::make_unique<atlas::world::ConstantNode>();
    node->value = 42.0f;
    auto id = graph.AddNode(std::move(node));
    assert(id == 1);
    assert(graph.NodeCount() == 1);
    std::cout << "[PASS] test_worldgraph_add_nodes" << std::endl;
}

void test_worldgraph_remove_node() {
    atlas::world::WorldGraph graph;
    auto node = std::make_unique<atlas::world::ConstantNode>();
    node->value = 1.0f;
    auto id = graph.AddNode(std::move(node));
    graph.RemoveNode(id);
    assert(graph.NodeCount() == 0);
    std::cout << "[PASS] test_worldgraph_remove_node" << std::endl;
}

void test_worldgraph_compile_empty() {
    atlas::world::WorldGraph graph;
    assert(graph.Compile());
    assert(graph.IsCompiled());
    std::cout << "[PASS] test_worldgraph_compile_empty" << std::endl;
}

void test_worldgraph_compile_single_node() {
    atlas::world::WorldGraph graph;
    auto node = std::make_unique<atlas::world::ConstantNode>();
    node->value = 3.14f;
    auto id = graph.AddNode(std::move(node));
    assert(graph.Compile());

    atlas::world::WorldGenContext ctx{42, 0, 0, 0, 0};
    assert(graph.Execute(ctx));

    auto* output = graph.GetOutput(id, 0);
    assert(output != nullptr);
    assert(output->type == atlas::world::ValueType::Float);
    assert(!output->data.empty());
    float val = output->data[0];
    assert(val > 3.13f && val < 3.15f);
    std::cout << "[PASS] test_worldgraph_compile_single_node" << std::endl;
}

void test_worldgraph_compile_chain() {
    atlas::world::WorldGraph graph;
    auto seedId = graph.AddNode(std::make_unique<atlas::world::SeedNode>());
    auto freqNode = std::make_unique<atlas::world::ConstantNode>();
    freqNode->value = 0.01f;
    auto freqId = graph.AddNode(std::move(freqNode));
    auto noiseId = graph.AddNode(std::make_unique<atlas::world::NoiseNode>());

    // Connect seed -> noise input 0 (Seed), freq -> noise input 1 (Frequency)
    graph.AddEdge({seedId, 0, noiseId, 0});
    graph.AddEdge({freqId, 0, noiseId, 1});

    assert(graph.Compile());

    atlas::world::WorldGenContext ctx{12345, 0, 0, 0, 0};
    assert(graph.Execute(ctx));

    auto* output = graph.GetOutput(noiseId, 0);
    assert(output != nullptr);
    assert(output->type == atlas::world::ValueType::HeightField);
    assert(output->data.size() == 64 * 64);
    std::cout << "[PASS] test_worldgraph_compile_chain" << std::endl;
}

void test_worldgraph_deterministic() {
    auto buildAndRun = [](uint64_t seed) -> std::vector<float> {
        atlas::world::WorldGraph graph;
        auto seedId = graph.AddNode(std::make_unique<atlas::world::SeedNode>());
        auto freqNode = std::make_unique<atlas::world::ConstantNode>();
        freqNode->value = 0.02f;
        auto freqId = graph.AddNode(std::move(freqNode));
        auto noiseId = graph.AddNode(std::make_unique<atlas::world::NoiseNode>());
        graph.AddEdge({seedId, 0, noiseId, 0});
        graph.AddEdge({freqId, 0, noiseId, 1});
        graph.Compile();
        atlas::world::WorldGenContext ctx{seed, 0, 0, 0, 0};
        graph.Execute(ctx);
        return graph.GetOutput(noiseId, 0)->data;
    };

    auto a = buildAndRun(42);
    auto b = buildAndRun(42);
    assert(a == b);

    auto c = buildAndRun(99);
    assert(a != c);
    std::cout << "[PASS] test_worldgraph_deterministic" << std::endl;
}

void test_worldgraph_clamp_node() {
    // Build a noise heightfield, then clamp it to [0, 1]
    atlas::world::WorldGraph graph;
    auto seedId = graph.AddNode(std::make_unique<atlas::world::SeedNode>());
    auto freqNode = std::make_unique<atlas::world::ConstantNode>();
    freqNode->value = 0.01f;
    auto freqId = graph.AddNode(std::move(freqNode));
    auto noiseId = graph.AddNode(std::make_unique<atlas::world::NoiseNode>());

    auto minNode = std::make_unique<atlas::world::ConstantNode>();
    minNode->value = 0.0f;
    auto minId = graph.AddNode(std::move(minNode));
    auto maxNode = std::make_unique<atlas::world::ConstantNode>();
    maxNode->value = 1.0f;
    auto maxId = graph.AddNode(std::move(maxNode));
    auto clampId = graph.AddNode(std::make_unique<atlas::world::ClampNode>());

    graph.AddEdge({seedId, 0, noiseId, 0});
    graph.AddEdge({freqId, 0, noiseId, 1});
    graph.AddEdge({noiseId, 0, clampId, 0}); // HeightField -> In
    graph.AddEdge({minId, 0, clampId, 1});    // Float -> Min
    graph.AddEdge({maxId, 0, clampId, 2});    // Float -> Max

    assert(graph.Compile());
    atlas::world::WorldGenContext ctx{1, 0, 0, 0, 0};
    assert(graph.Execute(ctx));

    auto* output = graph.GetOutput(clampId, 0);
    assert(output != nullptr);
    // All values should be in [0, 1]
    for (float v : output->data) {
        assert(v >= 0.0f && v <= 1.0f);
    }
    std::cout << "[PASS] test_worldgraph_clamp_node" << std::endl;
}
