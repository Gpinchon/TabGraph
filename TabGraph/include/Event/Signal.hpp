#pragma once

#include <unordered_map>
#include <functional>
#include <memory>

/**
* Inspired by schneegans
* See : https://schneegans.github.io/tutorials/2015/09/20/signal-slot
*/

template <typename... Args>
class Signal
{
public:
	Signal() = default;
	~Signal() = default;

	auto Connect(std::function<void(Args...)> const& slot) {
		_slots[++_currentSlotID] = slot;
		return _currentSlotID;
	}

	template<typename T>
	auto ConnectMember(std::weak_ptr<T> inst, void (T::* func)(Args...)) {
		_slots[++_currentSlotID] = ([=](Args... args) {
			if (inst.lock() != nullptr)
				(inst.lock())->*func(args...);
			else
				Disconnect(_currentSlotID);
			});
		return _currentSlotID;
	}

	template<typename T>
	auto ConnectMember(std::weak_ptr<T> inst, void (T::* func)(Args...) const) {
		_slots[++_currentSlotID] = ([slotID = _currentSlotID, this](Args... args) {
			if (inst.lock() != nullptr)
				(inst.lock())->*func(args...);
			else
				this->Disconnect(_currentSlotID);
		});
		return _currentSlotID;
	}

	auto Disconnect(uint32_t slotID) {
		_slots.erase(slotID);
	}

	auto Emit(Args... args) const {
		for (const auto& slot : _slots)
			slot.second(args...);
	}

private:
	std::unordered_map<uint32_t, std::function<void(Args...)>> _slots;
	uint32_t _currentSlotID{ 0 };
};
