#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

#include <Types.glsl>

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct TextureTransform {
#ifdef __cplusplus
    TextureTransform()
        : offset(0, 0)
        , scale(1, 1)
        , rotation(0)
    {
    }
#endif //__cplusplus
    vec2 offset;
    vec2 scale;
    float rotation;
    uint _padding[3];
};

struct TextureInfo {
#ifdef __cplusplus
    TextureInfo()
        : handle(0)
        , texCoord(0)
    {
    }
#endif //__cplusplus
    TextureTransform transform;
    sampler2D handle;
    uint texCoord;
    uint _padding[1];
};

struct Sheen {
#ifdef __cplusplus
    Sheen()
        : colorFactor(1, 1, 1)
        , roughnessFactor(1)
    {
    }
#endif //__cplusplus
    TextureInfo colorTexture;
    TextureInfo roughnessTexture;
    vec3 colorFactor;
    float roughnessFactor;
    uint _padding[1];
};

struct NormalTexture {
#ifdef __cplusplus
    NormalTexture()
        : scale(1)
    {
    }
#endif //__cplusplus
    TextureInfo info;
    float scale;
};

struct OcclusionTexture {
#ifdef __cplusplus
    OcclusionTexture()
        : strength(1)
    {
    }
#endif //__cplusplus
    TextureInfo info;
    float strength;
};

struct BaseMaterial {
#ifdef __cplusplus
    BaseMaterial()
        : emissiveFactor(1, 1, 1)
        , alphaCutoff(0.5)
    {
    }
#endif //__cplusplus
    NormalTexture normalTexture;
    OcclusionTexture occlusionTexture;
    TextureInfo emissiveTexture;
    vec3 emissiveFactor;
    float alphaCutoff;
};

struct SpecularGlossinessMaterial {
#ifdef __cplusplus
    SpecularGlossinessMaterial()
        : diffuseFactor(1, 1, 1, 1)
        , specularFactor(1, 1, 1)
        , glossinessFactor(1)
    {
    }
#endif //__cplusplus
    BaseMaterial base;
    Sheen sheen;
    TextureInfo diffuseTexture;
    TextureInfo specularGlossinessTexture;
    vec4 diffuseFactor;
    vec3 specularFactor;
    float glossinessFactor;
    uint _padding[2];
};

struct MetallicRoughnessMaterial {
#ifdef __cplusplus
    MetallicRoughnessMaterial()
        : colorFactor(1, 1, 1, 1)
        , metallicFactor(0.5)
        , roughnessFactor(0.5)
    {
    }
#endif //__cplusplus
    BaseMaterial base;
    Sheen sheen;
    TextureInfo colorTexture;
    TextureInfo metallicRoughnessTexture;
    vec4 colorFactor;
    float metallicFactor;
    float roughnessFactor;
    uint _padding[2];
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MATERIAL_GLSL