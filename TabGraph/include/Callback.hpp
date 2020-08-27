#pragma once
#include <memory>
#include <functional>
#include <type_traits>

/** Callback wrapper */
template <typename Signature = void()>
class Callback
{
public:
    template<typename F, typename... Args>
    static auto Create(const F &callback, Args... args) {
        auto bindResult = std::bind(callback, args...);
        return std::shared_ptr<Callback<Signature>>(new Callback<Signature>(bindResult));
    }
    template<typename... Args>
    auto Call(Args... args) const { return _function(args...); };
    template<typename... Args>
    auto operator() (Args... args) const { return Call(args...); };
protected:
    template <typename Function>
    Callback(Function f) : _function(f) {};
private:
    std::function<Signature> _function;
};
