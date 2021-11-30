// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/EventHandler.hpp>
#include <memory>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	inline EventHandler::EventHandler(const EventHandler& other) :
	HandledObject(other)
	{
	}

	inline void EventHandler::Dispatch(const WindowEvent& event)
	{
		OnEvent(this, event);

		switch (event.type)
		{
			case WindowEventType::GainedFocus:
				OnGainedFocus(this);
				break;

			case WindowEventType::KeyPressed:
				OnKeyPressed(this, event.key);
				break;

			case WindowEventType::KeyReleased:
				OnKeyReleased(this, event.key);
				break;

			case WindowEventType::LostFocus:
				OnLostFocus(this);
				break;

			case WindowEventType::MouseButtonPressed:
				OnMouseButtonPressed(this, event.mouseButton);
				break;

			case WindowEventType::MouseButtonReleased:
				OnMouseButtonReleased(this, event.mouseButton);
				break;

			case WindowEventType::MouseEntered:
				OnMouseEntered(this);
				break;

			case WindowEventType::MouseLeft:
				OnMouseLeft(this);
				break;

			case WindowEventType::MouseMoved:
				OnMouseMoved(this, event.mouseMove);
				break;

			case WindowEventType::MouseWheelMoved:
				OnMouseWheelMoved(this, event.mouseWheel);
				break;

			case WindowEventType::Moved:
				OnMoved(this, event.position);
				break;

			case WindowEventType::Quit:
				OnQuit(this);
				break;

			case WindowEventType::Resized:
				OnResized(this, event.size);
				break;

			case WindowEventType::TextEntered:
				OnTextEntered(this, event.text);
				break;

			case WindowEventType::TextEdited:
				OnTextEdited(this, event.edit);
				break;
		}
	}
}

#include <Nazara/Platform/DebugOff.hpp>
