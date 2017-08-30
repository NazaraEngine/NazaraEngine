#ifndef __MOUSEENTERSTATE_HPP__
#define __MOUSEENTERSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

class MouseEnterState : public BaseState
{
	public:
		MouseEnterState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseEntered, m_mouseEnteredSlot);
		NazaraSlot(Nz::EventHandler, OnMouseLeft, m_mouseLeftSlot);
};

#endif // __MOUSEENTERSTATE_HPP__