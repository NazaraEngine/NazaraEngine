#ifndef __KEYSTATE_HPP__
#define __KEYSTATE_HPP__

#include "BaseState.hpp"

class StateContext;

enum class KeyStatus
{
	Pressed,
	Released
};

class KeyState : public BaseState
{
	public:
		KeyState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;

	private:
		void DrawMenu() override;

		void ManageInput(KeyStatus isKeyPressed, const Nz::WindowEvent::KeyEvent& key, Ndk::StateMachine& fsm);

		KeyStatus m_keyStatus;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnKeyReleased, m_keyReleasedSlot);
};

#endif // __KEYSTATE_HPP__