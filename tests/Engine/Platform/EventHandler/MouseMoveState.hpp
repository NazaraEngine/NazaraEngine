#ifndef __MOUSEMOVESTATE_HPP__
#define __MOUSEMOVESTATE_HPP__

#include "BaseState.hpp"

class StateContext;

class MouseMoveState : public BaseState
{
	public:
		MouseMoveState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseMoved, m_mouseMovedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseWheelMoved, m_mouseWheelMovedSlot);
};

#endif // __MOUSEMOVESTATE_HPP__