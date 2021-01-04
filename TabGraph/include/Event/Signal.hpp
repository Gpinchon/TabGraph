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
	using SlotID = uint32_t;
	class Connection {
	public:
		Slot(const Signal<Args...>* signal, SlotID slotNbr) : _signal(signal), _slotNbr(slotNbr) {}
		void Disconnect() {
			if (_signal != nullptr) {
				_signal->Disconnect(_slotNbr);
				_signal = nullptr;
			}
		};
	private:
		const Signal<Args...>* _signal{ nullptr };
		SlotID _slotNbr{ 0 };
	};
	Signal() = default;
	~Signal() = default;

	Signal(const Signal &) {}

	/**
	* @brief Connects a row functor to the signal
	* @return The slot ID
	*/
	auto Connect(std::function<void(Args...)> const& slot) {
		_slots[++_currentSlotID] = slot;
		return Signal::Slot(this, _currentSlotID);
	}

	/**
	* @brief Connects member method using smart ptr, disconnects if inst was destroyed
	* @argument inst : the class instance the functor will use
	* @argument func : the functor to connect
	* @return The slot ID
	*/
	template<typename T>
	auto ConnectMember(std::weak_ptr<T> inst, void (T::* func)(Args...)) {
		_slots[++_currentSlotID] = ([=](Args... args) {
			if (inst.lock() != nullptr)
				(inst.lock().get()->*func)(args...);
			else
				Disconnect(_currentSlotID);
			});
		return Signal::Slot(this, _currentSlotID);
	}

	/**
	* @brief const overload of ConnectMember
	*/
	template<typename T>
	auto ConnectMember(std::weak_ptr<T> inst, void (T::* func)(Args...) const) {
		_slots[++_currentSlotID] = ([slotID = _currentSlotID, this](Args... args) {
			if (inst.lock() != nullptr)
				(inst.lock())->*func(args...);
			else
				this->Disconnect(_currentSlotID);
			});
		return Signal::Slot(this, _currentSlotID);
	}

	/**
	* @brief Connect member using raw ptr, less safe if object gets destroyed
	* @argument inst : the class instance the functor will use
	* @argument func : the functor to connect
	* @return The slot ID
	*/
	template<typename T>
	auto ConnectMember(T* inst, void (T::* func)(Args...)) {
		return Connect([=](Args... args) {
			(inst->*func)(args...);
		});
	}

	/**
	* @brief const overload of ConnectMember
	*/
	template<typename T>
	auto ConnectMember(T* inst, void (T::* func)(Args...) const) {
		return Connect([=](Args... args) {
			(inst->*func)(args...);
			});
	}

	/**
	* @brief disconnects a slot from the signal
	*/
	auto Disconnect(uint32_t slotID) {
		_slots.erase(slotID);
	}

	/**
	* @brief Emits the signal and calls the functors connected to it
	*/
	auto Emit(Args... args) const {
		for (const auto& slot : _slots)
			slot.second(args...);
	}

private:
	//std::vector<Signal<Args...>::Slot*> _slots;
	std::unordered_map<SlotID, std::pair<std::function<void(Args...)>, Signal<Args...>::Slot>> _slots;
	SlotID _currentSlotID{ 0 };
};
