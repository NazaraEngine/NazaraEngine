#ifndef __FOCUSSTATE_HPP__
#define __FOCUSSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

class FocusState : public Ndk::State
{
	public:
		FocusState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();

		StateContext& m_context;
		Text m_text;
		NazaraSlot(Nz::EventHandler, OnGainedFocus, m_gainedFocusSlot);
		NazaraSlot(Nz::EventHandler, OnLostFocus, m_lostFocusSlot);
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
};

#endif // __FOCUSSTATE_HPP__