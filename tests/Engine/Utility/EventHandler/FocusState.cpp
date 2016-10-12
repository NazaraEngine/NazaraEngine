#include "FocusState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

FocusState::FocusState(StateContext& context) :
State(),
m_context(context),
m_text(context)
{
}

void FocusState::Enter(Ndk::StateMachine& fsm)
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

	m_gainedFocusSlot.Connect(eventHandler.OnGainedFocus, [&] (const Nz::EventHandler*)
	{
		m_text.SetContent("GAINED\nM for Menu");
	});

	m_lostFocusSlot.Connect(eventHandler.OnLostFocus, [&] (const Nz::EventHandler*)
	{
		m_text.SetContent("LOST\nM for Menu");
	});
}

void FocusState::Leave(Ndk::StateMachine& fsm)
{
	m_text.SetVisible(false);
}

bool FocusState::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
	return true;
}

void FocusState::DrawMenu()
{
	m_text.SetContent("Click outside the windows, this text should change !\nM for Menu");
}