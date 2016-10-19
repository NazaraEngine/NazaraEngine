#ifndef __MOUSEMOVESTATE_HPP__
#define __MOUSEMOVESTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

class MouseMoveState : public Ndk::State
{
	public:
		MouseMoveState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();

		StateContext& m_context;
		Text m_text;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseMoved, m_mouseMovedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseWheelMoved, m_mouseWheelMovedSlot);
};

#endif // __MOUSEMOVESTATE_HPP__