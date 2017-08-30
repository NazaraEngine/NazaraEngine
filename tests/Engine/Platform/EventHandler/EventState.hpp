#ifndef __EVENTSTATE_HPP__
#define __EVENTSTATE_HPP__

#include "BaseState.hpp"

#include <deque>

class StateContext;

class EventState : public BaseState
{
	public:
		EventState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void AddEvent(const Nz::WindowEvent& event);

		void DrawMenu() override;

		Nz::String ToString(const Nz::WindowEvent& event) const;

		std::deque<Nz::String> m_events;
		int m_count;
		NazaraSlot(Nz::EventHandler, OnEvent, m_eventSlot);
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
};

#endif // __EVENTSTATE_HPP__