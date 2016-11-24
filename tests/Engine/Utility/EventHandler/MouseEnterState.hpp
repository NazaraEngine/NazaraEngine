#ifndef __MOUSEENTERSTATE_HPP__
#define __MOUSEENTERSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

class MouseEnterState : public Ndk::State
{
	public:
		MouseEnterState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();

		StateContext& m_context;
		Text m_text;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMouseEntered, m_mouseEnteredSlot);
		NazaraSlot(Nz::EventHandler, OnMouseLeft, m_mouseLeftSlot);
};

#endif // __MOUSEENTERSTATE_HPP__