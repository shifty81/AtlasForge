#include "../engine/render/GBuffer.h"
#include "../engine/render/PBRMaterial.h"
#include "../engine/render/ShadowMap.h"
#include "../engine/render/PostProcess.h"
#include "../engine/render/InstancedRenderer.h"
#include "../engine/render/SpatialHash.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

using namespace atlas::render;

// ============================================================
// G-Buffer tests
// ============================================================

void test_gbuffer_default_pbr_config() {
    auto config = GBuffer::DefaultPBRConfig(1920, 1080);
    assert(config.attachments.size() == 5);
    assert(config.width == 1920);
    assert(config.height == 1080);
    assert(config.enableDepth == true);
    // Verify the names of the five standard PBR attachments
    assert(config.attachments[0].name == "position");
    assert(config.attachments[1].name == "normal");
    assert(config.attachments[2].name == "albedo");
    assert(config.attachments[3].name == "metallic_roughness");
    assert(config.attachments[4].name == "depth");
    // Verify attachment dimensions match the config
    for (const auto& att : config.attachments) {
        assert(att.width == 1920);
        assert(att.height == 1080);
    }
    std::cout << "[PASS] test_gbuffer_default_pbr_config" << std::endl;
}

void test_gbuffer_init_shutdown() {
    GBuffer gb;
    assert(!gb.IsInitialized());

    auto config = GBuffer::DefaultPBRConfig(800, 600);
    gb.Init(config);
    assert(gb.IsInitialized());
    assert(gb.Width() == 800);
    assert(gb.Height() == 600);
    assert(gb.AttachmentCount() == 5);

    gb.Shutdown();
    assert(!gb.IsInitialized());
    assert(gb.Width() == 0);
    assert(gb.Height() == 0);
    assert(gb.AttachmentCount() == 0);
    std::cout << "[PASS] test_gbuffer_init_shutdown" << std::endl;
}

void test_gbuffer_bind_unbind() {
    GBuffer gb;
    auto config = GBuffer::DefaultPBRConfig(640, 480);
    gb.Init(config);
    assert(!gb.IsBound());

    gb.Bind();
    assert(gb.IsBound());

    gb.Unbind();
    assert(!gb.IsBound());

    // Bind after shutdown should not set bound
    gb.Shutdown();
    gb.Bind();
    assert(!gb.IsBound());
    std::cout << "[PASS] test_gbuffer_bind_unbind" << std::endl;
}

void test_gbuffer_resize() {
    GBuffer gb;
    auto config = GBuffer::DefaultPBRConfig(800, 600);
    gb.Init(config);
    assert(gb.Width() == 800);
    assert(gb.Height() == 600);

    gb.Resize(1920, 1080);
    assert(gb.Width() == 1920);
    assert(gb.Height() == 1080);

    // Verify all attachments were resized
    const GBufferAttachment* pos = gb.GetAttachment("position");
    assert(pos != nullptr);
    assert(pos->width == 1920);
    assert(pos->height == 1080);

    const GBufferAttachment* depth = gb.GetAttachment("depth");
    assert(depth != nullptr);
    assert(depth->width == 1920);
    assert(depth->height == 1080);
    std::cout << "[PASS] test_gbuffer_resize" << std::endl;
}

void test_gbuffer_get_attachment() {
    GBuffer gb;
    auto config = GBuffer::DefaultPBRConfig(512, 512);
    gb.Init(config);

    // Valid attachment lookup
    const GBufferAttachment* albedo = gb.GetAttachment("albedo");
    assert(albedo != nullptr);
    assert(albedo->name == "albedo");
    assert(albedo->format == GBufferFormat::RGBA8);
    assert(albedo->id != 0);

    const GBufferAttachment* normal = gb.GetAttachment("normal");
    assert(normal != nullptr);
    assert(normal->name == "normal");
    assert(normal->format == GBufferFormat::RGBA16F);

    // Missing attachment returns nullptr
    const GBufferAttachment* missing = gb.GetAttachment("specular");
    assert(missing == nullptr);

    // Each attachment should have a unique id
    const GBufferAttachment* pos = gb.GetAttachment("position");
    assert(pos->id != albedo->id);
    assert(pos->id != normal->id);
    std::cout << "[PASS] test_gbuffer_get_attachment" << std::endl;
}

