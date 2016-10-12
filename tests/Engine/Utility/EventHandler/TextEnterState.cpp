#include "TextEnterState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

TextEnterState::TextEnterState(StateContext& context) :
State(),
m_context(context),
m_text(context)
{
}

void TextEnterState::Enter(Ndk::StateMachine& fsm)
{
	m_text.SetVisible(true);
	DrawMenu();

	Nz::EventHandler& eventHandler = m_context.window.GetEventHandler();
	m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, [&] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		if (key.code == Nz::Keyboard::Key::M && key.shift)
		{
			fsm.ChangeState(StateFactory::Get(EventStatus::Menu));
		}
	});

	m_textEnteredSlot.Connect(eventHandler.OnTextEntered, [&] (const Nz::EventHandler*, const Nz::WindowEvent::TextEvent& event)
	{
		Nz::String content = "Character: " + Nz::String::Unicode(event.character);
		if (event.repeated)
			content += " repeated";
		m_text.SetContent(content + "\nM for Menu");
	});
}

void TextEnterState::Leave(Ndk::StateMachine& fsm)
{
	m_text.SetVisible(false);
}

bool TextEnterState::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
	return true;
}

void TextEnterState::DrawMenu()
{
	m_text.SetContent("Enter some text, this text should change !\nM for Menu");
}