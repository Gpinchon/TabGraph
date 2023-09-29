#pragma once

namespace TabGraph::Renderer::RAII {
struct Sampler {
    Sampler();
    ~Sampler();
    operator unsigned() const { return handle; }
    unsigned handle = 0;
};
}