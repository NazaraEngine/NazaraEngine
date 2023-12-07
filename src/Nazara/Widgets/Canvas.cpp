// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/DefaultWidgetTheme.hpp>
#include <limits>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	Canvas::Canvas(entt::registry& registry, Nz::WindowEventHandler& eventHandler, Nz::CursorControllerHandle cursorController, UInt32 renderMask, int initialRenderLayer) :
	BaseWidget(std::make_shared<DefaultWidgetTheme>()),
	m_cursorController(cursorController),
	m_renderMask(renderMask),
	m_keyboardOwner(InvalidCanvasIndex),
	m_hoveredWidget(InvalidCanvasIndex),
	m_mouseOwner(InvalidCanvasIndex),
	m_registry(registry)
	{
		m_canvas = this;
		BaseWidget::m_registry = &m_registry;
		m_parentWidget = nullptr;

		SetBaseRenderLayer(initialRenderLayer);

		// Register ourselves as a widget to handle cursor change
		RegisterToCanvas();

		// Connect to every meaningful event
		m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, this, &Canvas::OnEventKeyPressed);
		m_keyReleasedSlot.Connect(eventHandler.OnKeyReleased, this, &Canvas::OnEventKeyReleased);
		m_mouseButtonPressedSlot.Connect(eventHandler.OnMouseButtonPressed, this, &Canvas::OnEventMouseButtonPressed);
		m_mouseButtonReleasedSlot.Connect(eventHandler.OnMouseButtonReleased, this, &Canvas::OnEventMouseButtonRelease);
		m_mouseEnteredSlot.Connect(eventHandler.OnMouseEntered, this, &Canvas::OnEventMouseEntered);
		m_mouseLeftSlot.Connect(eventHandler.OnMouseLeft, this, &Canvas::OnEventMouseLeft);
		m_mouseMovedSlot.Connect(eventHandler.OnMouseMoved, this, &Canvas::OnEventMouseMoved);
		m_mouseWheelMovedSlot.Connect(eventHandler.OnMouseWheelMoved, this, &Canvas::OnEventMouseWheelMoved);
		m_textEnteredSlot.Connect(eventHandler.OnTextEntered, this, &Canvas::OnEventTextEntered);
		m_textEditedSlot.Connect(eventHandler.OnTextEdited, this, &Canvas::OnEventTextEdited);
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

	template<typename F>
	bool Canvas::DispatchEvent(std::size_t widgetIndex, F&& functor)
	{
		for (;;)
		{
			WidgetEntry& targetWidget = m_widgetEntries[widgetIndex];
			if (functor(targetWidget))
				return true;

			if (!targetWidget.widget->m_parentWidget)
				return false;

			widgetIndex = targetWidget.widget->m_parentWidget->m_canvasIndex;
		}
	}

	void Canvas::OnEventMouseButtonPressed(const WindowEventHandler* eventHandler, const WindowEvent::MouseButtonEvent& event)
	{
		UpdateHoveredWidget(event.x, event.y);

		bool handled = false;
		if (std::size_t targetWidgetIndex = GetMouseEventTarget(); targetWidgetIndex != InvalidCanvasIndex)
		{
			handled = DispatchEvent(targetWidgetIndex, [&](WidgetEntry& widgetEntry)
			{
				int x = static_cast<int>(std::round(event.x - widgetEntry.box.x));
				int y = static_cast<int>(std::round(m_size.y - event.y - widgetEntry.box.y));

				if (event.clickCount == 2)
					return widgetEntry.widget->OnMouseButtonDoublePress(x, y, event.button);
				else if (event.clickCount == 3)
					return widgetEntry.widget->OnMouseButtonTriplePress(x, y, event.button);
				else
					return widgetEntry.widget->OnMouseButtonPress(x, y, event.button);
			});
		}

		SetMouseOwner(m_hoveredWidget);
		m_mouseOwnerButtons[event.button] = true;

		if (!handled)
			OnUnhandledMouseButtonPressed(eventHandler, event);
	}

	void Canvas::OnEventMouseButtonRelease(const WindowEventHandler* eventHandler, const WindowEvent::MouseButtonEvent& event)
	{
		bool handled = false;
		if (std::size_t targetWidgetIndex = GetMouseEventTarget(); targetWidgetIndex != InvalidCanvasIndex)
		{
			handled = DispatchEvent(targetWidgetIndex, [&](WidgetEntry& widgetEntry)
			{
				int x = static_cast<int>(std::round(event.x - widgetEntry.box.x));
				int y = static_cast<int>(std::round(m_size.y - event.y - widgetEntry.box.y));

				return widgetEntry.widget->OnMouseButtonRelease(x, y, event.button);
			});
		}

		m_mouseOwnerButtons[event.button] = false;
		if (m_mouseOwnerButtons.none())
			SetMouseOwner(InvalidCanvasIndex);

		UpdateHoveredWidget(event.x, event.y);

		if (!handled)
			OnUnhandledMouseButtonReleased(eventHandler, event);
	}

	void Canvas::OnEventMouseEntered(const WindowEventHandler* /*eventHandler*/)
	{
		// Keep previous mouse states but not new ones
		if (m_mouseOwner != InvalidCanvasIndex)
		{
			WidgetEntry& ownerWidget = m_widgetEntries[m_mouseOwner];

			for (std::size_t i = 0; i < Mouse::ButtonCount; ++i)
			{
				if (m_mouseOwnerButtons[i])
				{
					if (!Mouse::IsButtonPressed(static_cast<Mouse::Button>(i)))
					{
						ownerWidget.widget->OnMouseButtonRelease(-1, -1, static_cast<Mouse::Button>(i));
						m_mouseOwnerButtons[i] = false;
					}
				}
			}

			if (m_mouseOwnerButtons.none())
				SetMouseOwner(InvalidCanvasIndex);
		}
		else
			m_mouseOwnerButtons.reset();
	}

	void Canvas::OnEventMouseLeft(const WindowEventHandler* /*eventHandler*/)
	{
		if (std::size_t targetWidgetIndex = GetMouseEventTarget(); targetWidgetIndex != InvalidCanvasIndex)
		{
			m_widgetEntries[targetWidgetIndex].widget->OnMouseExit();
			m_hoveredWidget = InvalidCanvasIndex;
		}
	}

	void Canvas::OnEventMouseMoved(const WindowEventHandler* eventHandler, const WindowEvent::MouseMoveEvent& event)
	{
		// Don't update hovered widget while the user doesn't release its mouse
		UpdateHoveredWidget(event.x, event.y);

		if (std::size_t targetWidgetIndex = GetMouseEventTarget(); targetWidgetIndex != InvalidCanvasIndex)
		{
			WidgetEntry& targetWidget = m_widgetEntries[targetWidgetIndex];

			int x = static_cast<int>(std::round(event.x - targetWidget.box.x));
			int y = static_cast<int>(std::round(m_size.y - event.y - targetWidget.box.y));

			if (targetWidget.widget->OnMouseMoved(x, y, event.deltaX, -event.deltaY))
				return;
		}

		OnUnhandledMouseMoved(eventHandler, event);
	}

	void Canvas::OnEventMouseWheelMoved(const WindowEventHandler* eventHandler, const WindowEvent::MouseWheelEvent& event)
	{
		bool handled = false;
		if (std::size_t targetWidgetIndex = GetMouseEventTarget(); targetWidgetIndex != InvalidCanvasIndex)
		{
			handled = DispatchEvent(targetWidgetIndex, [&](WidgetEntry& widgetEntry)
			{
				int x = static_cast<int>(std::round(event.x - widgetEntry.box.x));
				int y = static_cast<int>(std::round(m_size.y - event.y - widgetEntry.box.y));

				return widgetEntry.widget->OnMouseWheelMoved(x, y, event.delta);
			});
		}

		if (!handled)
			OnUnhandledMouseWheelMoved(eventHandler, event);
	}

	void Canvas::OnEventKeyPressed(const WindowEventHandler* eventHandler, const WindowEvent::KeyEvent& event)
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

	void Canvas::OnEventKeyReleased(const WindowEventHandler* eventHandler, const WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
		{
			if (m_widgetEntries[m_keyboardOwner].widget->OnKeyReleased(event))
				return;
		}

		OnUnhandledKeyReleased(eventHandler, event);
	}

	void Canvas::OnEventTextEntered(const WindowEventHandler* /*eventHandler*/, const WindowEvent::TextEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnTextEntered(event.character, event.repeated);
	}

	void Canvas::OnEventTextEdited(const WindowEventHandler* /*eventHandler*/, const WindowEvent::EditEvent& event)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetEntries[m_keyboardOwner].widget->OnTextEdited(event.text, event.length);
	}

	void Canvas::UpdateHoveredWidget(int x, int y)
	{
		std::size_t bestEntry = InvalidCanvasIndex;
		float bestEntryArea = std::numeric_limits<float>::infinity();
		int bestEntryLayer = std::numeric_limits<int>::min();

		Vector3f mousePos(float(x), m_size.y - float(y), 0.f);
		for (std::size_t i = 0; i < m_widgetEntries.size(); ++i)
		{
			const Boxf& box = m_widgetEntries[i].box;
			int layer = m_widgetEntries[i].widget->GetBaseRenderLayer();

			if (box.Contains(mousePos))
			{
				float area = box.width * box.height;
				if (area < bestEntryArea && layer >= bestEntryLayer)
				{
					bestEntry = i;
					bestEntryArea = area;
					bestEntryLayer = layer;
				}
			}
		}

		// If we have a mouse owner, only allow to hover it or not
		if (m_mouseOwner != InvalidCanvasIndex && bestEntry != m_mouseOwner)
			bestEntry = InvalidCanvasIndex;

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

				// Only allow cursor update when not owning mouse
				if (m_cursorController && m_mouseOwner == InvalidCanvasIndex)
					m_cursorController->UpdateCursor(Cursor::Get(m_widgetEntries[m_hoveredWidget].cursor));
			}
		}
		else if (m_hoveredWidget != InvalidCanvasIndex)
		{
			m_widgetEntries[m_hoveredWidget].widget->OnMouseExit();
			m_hoveredWidget = InvalidCanvasIndex;

			// Only allow cursor update when not owning mouse
			if (m_cursorController && m_mouseOwner == InvalidCanvasIndex)
				m_cursorController->UpdateCursor(Cursor::Get(SystemCursor::Default));
		}
	}
}
