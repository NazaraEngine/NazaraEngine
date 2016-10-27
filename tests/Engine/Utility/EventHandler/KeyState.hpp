#ifndef __KEYSTATE_HPP__
#define __KEYSTATE_HPP__

#include "Text.hpp"

#include <Nazara/Utility/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

enum class KeyStatus
{
	Pressed,
	Released
};

class KeyState : public Ndk::State
{
	public:
		KeyState(StateContext& stateContext);

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void DrawMenu();
		void ManageInput(KeyStatus isKeyPressed, const Nz::WindowEvent::KeyEvent& key, Ndk::StateMachine& fsm);

		StateContext& m_context;
		Text m_text;
		KeyStatus m_keyStatus;
		NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
		NazaraSlot(Nz::EventHandler, OnKeyReleased, m_keyReleasedSlot);
};

#endif // __KEYSTATE_HPP__