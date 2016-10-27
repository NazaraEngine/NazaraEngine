#ifndef __MOUSECLICKSTATE_HPP__
#define __MOUSECLICKSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

enum class MouseStatus
{
	DoubleClick,
	Pressed,
	Released
};

class MouseClickState : public Ndk::State
{
	public:
		MouseClickState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();
		void ManageInput(MouseStatus mouseStatus, const Nz::WindowEvent::MouseButtonEvent& mouse, Ndk::StateMachine& fsm);

		StateContext& m_context;
		Text m_text;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseButtonDoubleClicked, m_mouseButtonDoubleClickedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseButtonPressed, m_mouseButtonPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseButtonReleased, m_mouseButtonReleasedSlot);
};

#endif // __MOUSECLICKSTATE_HPP__