#ifndef __STATEFACTORY_HPP__
#define __STATEFACTORY_HPP__

#include <NDK/State.hpp>

#include <map>
#include <memory>

class StateContext;

enum class EventStatus
{
	Min = 0,
	Menu,
	Event,
	Focus,
	Key,
	MouseClick,
	MouseEnter,
	MouseMove,
	TextEnter,
	WindowModification,
	Max = WindowModification
};

class StateFactory
{
	public:

		static std::shared_ptr<Ndk::State> Get(EventStatus state);
		static std::shared_ptr<Ndk::State> Get(unsigned int state);

		static bool Initialize(StateContext& stateContext);

		static void Uninitialize();

	private:

		static std::map<EventStatus, std::shared_ptr<Ndk::State>> s_states;
};

#endif // __STATEFACTORY_HPP__
