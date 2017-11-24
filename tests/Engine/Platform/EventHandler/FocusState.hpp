#ifndef __FOCUSSTATE_HPP__
#define __FOCUSSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

class FocusState : public BaseState
{
	public:
		FocusState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		NazaraSlot(Nz::EventHandler, OnGainedFocus, m_gainedFocusSlot);
		NazaraSlot(Nz::EventHandler, OnLostFocus, m_lostFocusSlot);
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
};

#endif // __FOCUSSTATE_HPP__