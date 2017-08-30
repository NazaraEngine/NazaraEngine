#ifndef __MENUSTATE_HPP__
#define __MENUSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

class MenuState : public BaseState
{
	public:
		MenuState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

		void Leave(Ndk::StateMachine& fsm) override;

		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();

		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		int m_selectedNextState;
};

#endif // __MENUSTATE_HPP__
