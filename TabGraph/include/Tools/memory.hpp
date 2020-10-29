#pragma once

#include <memory>

namespace tools
{

    template <class T>
    void construct_deleter(T* t)
    {
        if (!t->_construct_pself)
        {
            t->~T();
        }

        free(t);
    }

    template <class T, typename... Params>
    std::shared_ptr<T> make_shared(Params&&... args)
    {
        std::shared_ptr<T> rtn;
        T* t = (T*)calloc(1, sizeof(T));
        t->_construct_pself = reinterpret_cast<decltype(t->_construct_pself)>(&rtn);
        rtn.reset(t, construct_deleter<T>);
        t = new(t) T(std::forward<Params>(args)...);
        t->_construct_pself = nullptr;
        t->_construct_self = rtn;

        return rtn;
    }

    template <class T>
    class enable_shared_from_this
    {
    public:
        std::shared_ptr<T> *_construct_pself{ nullptr };
        std::weak_ptr<T> _construct_self;
        std::shared_ptr<T> shared_from_this()
        {
            if (_construct_pself)
            {
                return *static_cast<std::shared_ptr<T>*>(_construct_pself);
            }
            else
            {
                return _construct_self.lock();
            }
        }
    };
}