#ifndef BASESTATE_HPP
#define BASESTATE_HPP

#include "Text.hpp"

#include <Nazara/Platform/EventHandler.hpp>

#include <NDK/State.hpp>

class StateContext;

class BaseState : public Ndk::State
{
	public:
		BaseState(StateContext& stateContext);
		virtual ~BaseState();

		virtual void Enter(Ndk::StateMachine& fsm) override;

		virtual void Leave(Ndk::StateMachine& fsm) override;

		virtual bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	protected:
		virtual void DrawMenu();

		StateContext& m_context;
		Text m_text;
};

#endif // BASESTATE_HPP