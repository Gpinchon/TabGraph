/*
* @Author: gpinchon
* @Date:   2021-06-26 00:56:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:45
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Core {
    class Object;
};
namespace Nodes {
    class Node;
    class Group;
    class Renderable;
};
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Visitors {
class Visitor {
public:
    virtual void operator()(Core::Object&) {};
    virtual void operator()(Nodes::Node&) {};
    virtual void operator()(Nodes::Group&) {};
    virtual void operator()(Nodes::Renderable&) {};
};
};