// ============================================================
// PBR Material tests
// ============================================================

void test_pbr_default_params() {
    auto p = PBRMaterialParams::Default();
    assert(p.metallic == 0.0f);
    assert(p.roughness == 0.5f);
    assert(p.ao == 1.0f);
    assert(p.emissiveIntensity == 0.0f);
    assert(p.normalScale == 1.0f);
    assert(std::fabs(p.heightScale - 0.05f) < 1e-6f);
    assert(p.alphaMode == AlphaMode::Opaque);
    assert(p.alphaCutoff == 0.5f);
    assert(p.albedoColor[0] == 0.5f);
    assert(p.albedoColor[3] == 1.0f);
    std::cout << "[PASS] test_pbr_default_params" << std::endl;
}

void test_pbr_unique_ids() {
    PBRMaterial mat1;
    PBRMaterial mat2;
    PBRMaterial mat3;
    assert(mat1.GetId() != 0);
    assert(mat2.GetId() != 0);
    assert(mat3.GetId() != 0);
    assert(mat1.GetId() != mat2.GetId());
    assert(mat2.GetId() != mat3.GetId());
    assert(mat1.GetId() != mat3.GetId());
    // IDs should be monotonically increasing
    assert(mat2.GetId() > mat1.GetId());
    assert(mat3.GetId() > mat2.GetId());
    std::cout << "[PASS] test_pbr_unique_ids" << std::endl;
}

void test_pbr_texture_binding() {
    PBRMaterial mat;
    assert(mat.TextureBindingCount() == 0);
    assert(!mat.HasTexture(PBRTextureSlot::Albedo));

    mat.BindTexture(PBRTextureSlot::Albedo, 100, 200);
    assert(mat.HasTexture(PBRTextureSlot::Albedo));
    assert(mat.TextureBindingCount() == 1);
    const PBRTextureBinding* b = mat.GetTextureBinding(PBRTextureSlot::Albedo);
    assert(b != nullptr);
    assert(b->textureId == 100);
    assert(b->samplerId == 200);

    // Bind a second slot
    mat.BindTexture(PBRTextureSlot::Normal, 101, 201);
    assert(mat.TextureBindingCount() == 2);
    assert(mat.HasTexture(PBRTextureSlot::Normal));

    // Rebind same slot replaces rather than adding
    mat.BindTexture(PBRTextureSlot::Albedo, 999, 888);
    assert(mat.TextureBindingCount() == 2);
    assert(mat.GetTextureBinding(PBRTextureSlot::Albedo)->textureId == 999);

    // Unbind
    mat.UnbindTexture(PBRTextureSlot::Albedo);
    assert(!mat.HasTexture(PBRTextureSlot::Albedo));
    assert(mat.TextureBindingCount() == 1);

    // Unbinding non-existent slot is safe
    mat.UnbindTexture(PBRTextureSlot::Emissive);
    assert(mat.TextureBindingCount() == 1);
    std::cout << "[PASS] test_pbr_texture_binding" << std::endl;
}

