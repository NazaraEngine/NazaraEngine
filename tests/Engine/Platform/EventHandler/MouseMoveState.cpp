#include "MouseMoveState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

MouseMoveState::MouseMoveState(StateContext& context) :
BaseState(context)
{
}

void MouseMoveState::Enter(Ndk::StateMachine& fsm)
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

	m_mouseMovedSlot.Connect(eventHandler.OnMouseMoved, [&] (const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		m_text.SetContent("Position(" + Nz::String::Number(event.x) + ", " + Nz::String::Number(event.y) + ") delta: (" + Nz::String::Number(event.deltaX) + ", " + Nz::String::Number(event.deltaY) + ") \nM for Menu");
	});

	m_mouseWheelMovedSlot.Connect(eventHandler.OnMouseWheelMoved, [&] (const Nz::EventHandler*, const Nz::WindowEvent::MouseWheelEvent& event)
	{
		m_text.SetContent("Wheel delta: " + Nz::String::Number(event.delta) + "\nM for Menu");
	});
}

void MouseMoveState::DrawMenu()
{
	m_text.SetContent("Move your mouse or your wheel, this text should change !\nM for Menu");
}