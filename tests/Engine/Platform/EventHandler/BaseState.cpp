#include "BaseState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

BaseState::BaseState(StateContext& context) :
State(),
m_context(context),
m_text(context)
{
}

BaseState::~BaseState()
{
}

void BaseState::Enter(Ndk::StateMachine& fsm)
{
	m_text.SetVisible(true);
	DrawMenu();
}

void BaseState::Leave(Ndk::StateMachine& /*fsm*/)
{
	m_text.SetVisible(false);
}

bool BaseState::Update(Ndk::StateMachine& /*fsm*/, float /*elapsedTime*/)
{
	return true;
}

void BaseState::DrawMenu()
{
	m_text.SetContent("This shouldn't be visible\nM for Menu");
}