void test_pbr_validate() {
    PBRMaterial mat;
    // Default params should be valid
    assert(mat.Validate());

    // Invalid metallic (> 1.0)
    PBRMaterialParams bad = PBRMaterialParams::Default();
    bad.metallic = 1.5f;
    mat.SetParams(bad);
    assert(!mat.Validate());

    // Invalid roughness (negative)
    bad = PBRMaterialParams::Default();
    bad.roughness = -0.1f;
    mat.SetParams(bad);
    assert(!mat.Validate());

    // Invalid AO (> 1.0)
    bad = PBRMaterialParams::Default();
    bad.ao = 2.0f;
    mat.SetParams(bad);
    assert(!mat.Validate());

    // Invalid albedo color (negative)
    bad = PBRMaterialParams::Default();
    bad.albedoColor[0] = -0.1f;
    mat.SetParams(bad);
    assert(!mat.Validate());

    // Invalid emissive intensity (negative)
    bad = PBRMaterialParams::Default();
    bad.emissiveIntensity = -1.0f;
    mat.SetParams(bad);
    assert(!mat.Validate());

    // Invalid emissive color (> 1.0)
    bad = PBRMaterialParams::Default();
    bad.emissiveColor[1] = 1.5f;
    mat.SetParams(bad);
    assert(!mat.Validate());

    // Restore valid params
    mat.SetParams(PBRMaterialParams::Default());
    assert(mat.Validate());
    std::cout << "[PASS] test_pbr_validate" << std::endl;
}

void test_pbr_emissive() {
    PBRMaterialParams p = PBRMaterialParams::Default();

    // Default is not emissive (intensity=0)
    assert(!p.IsEmissive());

    // Intensity > 0 but all emissive colors are 0 → not emissive
    p.emissiveIntensity = 1.0f;
    p.emissiveColor[0] = 0.0f;
    p.emissiveColor[1] = 0.0f;
    p.emissiveColor[2] = 0.0f;
    assert(!p.IsEmissive());

    // Emissive color > 0 but intensity 0 → not emissive
    p.emissiveIntensity = 0.0f;
    p.emissiveColor[0] = 0.5f;
    assert(!p.IsEmissive());

    // Both intensity > 0 and some color > 0 → emissive
    p.emissiveIntensity = 2.0f;
    p.emissiveColor[0] = 0.8f;
    assert(p.IsEmissive());

    // Green channel only
    p.emissiveColor[0] = 0.0f;
    p.emissiveColor[1] = 0.3f;
    assert(p.IsEmissive());

    // Blue channel only
    p.emissiveColor[1] = 0.0f;
    p.emissiveColor[2] = 0.1f;
    assert(p.IsEmissive());
    std::cout << "[PASS] test_pbr_emissive" << std::endl;
}

// ============================================================
// Shadow Map tests
// ============================================================

void test_shadow_default_config() {
    auto cfg = ShadowMapConfig::Default();
    assert(cfg.cascadeCount == 3);
    assert(cfg.baseResolution == 1024);
    assert(std::fabs(cfg.bias - 0.005f) < 1e-6f);
    assert(std::fabs(cfg.normalBias - 0.02f) < 1e-6f);
    assert(cfg.softShadows == true);
    assert(cfg.pcfSamples == 4);
    std::cout << "[PASS] test_shadow_default_config" << std::endl;
}

void test_shadow_init_cascades() {
    ShadowMap sm;
    assert(!sm.IsInitialized());

    auto cfg = ShadowMapConfig::Default();
    cfg.cascadeCount = 3;
    sm.Init(cfg);
    assert(sm.IsInitialized());
    assert(sm.CascadeCount() == 3);

    // Each cascade should have a unique id and the configured resolution
    for (uint32_t i = 0; i < sm.CascadeCount(); ++i) {
        const ShadowCascade* c = sm.GetCascade(i);
        assert(c != nullptr);
        assert(c->id != 0);
        assert(c->resolution == cfg.baseResolution);
    }

    // Out-of-range cascade returns nullptr
    assert(sm.GetCascade(99) == nullptr);

    sm.Shutdown();
    assert(!sm.IsInitialized());
    assert(sm.CascadeCount() == 0);
    std::cout << "[PASS] test_shadow_init_cascades" << std::endl;
}

