/*
* @Author: gpinchon
* @Date:   2021-06-20 13:20:49
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:02:08
*/
#pragma once
#include <Visitors/NodeVisitor.hpp>

#include <functional>
#include <set>
#include <typeindex>
#include <variant>

namespace TabGraph::Visitors {
class SearchVisitor : public NodeVisitor {
public:
    using SearchFunctor = std::function<bool(SearchVisitor&, Core::Object&)>;
    using SearchResult = std::set<Core::Object*>;
    enum class By {
        Name,
        Pointer,
        Type,
        Id,
        Custom
    };
    const By by;
    /**
     * Builds a SearchVisitor to look by pointer
     * \param pointer : the address to look for
     * \param mode : the Mode of this NodeVisitor
     */
    SearchVisitor(const void* pointer, const NodeVisitor::Mode& mode)
        : NodeVisitor(mode)
        , by(SearchVisitor::By::Pointer)
        , _functor([pointer](SearchVisitor& search, Core::Object& object) {
            return pointer == &object;
        })
    {
    }
    /**
     * @brief 
     * @param functor 
     * @param mode 
    */
    SearchVisitor(const SearchFunctor& functor, const NodeVisitor::Mode& mode)
        : NodeVisitor(mode)
        , by(SearchVisitor::By::Custom)
        , _functor(functor)
    {
    }
    SearchVisitor(const std::string& name, const NodeVisitor::Mode& mode);
    SearchVisitor(const std::type_info& type, const NodeVisitor::Mode& mode);
    SearchVisitor(const uint32_t& id, const NodeVisitor::Mode& mode);
    inline const auto& GetResult() const
    {
        return _result;
    }

private:
    inline virtual void _Visit(Nodes::Node& node) override
    {
        if (_functor(*this, node))
            _result.insert(&node);
    }
    SearchFunctor _functor;
    SearchResult _result;
};
}
