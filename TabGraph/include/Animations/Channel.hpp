#pragma once
#include <memory>

////////////////////////////////////////////////////////////////////////////////
//Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Node;
}

////////////////////////////////////////////////////////////////////////////////
//Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Animations {
struct Channel {
    enum class Path {
        None,
        Translation,
        Rotation,
        Scale,
        Weights
    };
    Path path { Path::None };
    std::shared_ptr<Nodes::Node> target;
    size_t samplerIndex { 0 };
};
}