void test_shadow_light_direction() {
    ShadowMap sm;
    sm.Init(ShadowMapConfig::Default());

    // Default direction is (0, -1, 0)
    LightDirection d = sm.GetLightDirection();
    assert(std::fabs(d.y - (-1.0f)) < 1e-5f);

    // Set and get a new normalized direction
    sm.SetLightDirection(1.0f, -1.0f, 0.0f);
    d = sm.GetLightDirection();
    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
    assert(std::fabs(len - 1.0f) < 1e-5f);
    // Expect (1/sqrt(2), -1/sqrt(2), 0)
    float expected = 1.0f / std::sqrt(2.0f);
    assert(std::fabs(d.x - expected) < 1e-5f);
    assert(std::fabs(d.y - (-expected)) < 1e-5f);
    assert(std::fabs(d.z) < 1e-5f);

    sm.Shutdown();
    std::cout << "[PASS] test_shadow_light_direction" << std::endl;
}

void test_shadow_update_cascades() {
    ShadowMap sm;
    auto cfg = ShadowMapConfig::Default();
    cfg.cascadeCount = 3;
    sm.Init(cfg);

    sm.SetLightDirection(0.0f, -1.0f, 0.5f);
    sm.UpdateCascades(0.1f, 500.0f, 60.0f, 16.0f / 9.0f);

    // Cascades should form a chain of near/far planes covering [0.1, 500]
    const ShadowCascade* c0 = sm.GetCascade(0);
    const ShadowCascade* c1 = sm.GetCascade(1);
    const ShadowCascade* c2 = sm.GetCascade(2);
    assert(c0 != nullptr && c1 != nullptr && c2 != nullptr);

    // Near plane of first cascade is camera near
    assert(std::fabs(c0->nearPlane - 0.1f) < 1e-4f);
    // Far plane of last cascade is camera far
    assert(std::fabs(c2->farPlane - 500.0f) < 1e-2f);

    // Each cascade's far == next cascade's near (chain)
    assert(std::fabs(c0->farPlane - c1->nearPlane) < 1e-4f);
    assert(std::fabs(c1->farPlane - c2->nearPlane) < 1e-4f);

    // Splits should be monotonically increasing
    assert(c0->nearPlane < c0->farPlane);
    assert(c1->nearPlane < c1->farPlane);
    assert(c2->nearPlane < c2->farPlane);

    // View and projection matrices should not be identity (they were populated)
    bool viewNonIdentity = false;
    for (int j = 0; j < 16; ++j) {
        float identVal = (j == 0 || j == 5 || j == 10 || j == 15) ? 1.0f : 0.0f;
        if (std::fabs(c0->viewMatrix[j] - identVal) > 1e-6f) viewNonIdentity = true;
    }
    assert(viewNonIdentity);

    bool projNonIdentity = false;
    for (int j = 0; j < 16; ++j) {
        float identVal = (j == 0 || j == 5 || j == 10 || j == 15) ? 1.0f : 0.0f;
        if (std::fabs(c0->projMatrix[j] - identVal) > 1e-6f) projNonIdentity = true;
    }
    assert(projNonIdentity);

    sm.Shutdown();
    std::cout << "[PASS] test_shadow_update_cascades" << std::endl;
}

void test_shadow_bind_states() {
    ShadowMap sm;
    sm.Init(ShadowMapConfig::Default());
    assert(!sm.IsBound());

    sm.BindForWriting(0);
    assert(sm.IsBound());

    sm.Unbind();
    assert(!sm.IsBound());

    sm.BindForReading();
    assert(sm.IsBound());

    sm.Unbind();
    assert(!sm.IsBound());

    sm.Shutdown();
    std::cout << "[PASS] test_shadow_bind_states" << std::endl;
}

// ============================================================
// Post-process tests
// ============================================================

