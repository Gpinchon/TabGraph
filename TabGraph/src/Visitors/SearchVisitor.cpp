/*
* @Author: gpinchon
* @Date:   2021-06-20 13:29:09
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:29:56
*/
#include <Nodes/Node.hpp>
#include <Visitors/SearchVisitor.hpp>

namespace TabGraph::Visitors {
SearchVisitor::SearchVisitor(const std::string& name, const NodeVisitor::Mode& mode)
    : NodeVisitor(mode)
    , by(SearchVisitor::By::Name)
    , _functor([name](SearchVisitor& search, Core::Object& object) {
        return name == object.GetName();
    })
{
}
SearchVisitor::SearchVisitor(const uint32_t& id, const NodeVisitor::Mode& mode)
    : NodeVisitor(mode)
    , by(SearchVisitor::By::Id)
    , _functor([id](SearchVisitor& search, Core::Object& object) {
        return id == object.GetId();
    })
{
}
SearchVisitor::SearchVisitor(const std::type_info& type, const NodeVisitor::Mode& mode)
    : NodeVisitor(mode)
    , by(SearchVisitor::By::Type)
    , _functor([typeIndex = std::type_index(type)](SearchVisitor& search, Core::Object& object) {
        return object.IsCompatible(typeIndex);
    })
{
}
}