#include "MenuState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

MenuState::MenuState(StateContext& context) :
State(),
m_context(context),
m_text(context),
m_selectedNextState(-1)
{
}

void MenuState::Enter(Ndk::StateMachine& /*fsm*/)
{
	m_text.SetVisible(true);
	DrawMenu();

	Nz::EventHandler& eventHandler = m_context.window.GetEventHandler();
	m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, [this] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		if (key.code >= Nz::Keyboard::Key::A && key.code < (Nz::Keyboard::Key::A + static_cast<int>(EventStatus::Max) - 1))
		{
			m_selectedNextState = key.code - static_cast<int>(Nz::Keyboard::Key::A);
		}
	});
}

void MenuState::Leave(Ndk::StateMachine& /*fsm*/)
{
	m_text.SetVisible(false);
	m_selectedNextState = -1;
}

bool MenuState::Update(Ndk::StateMachine& fsm, float /*elapsedTime*/)
{
	if (m_selectedNextState != -1) {
		fsm.ChangeState(StateFactory::Get(m_selectedNextState + 1));
	}

	return true;
}

void MenuState::DrawMenu()
{
	m_text.SetContent("a. Event\nb. Focus\nc. Key\nd. Mouse click\ne. Mouse enter\nf. Mouse move\ng. Text enter\nh. Window modification");
}