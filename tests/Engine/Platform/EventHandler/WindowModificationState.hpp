#ifndef __WINDOWMODIFICATIONSTATE_HPP__
#define __WINDOWMODIFICATIONSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

class WindowModificationState : public BaseState
{
	public:
		WindowModificationState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMoved, m_movedSlot);
		NazaraSlot(Nz::EventHandler, OnResized, m_resizedSlot);
};

#endif // __WINDOWMODIFICATIONSTATE_HPP__