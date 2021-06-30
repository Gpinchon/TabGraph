#pragma once

#include <Visitors/Visitor.hpp>

#include <string>
#include <typeindex>

////////////////////////////////////////////////////////////////////////////////
//Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Core {
class Object {
public:
    Object();
    Object(const std::string& name)
        : Object()
    {
        _name = name;
    }
    Object(const Object& other)
        : Object(other._name)
    {
    }
    inline auto GetId() const
    {
        return _id;
    }
    inline auto GetName() const
    {
        return _name;
    }

    inline void SetId(uint32_t id)
    {
        _id = id;
    }
    inline void SetName(const std::string& name)
    {
        _name = name;
    }

    /**
     * @brief see Inherit
    */
    inline virtual size_t GetSize() const
    {
        return sizeof(Object);
    }
    /**
     * @brief see Inherit
    */
    inline virtual const std::type_info& GetTypeInfo() const
    {
        return typeid(Object);
    }
    /**
     * @brief see Inherit
    */
    inline virtual std::string GetClassName() const
    {
        return typeid(Object).name();
    }
    /**
     * @brief see Inherit
    */
    inline virtual bool IsOfType(const std::type_index& typeIndex) const
    {
        return typeIndex == typeid(Object);
    }
    /**
     * @brief see Inherit
    */
    inline virtual bool IsCompatible(const std::type_index& typeIndex) const
    {
        return IsOfType(typeIndex);
    }
    /**
     * @brief see Inherit
    */
    inline virtual void Accept(Visitors::Visitor& visitor)
    {
        return visitor(*this);
    }

private:
    uint32_t _id { 0 };
    std::string _name { "" };
};
};
