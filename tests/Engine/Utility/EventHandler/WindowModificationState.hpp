#ifndef __WINDOWMODIFICATIONSTATE_HPP__
#define __WINDOWMODIFICATIONSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

class WindowModificationState : public Ndk::State
{
	public:
		WindowModificationState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();

		StateContext& m_context;
		Text m_text;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnMoved, m_movedSlot);
		NazaraSlot(Nz::EventHandler, OnResized, m_resizedSlot);
};

#endif // __WINDOWMODIFICATIONSTATE_HPP__