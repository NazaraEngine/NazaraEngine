#ifndef __TEXTENTERSTATE_HPP__
#define __TEXTENTERSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

class TextEnterState : public BaseState
{
	public:
		TextEnterState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnTextEntered, m_textEnteredSlot);
};

#endif // __TEXTENTERSTATE_HPP__