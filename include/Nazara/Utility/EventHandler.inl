// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/EventHandler.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline EventHandler::EventHandler(const EventHandler&)
	{
	}

	inline void EventHandler::Dispatch(const WindowEvent& event)
	{
		OnEvent(this, event);

		switch (event.type)
		{
			case WindowEventType_GainedFocus:
				OnGainedFocus(this);
				break;

			case WindowEventType_KeyPressed:
				OnKeyPressed(this, event.key);
				break;

			case WindowEventType_KeyReleased:
				OnKeyReleased(this, event.key);
				break;

			case WindowEventType_LostFocus:
				OnLostFocus(this);
				break;

			case WindowEventType_MouseButtonDoubleClicked:
				OnMouseButtonDoubleClicked(this, event.mouseButton);
				break;

			case WindowEventType_MouseButtonPressed:
				OnMouseButtonPressed(this, event.mouseButton);
				break;

			case WindowEventType_MouseButtonReleased:
				OnMouseButtonReleased(this, event.mouseButton);
				break;

			case WindowEventType_MouseEntered:
				OnMouseEntered(this);
				break;

			case WindowEventType_MouseLeft:
				OnMouseLeft(this);
				break;

			case WindowEventType_MouseMoved:
				OnMouseMoved(this, event.mouseMove);
				break;

			case WindowEventType_MouseWheelMoved:
				OnMouseWheelMoved(this, event.mouseWheel);
				break;

			case WindowEventType_Moved:
				OnMoved(this, event.position);
				break;

			case WindowEventType_Quit:
				OnQuit(this);
				break;

			case WindowEventType_Resized:
				OnResized(this, event.size);
				break;

			case WindowEventType_TextEntered:
				OnTextEntered(this, event.text);
				break;
		}
	}
}

#include <Nazara/Utility/DebugOff.hpp>