void test_postprocess_default_settings() {
    auto s = PostProcessSettings::Default();
    assert(s.bloom.enabled == true);
    assert(s.toneMapping.enabled == true);
    assert(s.toneMapping.op == ToneMapOperator::ACES);
    assert(s.fxaaEnabled == true);
    assert(s.vignetteIntensity == 0.0f);
    assert(s.chromaticAberrationIntensity == 0.0f);
    assert(s.filmGrainIntensity == 0.0f);
    assert(std::fabs(s.toneMapping.gamma - 2.2f) < 1e-5f);
    assert(s.bloom.mipCount == 5);
    std::cout << "[PASS] test_postprocess_default_settings" << std::endl;
}

void test_postprocess_init_shutdown() {
    PostProcessPipeline pp;
    assert(!pp.IsInitialized());

    pp.Init(1920, 1080);
    assert(pp.IsInitialized());
    assert(pp.GetOutputTextureId() != 0);

    pp.Shutdown();
    assert(!pp.IsInitialized());
    assert(pp.GetOutputTextureId() == 0);

    // Re-init works
    pp.Init(800, 600);
    assert(pp.IsInitialized());
    pp.Shutdown();
    std::cout << "[PASS] test_postprocess_init_shutdown" << std::endl;
}

void test_postprocess_toggle_effects() {
    PostProcessPipeline pp;
    pp.Init(800, 600);

    // Default: bloom, tone mapping, FXAA are enabled
    assert(pp.IsEffectEnabled(PostProcessEffect::Bloom));
    assert(pp.IsEffectEnabled(PostProcessEffect::ToneMapping));
    assert(pp.IsEffectEnabled(PostProcessEffect::FXAA));
    assert(!pp.IsEffectEnabled(PostProcessEffect::Vignette));

    // Disable bloom
    pp.SetEffectEnabled(PostProcessEffect::Bloom, false);
    assert(!pp.IsEffectEnabled(PostProcessEffect::Bloom));

    // Re-enable bloom
    pp.SetEffectEnabled(PostProcessEffect::Bloom, true);
    assert(pp.IsEffectEnabled(PostProcessEffect::Bloom));

    // Enable vignette
    pp.SetEffectEnabled(PostProcessEffect::Vignette, true);
    assert(pp.IsEffectEnabled(PostProcessEffect::Vignette));

    // Disable vignette
    pp.SetEffectEnabled(PostProcessEffect::Vignette, false);
    assert(!pp.IsEffectEnabled(PostProcessEffect::Vignette));

    // Enable and disable chromatic aberration
    pp.SetEffectEnabled(PostProcessEffect::ChromaticAberration, true);
    assert(pp.IsEffectEnabled(PostProcessEffect::ChromaticAberration));
    pp.SetEffectEnabled(PostProcessEffect::ChromaticAberration, false);
    assert(!pp.IsEffectEnabled(PostProcessEffect::ChromaticAberration));

    pp.Shutdown();
    std::cout << "[PASS] test_postprocess_toggle_effects" << std::endl;
}

void test_postprocess_effect_count() {
    PostProcessPipeline pp;
    pp.Init(800, 600);

    // Default: bloom + tone mapping + FXAA = 3
    assert(pp.EffectCount() == 3);

    // Disable one
    pp.SetEffectEnabled(PostProcessEffect::FXAA, false);
    assert(pp.EffectCount() == 2);

    // Enable film grain
    pp.SetEffectEnabled(PostProcessEffect::FilmGrain, true);
    assert(pp.EffectCount() == 3);

    // Disable all
    pp.SetEffectEnabled(PostProcessEffect::Bloom, false);
    pp.SetEffectEnabled(PostProcessEffect::ToneMapping, false);
    pp.SetEffectEnabled(PostProcessEffect::FilmGrain, false);
    assert(pp.EffectCount() == 0);

    pp.Shutdown();
    std::cout << "[PASS] test_postprocess_effect_count" << std::endl;
}

