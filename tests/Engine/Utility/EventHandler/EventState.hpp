#ifndef __EVENTSTATE_HPP__
#define __EVENTSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

#include <deque>

class StateContext;

class EventState : public Ndk::State
{
	public:
		EventState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void AddEvent(const Nz::WindowEvent& event);
		void DrawMenu();
		Nz::String ToString(const Nz::WindowEvent& event) const;

		StateContext& m_context;
		Text m_text;
		std::deque<Nz::String> m_events;
		int m_count;
		NazaraSlot(Nz::EventHandler, OnEvent, m_eventSlot);
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
};

#endif // __EVENTSTATE_HPP__