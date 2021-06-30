/*
* @Author: gpinchon
* @Date:   2021-06-26 12:38:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:01:48
*/

#pragma once

#include <Visitors/Visitor.hpp>

#include <string>
#include <typeindex>

namespace TabGraph::Core {
/**
 * @brief Use Curiously Recurring Template to generate proper type casting and Accept. Inspired by VulkanSceneGraph.
 * @tparam ParentClass : the class to inherit from
 * @tparam SubClass : the class that will inherit from ParentClass
*/
template <class ParentClass, class SubClass>
class Inherit : public ParentClass {
public:
    template <typename... Args>
    Inherit(Args&&... args)
        : ParentClass(args...)
    {
    }

    //Overriden functions
    /**
     * @return sizeof(SubClass)
    */
    inline size_t GetSize() const override
    {
        return sizeof(SubClass);
    }
    /**
     * @return the type_info of SubClass
    */
    inline const std::type_info& GetTypeInfo() const override
    {
        return typeid(SubClass);
    }
    /**
     * @return the name of the type_info of SubClass
    */
    inline std::string GetClassName() const override
    {
        return typeid(SubClass).name();
    }
    /**
     * @param typeIndex : the type to test
     * @return true if typeid(SubClass) == typeIndex
    */
    inline bool IsOfType(const std::type_index& typeIndex) const override
    {
        return typeIndex == typeid(SubClass);
    }
    /**
     * @brief Goes up the inheritances to find out if one of the parent class is of this type
     * @param typeIndex : the type to test
     * @return true if SubClass IsOfType(typeIndex), else checks if ParentClass is compatible
    */
    inline bool IsCompatible(const std::type_index& typeIndex) const override
    {
        return typeIndex == typeid(SubClass) ? true : ParentClass::IsCompatible(typeIndex);
    }
    /**
     * @brief calls operator() in Visitor, casting this to SubClass&
     * @param visitor : a Visitor to visit SubClass
    */
    inline void Accept(Visitors::Visitor& visitor) override
    {
        return visitor(static_cast<SubClass&>(*this));
    }
};
};
