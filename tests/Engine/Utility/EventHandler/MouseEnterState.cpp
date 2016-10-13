#include "MouseEnterState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

MouseEnterState::MouseEnterState(StateContext& context) :
State(),
m_context(context),
m_text(context)
{
}

void MouseEnterState::Enter(Ndk::StateMachine& fsm)
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

	m_mouseEnteredSlot.Connect(eventHandler.OnMouseEntered, [&] (const Nz::EventHandler*)
	{
		m_text.SetContent("Entered\nM for Menu");
	});

	m_mouseLeftSlot.Connect(eventHandler.OnMouseLeft, [&] (const Nz::EventHandler*)
	{
		m_text.SetContent("Left\nM for Menu");
	});
}

void MouseEnterState::Leave(Ndk::StateMachine& fsm)
{
	m_text.SetVisible(false);
}

bool MouseEnterState::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
	return true;
}

void MouseEnterState::DrawMenu()
{
	m_text.SetContent("Move your mouse outside the windows, this text should change !\nM for Menu");
}