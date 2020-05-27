#include "MenuState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NazaraSDK/StateMachine.hpp>

MenuState::MenuState(StateContext& context) :
BaseState(context),
m_selectedNextState(-1)
{
}

void MenuState::Enter(Ndk::StateMachine& fsm)
{
	BaseState::Enter(fsm);

	Nz::EventHandler& eventHandler = m_context.window.GetEventHandler();
	m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, [this] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		if (key.virtualKey >= Nz::Keyboard::VKey::A && key.virtualKey < static_cast<Nz::Keyboard::VKey>(static_cast<int>(Nz::Keyboard::VKey::A) + static_cast<int>(EventStatus::Max) - 1))
		{
			m_selectedNextState = static_cast<int>(key.virtualKey) - static_cast<int>(Nz::Keyboard::VKey::A);
		}
	});
}

void MenuState::Leave(Ndk::StateMachine& fsm)
{
	BaseState::Leave(fsm);
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
