add_executable(AtlasTests
    main.cpp
    test_graphvm.cpp
    test_ecs.cpp
    test_asset.cpp
    test_net.cpp
    test_world.cpp
    test_compiler.cpp
)

target_link_libraries(AtlasTests AtlasEngine)

add_test(NAME AtlasTests COMMAND AtlasTests)
