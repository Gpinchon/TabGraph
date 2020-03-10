#pragma once
#include <memory>
#include <functional>

/** Callback wrapper */
class Callback
{
public:
    static std::shared_ptr<Callback> Create(std::function<void()>);
    template<typename F, typename T>
    static std::shared_ptr<Callback> Create(const F &callback, const T &argument);
    void operator()();
protected:
    Callback() = default;
private:
    std::function<void()> _function;
};

template<typename F, typename T>
std::shared_ptr<Callback> Callback::Create(const F &callback, const T &argument)
{
	std::shared_ptr<Callback> newCallback(new Callback());
	newCallback->_function = std::bind(callback, argument);
    return newCallback;
}