void test_postprocess_resize() {
    PostProcessPipeline pp;
    pp.Init(800, 600);
    uint32_t texBefore = pp.GetOutputTextureId();

    pp.Resize(1920, 1080);
    // Resize re-allocates the output texture (new id)
    uint32_t texAfter = pp.GetOutputTextureId();
    assert(texAfter != texBefore);
    assert(texAfter != 0);

    // Resize when not initialized should be a no-op
    pp.Shutdown();
    uint32_t texShutdown = pp.GetOutputTextureId();
    pp.Resize(640, 480);
    assert(pp.GetOutputTextureId() == texShutdown);

    std::cout << "[PASS] test_postprocess_resize" << std::endl;
}

// ============================================================
// Instanced Renderer tests
// ============================================================

void test_instanced_init() {
    InstancedRenderer ir;
    assert(!ir.IsInitialized());

    ir.Init(5000);
    assert(ir.IsInitialized());
    assert(ir.MaxInstancesPerBatch() == 5000);
    assert(ir.BatchCount() == 0);
    assert(ir.DrawCallCount() == 0);

    ir.Shutdown();
    assert(!ir.IsInitialized());
    std::cout << "[PASS] test_instanced_init" << std::endl;
}

void test_instanced_create_batch() {
    InstancedRenderer ir;
    ir.Init(1000);

    uint32_t b1 = ir.CreateBatch(1, 10);
    uint32_t b2 = ir.CreateBatch(2, 20);
    uint32_t b3 = ir.CreateBatch(3, 30);
    assert(b1 != 0);
    assert(b2 != 0);
    assert(b3 != 0);
    assert(b1 != b2);
    assert(b2 != b3);
    assert(ir.BatchCount() == 3);

    // Verify batch contents
    const InstanceBatch* batch = ir.GetBatch(b1);
    assert(batch != nullptr);
    assert(batch->meshId == 1);
    assert(batch->materialId == 10);

    // Non-existent batch returns nullptr
    assert(ir.GetBatch(9999) == nullptr);

    ir.Shutdown();
    std::cout << "[PASS] test_instanced_create_batch" << std::endl;
}

void test_instanced_add_remove_instance() {
    InstancedRenderer ir;
    ir.Init(1000);
    uint32_t batchId = ir.CreateBatch(1, 1);

    InstanceData d1;
    d1.entityId = 100;
    d1.color[0] = 1.0f;
    uint32_t idx0 = ir.AddInstance(batchId, d1);
    assert(idx0 == 0);
    assert(ir.TotalInstanceCount() == 1);

    InstanceData d2;
    d2.entityId = 200;
    uint32_t idx1 = ir.AddInstance(batchId, d2);
    assert(idx1 == 1);
    assert(ir.TotalInstanceCount() == 2);

    InstanceData d3;
    d3.entityId = 300;
    ir.AddInstance(batchId, d3);
    assert(ir.TotalInstanceCount() == 3);

    // Remove middle instance
    ir.RemoveInstance(batchId, 1);
    assert(ir.TotalInstanceCount() == 2);

    // Remove first instance
    ir.RemoveInstance(batchId, 0);
    assert(ir.TotalInstanceCount() == 1);

    ir.Shutdown();
    std::cout << "[PASS] test_instanced_add_remove_instance" << std::endl;
}

void test_instanced_submit_draw_calls() {
    InstancedRenderer ir;
    ir.Init(1000);

    uint32_t b1 = ir.CreateBatch(1, 1);
    uint32_t b2 = ir.CreateBatch(2, 2);
    uint32_t b3 = ir.CreateBatch(3, 3);

    // Add instances to batches 1 and 2 (leave 3 empty)
    InstanceData d;
    d.entityId = 1;
    ir.AddInstance(b1, d);
    ir.AddInstance(b1, d);
    ir.AddInstance(b2, d);

    ir.SubmitAll();
    // Only non-empty batches generate draw calls (b1 and b2)
    assert(ir.DrawCallCount() == 2);

    // Add instance to b3 and submit again
    ir.AddInstance(b3, d);
    ir.SubmitAll();
    assert(ir.DrawCallCount() == 3);

    ir.Shutdown();
    std::cout << "[PASS] test_instanced_submit_draw_calls" << std::endl;
}

