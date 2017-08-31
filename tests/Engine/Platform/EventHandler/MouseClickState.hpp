#ifndef __MOUSECLICKSTATE_HPP__
#define __MOUSECLICKSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

enum class MouseStatus
{
	DoubleClick,
	Pressed,
	Released
};

class MouseClickState : public BaseState
{
	public:
		MouseClickState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		void ManageInput(MouseStatus mouseStatus, const Nz::WindowEvent::MouseButtonEvent& mouse, Ndk::StateMachine& fsm);

		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseButtonDoubleClicked, m_mouseButtonDoubleClickedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseButtonPressed, m_mouseButtonPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseButtonReleased, m_mouseButtonReleasedSlot);
};

#endif // __MOUSECLICKSTATE_HPP__