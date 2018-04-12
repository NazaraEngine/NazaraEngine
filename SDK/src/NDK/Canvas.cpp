// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Canvas.hpp>
#include <limits>

namespace Ndk
{
	void Canvas::ResizeToContent()
	{
	}

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

			if (m_keyboardOwner == lastEntryIndex)
				m_keyboardOwner = index;
		}

		m_widgetEntries.pop_back();
	}

	void Canvas::OnEventMouseButtonPressed(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(event.y - hoveredWidget.box.y));

			hoveredWidget.widget->OnMouseButtonPress(x, y, event.button);
		}
	}

	void Canvas::OnEventMouseButtonRelease(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent & event)
	{
		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(event.y - hoveredWidget.box.y));

			hoveredWidget.widget->OnMouseButtonRelease(x, y, event.button);
		}
	}

	void Canvas::OnEventMouseMoved(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		std::size_t bestEntry = InvalidCanvasIndex;
		float bestEntryArea = std::numeric_limits<float>::infinity();

		Nz::Vector3f mousePos(float(event.x), float(event.y), 0.f);
		for (std::size_t i = 0; i < m_widgetEntries.size(); ++i)
		{
			const Nz::Boxf& box = m_widgetEntries[i].box;

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
					m_cursorController->UpdateCursor(Nz::Cursor::Get(m_widgetEntries[m_hoveredWidget].cursor));
			}

			WidgetEntry& hoveredWidget = m_widgetEntries[m_hoveredWidget];

			int x = static_cast<int>(std::round(event.x - hoveredWidget.box.x));
			int y = static_cast<int>(std::round(event.y - hoveredWidget.box.y));
			hoveredWidget.widget->OnMouseMoved(x, y, event.deltaX, event.deltaY);
		}
		else if (m_hoveredWidget != InvalidCanvasIndex)
		{
			m_widgetEntries[m_hoveredWidget].widget->OnMouseExit();
			m_hoveredWidget = InvalidCanvasIndex;

			if (m_cursorController)
				m_cursorController->UpdateCursor(Nz::Cursor::Get(Nz::SystemCursor_Default));
		}
	}

	void Canvas::OnEventMouseLeft(const Nz::EventHandler* /*eventHandler*/)
	{
		if (m_hoveredWidget != InvalidCanvasIndex)
		{
			m_widgetEntries[m_hoveredWidget].widget->OnMouseExit();
			m_hoveredWidget = InvalidCanvasIndex;
		}
	}

	void Canvas::OnEventKeyPressed(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
		{
			if (m_widgetEntries[m_keyboardOwner].widget->OnKeyPressed(event))
				return;

			if (event.code == Nz::Keyboard::Tab)
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
	}

	void Canvas::OnEventKeyReleased(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnKeyReleased(event);
	}

	void Canvas::OnEventTextEntered(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::TextEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnTextEntered(event.character, event.repeated);
	}
}
