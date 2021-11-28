// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Canvas.hpp>
#include <limits>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	std::size_t Canvas::RegisterWidget(BaseWidget* widget)
	{
		WidgetEntry box;
		box.cursor = widget->GetCursor();
		box.widget = widget;

		std::size_t index = m_widgetEntries.size();
		m_widgetEntries.emplace_back(box);

		NotifyWidgetBoxUpdate(index);
		return index;
	}

	void Canvas::UnregisterWidget(std::size_t index)
	{
		WidgetEntry& entry = m_widgetEntries[index];

		if (m_hoveredWidget == index)
			m_hoveredWidget = InvalidCanvasIndex;

		if (m_mouseOwner == index)
		{
			m_mouseOwner = InvalidCanvasIndex;
			m_mouseOwnerButtons.reset();
		}

		if (m_keyboardOwner == index)
			m_keyboardOwner = InvalidCanvasIndex;

		if (m_widgetEntries.size() > 1U)
		{
			WidgetEntry& lastEntry = m_widgetEntries.back();
			std::size_t lastEntryIndex = m_widgetEntries.size() - 1;

			entry = std::move(lastEntry);
			entry.widget->UpdateCanvasIndex(index);

			if (m_hoveredWidget == lastEntryIndex)
				m_hoveredWidget = index;

			if (m_mouseOwner == lastEntryIndex)
				m_mouseOwner = index;

			if (m_keyboardOwner == lastEntryIndex)
				m_keyboardOwner = index;
		}

		m_widgetEntries.pop_back();
	}

	void Canvas::OnEventMouseButtonPressed(const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& event)
	{
		UpdateHoveredWidget(event.x, event.y);

		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(m_size.y - event.y - hoveredWidget.box.y));

			hoveredWidget.widget->OnMouseButtonPress(x, y, event.button);
		}

		SetMouseOwner(m_hoveredWidget);
		m_mouseOwnerButtons[event.button] = true;
	}

	void Canvas::OnEventMouseButtonRelease(const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& event)
	{
		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(m_size.y - event.y - hoveredWidget.box.y));

			hoveredWidget.widget->OnMouseButtonRelease(x, y, event.button);
		}

		m_mouseOwnerButtons[event.button] = false;
		if (m_mouseOwnerButtons.none())
			SetMouseOwner(InvalidCanvasIndex);

		UpdateHoveredWidget(event.x, event.y);
	}

	void Canvas::OnEventMouseEntered(const EventHandler* eventHandler)
	{
		// Keep previous mouse states but not new ones
		for (std::size_t i = 0; i < Mouse::ButtonCount; ++i)
			m_mouseOwnerButtons[i] = m_mouseOwnerButtons[i] && Mouse::IsButtonPressed(static_cast<Mouse::Button>(i));

		if (m_mouseOwnerButtons.none())
			SetMouseOwner(InvalidCanvasIndex);
	}

	void Canvas::OnEventMouseLeft(const EventHandler* /*eventHandler*/)
	{
		if (m_hoveredWidget != InvalidCanvasIndex && m_mouseOwner == InvalidCanvasIndex)
		{
			m_widgetEntries[m_hoveredWidget].widget->OnMouseExit();
			m_hoveredWidget = InvalidCanvasIndex;
		}
	}

	void Canvas::OnEventMouseMoved(const EventHandler* /*eventHandler*/, const WindowEvent::MouseMoveEvent& event)
	{
		// Don't update hovered widget while the user doesn't release its mouse
		UpdateHoveredWidget(event.x, event.y);

		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(m_size.y - event.y - hoveredWidget.box.y));

			hoveredWidget.widget->OnMouseMoved(x, y, event.deltaX, -event.deltaY);
		}
	}

	void Canvas::OnEventMouseWheelMoved(const EventHandler* /*eventHandler*/, const WindowEvent::MouseWheelEvent& event)
	{
		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(m_size.y - event.y - hoveredWidget.box.y));

			hoveredWidget.widget->OnMouseWheelMoved(x, y, event.delta);
		}
	}

	void Canvas::OnEventKeyPressed(const EventHandler* eventHandler, const WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
		{
			if (m_widgetEntries[m_keyboardOwner].widget->OnKeyPressed(event))
				return;

			if (event.virtualKey == Keyboard::VKey::Tab)
			{
				if (!event.shift)
				{
					// Forward
					for (std::size_t i = m_keyboardOwner + 1; i < m_widgetEntries.size(); ++i)
					{
						if (m_widgetEntries[i].widget->IsFocusable())
						{
							SetKeyboardOwner(i);
							return;
						}
					}

					for (std::size_t i = 0; i < m_keyboardOwner; ++i)
					{
						if (m_widgetEntries[i].widget->IsFocusable())
						{
							SetKeyboardOwner(i);
							return;
						}
					}
				}
				else
				{
					// Backward
					for (decltype(m_widgetEntries)::reverse_iterator rit{ m_widgetEntries.begin() + m_keyboardOwner }; rit != m_widgetEntries.rend(); ++rit)
					{
						if (rit->widget->IsFocusable())
						{
							SetKeyboardOwner(std::distance(m_widgetEntries.begin(), rit.base()) - 1);
							return;
						}
					}

					decltype(m_widgetEntries)::reverse_iterator rend { m_widgetEntries.begin() + m_keyboardOwner };
					for (auto rit = m_widgetEntries.rbegin(); rit != rend; ++rit)
					{
						if (rit->widget->IsFocusable())
						{
							SetKeyboardOwner(std::distance(m_widgetEntries.begin(), rit.base()) - 1);
							return;
						}
					}
				}
			}
		}

		OnUnhandledKeyPressed(eventHandler, event);
	}

	void Canvas::OnEventKeyReleased(const EventHandler* eventHandler, const WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnKeyReleased(event);

		OnUnhandledKeyReleased(eventHandler, event);
	}

	void Canvas::OnEventTextEntered(const EventHandler* /*eventHandler*/, const WindowEvent::TextEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnTextEntered(event.character, event.repeated);
	}

	void Canvas::OnEventTextEdited(const EventHandler* /*eventHandler*/, const WindowEvent::EditEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnTextEdited(event.text, event.length);
	}

	void Canvas::UpdateHoveredWidget(int x, int y)
	{
		if (m_mouseOwner != InvalidCanvasIndex)
			return;

		std::size_t bestEntry = InvalidCanvasIndex;
		float bestEntryArea = std::numeric_limits<float>::infinity();

		Vector3f mousePos(float(x), m_size.y - float(y), 0.f);
		for (std::size_t i = 0; i < m_widgetEntries.size(); ++i)
		{
			const Boxf& box = m_widgetEntries[i].box;

			if (box.Contains(mousePos))
			{
				float area = box.width * box.height;
				if (area < bestEntryArea)
				{
					bestEntry = i;
					bestEntryArea = area;
				}
			}
		}

		if (bestEntry != InvalidCanvasIndex)
		{
			if (m_hoveredWidget != bestEntry)
			{
				if (m_hoveredWidget != InvalidCanvasIndex)
				{
					WidgetEntry& previouslyHovered = m_widgetEntries[m_hoveredWidget];
					previouslyHovered.widget->OnMouseExit();
				}

				m_hoveredWidget = bestEntry;
				m_widgetEntries[m_hoveredWidget].widget->OnMouseEnter();

				if (m_cursorController)
					m_cursorController->UpdateCursor(Cursor::Get(m_widgetEntries[m_hoveredWidget].cursor));
			}
		}
		else if (m_hoveredWidget != InvalidCanvasIndex)
		{
			m_widgetEntries[m_hoveredWidget].widget->OnMouseExit();
			m_hoveredWidget = InvalidCanvasIndex;

			if (m_cursorController)
				m_cursorController->UpdateCursor(Cursor::Get(SystemCursor::Default));
		}
	}
}
