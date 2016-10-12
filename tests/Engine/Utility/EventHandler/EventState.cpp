#include "EventState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/StateMachine.hpp>

EventState::EventState(StateContext& context) :
State(),
m_context(context),
m_text(context),
m_count(0)
{
}

void EventState::Enter(Ndk::StateMachine& fsm)
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

	m_eventSlot.Connect(eventHandler.OnEvent, [&] (const Nz::EventHandler*, const Nz::WindowEvent& event)
	{
		AddEvent(event);
		++m_count;
	});
}

void EventState::Leave(Ndk::StateMachine& fsm)
{
	m_text.SetVisible(false);
}

bool EventState::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
	return true;
}

void EventState::AddEvent(const Nz::WindowEvent& event)
{
	if (m_events.size() > 9)
		m_events.pop_front();

	m_events.push_back(Nz::String::Number(m_count) + " - " + ToString(event));

	Nz::String content;
	for (auto&& event : m_events)
	{
		content += event + "\n";
	}
	content += "\nM for Menu";
	m_text.SetContent(content, 36);
}

void EventState::DrawMenu()
{
	m_text.SetContent("Do whathever you want, this text should change !\nM for Menu");
}

Nz::String EventState::ToString(const Nz::WindowEvent& event) const
{
	switch (event.type)
	{
		case Nz::WindowEventType_GainedFocus:
			return "WindowEventType_GainedFocus";
		case Nz::WindowEventType_LostFocus:
			return "WindowEventType_LostFocus";
		case Nz::WindowEventType_KeyPressed:
			return "WindowEventType_KeyPressed";
		case Nz::WindowEventType_KeyReleased:
			return "WindowEventType_KeyReleased";
		case Nz::WindowEventType_MouseButtonDoubleClicked:
			return "WindowEventType_MouseButtonDoubleClicked";
		case Nz::WindowEventType_MouseButtonPressed:
			return "WindowEventType_MouseButtonPressed";
		case Nz::WindowEventType_MouseButtonReleased:
			return "WindowEventType_MouseButtonReleased";
		case Nz::WindowEventType_MouseEntered:
			return "WindowEventType_MouseEntered";
		case Nz::WindowEventType_MouseLeft:
			return "WindowEventType_MouseLeft";
		case Nz::WindowEventType_MouseMoved:
			return "WindowEventType_MouseMoved";
		case Nz::WindowEventType_MouseWheelMoved:
			return "WindowEventType_MouseWheelMoved";
		case Nz::WindowEventType_Moved:
			return "WindowEventType_Moved";
		case Nz::WindowEventType_Quit:
			return "WindowEventType_Quit";
		case Nz::WindowEventType_Resized:
			return "WindowEventType_Resized";
		case Nz::WindowEventType_TextEntered:
			return "WindowEventType_TextEntered";
		default:
			return "Not handled";
	}
}