void test_instanced_clear_batch() {
    InstancedRenderer ir;
    ir.Init(1000);
    uint32_t batchId = ir.CreateBatch(1, 1);

    InstanceData d;
    d.entityId = 1;
    ir.AddInstance(batchId, d);
    ir.AddInstance(batchId, d);
    ir.AddInstance(batchId, d);
    assert(ir.TotalInstanceCount() == 3);

    ir.ClearBatch(batchId);
    assert(ir.TotalInstanceCount() == 0);

    // Batch still exists but is empty
    assert(ir.BatchCount() == 1);
    const InstanceBatch* batch = ir.GetBatch(batchId);
    assert(batch != nullptr);
    assert(batch->instances.empty());

    // SubmitAll should produce 0 draw calls for the empty batch
    ir.SubmitAll();
    assert(ir.DrawCallCount() == 0);

    ir.Shutdown();
    std::cout << "[PASS] test_instanced_clear_batch" << std::endl;
}

// ============================================================
// Spatial Hash tests
// ============================================================

void test_spatial_insert_query() {
    SpatialHash sh(10.0f);

    sh.Insert({1, 0.0f, 0.0f, 0.0f, 0.5f});
    sh.Insert({2, 5.0f, 0.0f, 0.0f, 0.5f});
    sh.Insert({3, 100.0f, 100.0f, 100.0f, 0.5f});
    assert(sh.EntityCount() == 3);

    // Query near origin with radius 6 should find entities 1 and 2
    auto results = sh.QueryRadius(0.0f, 0.0f, 0.0f, 6.0f);
    assert(results.size() == 2);
    bool found1 = false, found2 = false;
    for (uint32_t id : results) {
        if (id == 1) found1 = true;
        if (id == 2) found2 = true;
    }
    assert(found1 && found2);

    // Query near entity 3 should only find entity 3
    auto far = sh.QueryRadius(100.0f, 100.0f, 100.0f, 2.0f);
    assert(far.size() == 1);
    assert(far[0] == 3);
    std::cout << "[PASS] test_spatial_insert_query" << std::endl;
}

void test_spatial_remove() {
    SpatialHash sh(10.0f);
    sh.Insert({1, 0.0f, 0.0f, 0.0f, 0.5f});
    sh.Insert({2, 1.0f, 0.0f, 0.0f, 0.5f});
    assert(sh.EntityCount() == 2);

    auto before = sh.QueryRadius(0.0f, 0.0f, 0.0f, 5.0f);
    assert(before.size() == 2);

    sh.Remove(1);
    assert(sh.EntityCount() == 1);

    auto after = sh.QueryRadius(0.0f, 0.0f, 0.0f, 5.0f);
    assert(after.size() == 1);
    assert(after[0] == 2);

    // Removing a non-existent entity is safe
    sh.Remove(999);
    assert(sh.EntityCount() == 1);
    std::cout << "[PASS] test_spatial_remove" << std::endl;
}

void test_spatial_update() {
    SpatialHash sh(10.0f);
    sh.Insert({1, 0.0f, 0.0f, 0.0f, 0.5f});

    // Entity 1 is at origin, should be found near origin
    auto near = sh.QueryRadius(0.0f, 0.0f, 0.0f, 2.0f);
    assert(near.size() == 1);

    // Move entity 1 far away
    sh.Update(1, 500.0f, 500.0f, 500.0f);

    // Should no longer be found near origin
    near = sh.QueryRadius(0.0f, 0.0f, 0.0f, 2.0f);
    assert(near.size() == 0);

    // Should be found at new position
    auto farQuery = sh.QueryRadius(500.0f, 500.0f, 500.0f, 2.0f);
    assert(farQuery.size() == 1);
    assert(farQuery[0] == 1);

    // Entity count unchanged after update
    assert(sh.EntityCount() == 1);
    std::cout << "[PASS] test_spatial_update" << std::endl;
}

