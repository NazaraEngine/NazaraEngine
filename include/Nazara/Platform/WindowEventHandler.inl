// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <memory>

namespace Nz
{
	inline WindowEventHandler::WindowEventHandler(const WindowEventHandler& other) :
	HandledObject(other)
	{
	}

	inline void WindowEventHandler::Dispatch(const WindowEvent& event)
	{
		OnEvent(this, event);

		switch (event.type)
		{
			case WindowEventType::Created:
				OnCreated(this);
				break;

			case WindowEventType::Destruction:
				OnDestruction(this);
				break;

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

			case WindowEventType::Minimized:
				OnMinimized(this);
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

			case WindowEventType::Restored:
				OnRestored(this);
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

