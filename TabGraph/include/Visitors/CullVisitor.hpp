/*
* @Author: gpinchon
* @Date:   2021-06-20 13:22:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:02:04
*/
#pragma once
#include <Renderer/Renderer.hpp>
#include <Visitors/NodeVisitor.hpp>

#include <functional>
#include <typeindex>
#include <variant>
#include <vector>

namespace TabGraph::Nodes {
class Renderable;
};

namespace TabGraph::Visitors {
class CullVisitor : public NodeVisitor {
public:
    CullVisitor(const Renderer::Options& options, const NodeVisitor::Mode& mode)
        : NodeVisitor(mode)
        , _options(options)
    {
    }
    inline const auto& GetOptions() const
    {
        return _options;
    }
    inline const auto& GetResult() const
    {
        return _result;
    }
    
private:
    const Renderer::Options _options;
    std::vector<Renderer::ShapeState> _result;

    // Hérité via NodeVisitor
    virtual void _Visit(Nodes::Node& node) override;
    virtual void _Visit(Nodes::Group& node) override;
    virtual void _Visit(Nodes::Renderable& node) override;
};
};