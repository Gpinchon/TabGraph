#include "Callback.hpp"

std::shared_ptr<Callback> Callback::Create(std::function<void()> function)
{
	std::shared_ptr<Callback> newCallback(new Callback());
	newCallback->_function = function;
	return newCallback;
}

 void Callback::operator()()
 {
 	_function();
 }