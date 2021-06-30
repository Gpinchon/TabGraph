#pragma once

#include <Visitors/Visitor.hpp>

namespace TabGraph::Visitors {
class NodeVisitor : public Visitor {
public:
    enum class Mode {
        VisitOnce, //Visit only one node
        VisitParents, //Visit parents until we encounter a node with no parent
        VisitChildren //Visit children until we encounter a node with no children
    };
    NodeVisitor(const Mode& a_Mode)
        : mode(a_Mode)
    {
    }
    const Mode mode;
    //Using NVI as this might be faster in some case and to enforce Visit->Traverse order
    virtual void operator()(Nodes::Node& node) final
    {
        _Visit(node);
        _Traverse(node);
    };

private:
    virtual void _Visit(Nodes::Node& node) = 0;
    void _Traverse(Nodes::Node& node);
};
};
