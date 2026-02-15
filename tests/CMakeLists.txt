add_executable(AtlasTests
    main.cpp
    test_graphvm.cpp
    test_ecs.cpp
    test_components.cpp
    test_asset.cpp
    test_net.cpp
    test_net_queue.cpp
    test_world.cpp
    test_compiler.cpp
    test_engine.cpp
    test_console.cpp
    test_terrain.cpp
    test_noise.cpp
    test_streaming.cpp
    test_galaxy.cpp
    test_interaction.cpp
    test_diplomacy.cpp
    test_asset_graph.cpp
    test_mod.cpp
    test_editor_assistant.cpp
    test_input.cpp
    test_camera.cpp
    test_physics.cpp
    test_audio.cpp
    test_gameplay.cpp
    test_skill.cpp
    test_weapon.cpp
    test_character.cpp
    test_animation.cpp
    test_tile.cpp
    test_sound.cpp
    test_ui_screen.cpp
    test_flow.cpp
    test_story.cpp
    ${CMAKE_SOURCE_DIR}/editor/panels/ConsolePanel.cpp
    ${CMAKE_SOURCE_DIR}/editor/panels/InteractionDebugger.cpp
    ${CMAKE_SOURCE_DIR}/editor/assistant/EditorAssistant.cpp
)

target_link_libraries(AtlasTests AtlasEngine)
target_include_directories(AtlasTests PRIVATE ${CMAKE_SOURCE_DIR}/editor)

add_test(NAME AtlasTests COMMAND AtlasTests)
