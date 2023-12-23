#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Sampler;
}

namespace TabGraph::SG {
class TextureSampler;
}

namespace TabGraph::Renderer {
using SamplerCacheKey = Tools::ObjectCacheKey<SG::TextureSampler*>;
using SamplerCache    = Tools::ObjectCache<SamplerCacheKey, std::shared_ptr<RAII::Sampler>>;
struct SamplerLoader : SamplerCache {
    std::shared_ptr<RAII::Sampler> operator()(Context& a_Context, SG::TextureSampler* a_Sampler);
};
}
