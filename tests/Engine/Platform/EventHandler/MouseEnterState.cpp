#include "MouseEnterState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

MouseEnterState::MouseEnterState(StateContext& context) :
BaseState(context)
{
}

void MouseEnterState::Enter(Ndk::StateMachine& fsm)
{
	BaseState::Enter(fsm);

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

void MouseEnterState::DrawMenu()
{
	m_text.SetContent("Move your mouse outside the windows, this text should change !\nM for Menu");
}