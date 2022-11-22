#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Visitor/NodeVisitor.hpp>

#include <ostream>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Node;
class NodeGroup;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class SerializeVisitor : public SG::NodeVisitor {
public:
    SerializeVisitor(std::ostream& a_Ostream, const Mode& a_Mode)
        : SG::NodeVisitor(a_Mode)
        , _ostream(a_Ostream)
    {}

private:
    virtual void _Visit(SG::Object& node) override final;
    virtual void _Visit(SG::Node& a_Node) override final;
    virtual void _Visit(SG::NodeGroup& a_NodeGroup) override final;
    std::ostream& _ostream;
};
}