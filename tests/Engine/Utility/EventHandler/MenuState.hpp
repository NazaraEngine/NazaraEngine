#ifndef __MENUSTATE_HPP__
#define __MENUSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

class MenuState : public Ndk::State
{
	public:
		MenuState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();

		StateContext& m_context;
		Text m_text;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		int m_selectedNextState;
};

#endif // __MENUSTATE_HPP__