void test_spatial_aabb_query() {
    SpatialHash sh(5.0f);

    sh.Insert({1, 2.0f, 2.0f, 2.0f, 0.5f});
    sh.Insert({2, 8.0f, 8.0f, 8.0f, 0.5f});
    sh.Insert({3, 50.0f, 50.0f, 50.0f, 0.5f});

    // AABB covering (0,0,0)-(10,10,10) should find entities 1 and 2
    auto results = sh.QueryAABB(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);
    assert(results.size() == 2);
    bool found1 = false, found2 = false;
    for (uint32_t id : results) {
        if (id == 1) found1 = true;
        if (id == 2) found2 = true;
    }
    assert(found1 && found2);

    // Tight AABB around entity 3
    auto tight = sh.QueryAABB(49.0f, 49.0f, 49.0f, 51.0f, 51.0f, 51.0f);
    assert(tight.size() == 1);
    assert(tight[0] == 3);

    // AABB that contains nothing
    auto empty = sh.QueryAABB(-100.0f, -100.0f, -100.0f, -90.0f, -90.0f, -90.0f);
    assert(empty.empty());
    std::cout << "[PASS] test_spatial_aabb_query" << std::endl;
}

void test_spatial_nearest_neighbors() {
    SpatialHash sh(10.0f);

    sh.Insert({1, 0.0f, 0.0f, 0.0f, 0.0f});   // distance 0 from origin
    sh.Insert({2, 3.0f, 0.0f, 0.0f, 0.0f});   // distance 3
    sh.Insert({3, 10.0f, 0.0f, 0.0f, 0.0f});  // distance 10
    sh.Insert({4, 1.0f, 0.0f, 0.0f, 0.0f});   // distance 1
    sh.Insert({5, 20.0f, 0.0f, 0.0f, 0.0f});  // distance 20

    // Get 3 nearest to origin → ids 1 (d=0), 4 (d=1), 2 (d=3)
    auto nn = sh.GetNearestNeighbors(0.0f, 0.0f, 0.0f, 3);
    assert(nn.size() == 3);
    assert(nn[0] == 1);  // closest
    assert(nn[1] == 4);  // second closest
    assert(nn[2] == 2);  // third closest

    // Request more than exist
    auto all = sh.GetNearestNeighbors(0.0f, 0.0f, 0.0f, 100);
    assert(all.size() == 5);
    // Sorted by distance: 1, 4, 2, 3, 5
    assert(all[0] == 1);
    assert(all[4] == 5);

    // Request 0
    auto none = sh.GetNearestNeighbors(0.0f, 0.0f, 0.0f, 0);
    assert(none.empty());
    std::cout << "[PASS] test_spatial_nearest_neighbors" << std::endl;
}

void test_spatial_cell_count() {
    SpatialHash sh(10.0f);
    assert(sh.OccupiedCellCount() == 0);

    // Insert entity at origin with small radius → occupies 1 cell
    sh.Insert({1, 5.0f, 5.0f, 5.0f, 0.1f});
    uint32_t cellsAfterOne = sh.OccupiedCellCount();
    assert(cellsAfterOne >= 1);

    // Insert entity far away → occupies additional cell(s)
    sh.Insert({2, 100.0f, 100.0f, 100.0f, 0.1f});
    uint32_t cellsAfterTwo = sh.OccupiedCellCount();
    assert(cellsAfterTwo > cellsAfterOne);

    // Remove first entity → cell count should decrease
    sh.Remove(1);
    uint32_t cellsAfterRemove = sh.OccupiedCellCount();
    assert(cellsAfterRemove < cellsAfterTwo);

    // Clear everything
    sh.Clear();
    assert(sh.OccupiedCellCount() == 0);
    assert(sh.EntityCount() == 0);
    std::cout << "[PASS] test_spatial_cell_count" << std::endl;
}
