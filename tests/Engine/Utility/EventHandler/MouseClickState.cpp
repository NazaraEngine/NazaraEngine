#include "MouseClickState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

MouseClickState::MouseClickState(StateContext& context) :
State(),
m_context(context),
m_text(context)
{
}

void MouseClickState::Enter(Ndk::StateMachine& fsm)
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

	m_mouseButtonDoubleClickedSlot.Connect(eventHandler.OnMouseButtonDoubleClicked, [&] (const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& mouse)
	{
		ManageInput(MouseStatus::DoubleClick, mouse, fsm);
	});

	m_mouseButtonPressedSlot.Connect(eventHandler.OnMouseButtonPressed, [&] (const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& mouse)
	{
		ManageInput(MouseStatus::Pressed, mouse, fsm);
	});

	m_mouseButtonReleasedSlot.Connect(eventHandler.OnMouseButtonReleased, [&] (const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& mouse)
	{
		ManageInput(MouseStatus::Released, mouse, fsm);
	});
}

void MouseClickState::Leave(Ndk::StateMachine& /*fsm*/)
{
	m_text.SetVisible(false);
}

bool MouseClickState::Update(Ndk::StateMachine& /*fsm*/, float /*elapsedTime*/)
{
	return true;
}

void MouseClickState::DrawMenu()
{
	m_text.SetContent("Click in the windows, this text should change !\nM for Menu");
}

void MouseClickState::ManageInput(MouseStatus mouseStatus, const Nz::WindowEvent::MouseButtonEvent& mouse, Ndk::StateMachine& /*fsm*/)
{
	Nz::String content;
	if (mouseStatus == MouseStatus::Pressed)
		content = "Pressed: ";
	else if (mouseStatus == MouseStatus::Released)
		content = "Released: ";
	else
		content = "Double clicked: ";

	switch (mouse.button)
	{
		case Nz::Mouse::Left:
			content += "Left";
			break;
		case Nz::Mouse::Middle:
			content += "Middle";
			break;
		case Nz::Mouse::Right:
			content += "Right";
			break;
		case Nz::Mouse::XButton1:
			content += "XButton1";
			break;
		case Nz::Mouse::XButton2:
			content += "XButton2";
			break;
		default:
			content += "Unknown";
			break;
	}

	m_text.SetContent(content + "\nM for Menu");
}