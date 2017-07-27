// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Canvas.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>
#include <limits>

namespace Ndk
{
	void Canvas::ResizeToContent()
	{
	}

	std::size_t Canvas::RegisterWidget(BaseWidget* widget)
	{
		WidgetBox box;
		box.cursor = widget->GetCursor();
		box.widget = widget;

		std::size_t index = m_widgetBoxes.size();
		m_widgetBoxes.emplace_back(box);

		NotifyWidgetBoxUpdate(index);
		return index;
	}

	void Canvas::UnregisterWidget(std::size_t index)
	{
		WidgetBox& entry = m_widgetBoxes[index];

		if (m_hoveredWidget == &entry)
			m_hoveredWidget = nullptr;

		if (m_keyboardOwner == entry.widget)
			m_keyboardOwner = nullptr;

		if (m_widgetBoxes.size() > 1U)
		{
			WidgetBox& lastEntry = m_widgetBoxes.back();

			entry = std::move(lastEntry);
			entry.widget->UpdateCanvasIndex(index);
		}

		m_widgetBoxes.pop_back();
	}

	void Canvas::OnEventMouseButtonPressed(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		if (m_hoveredWidget)
		{
			int x = static_cast<int>(std::round(event.x - m_hoveredWidget->box.x));
			int y = static_cast<int>(std::round(event.y - m_hoveredWidget->box.y));

			m_hoveredWidget->widget->OnMouseButtonPress(x, y, event.button);
		}
	}

	void Canvas::OnEventMouseButtonRelease(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseButtonEvent & event)
	{
		if (m_hoveredWidget)
		{
			int x = static_cast<int>(std::round(event.x - m_hoveredWidget->box.x));
			int y = static_cast<int>(std::round(event.y - m_hoveredWidget->box.y));

			m_hoveredWidget->widget->OnMouseButtonRelease(x, y, event.button);
		}
	}

	void Canvas::OnEventMouseMoved(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		const WidgetBox* bestEntry = nullptr;
		float bestEntryArea = std::numeric_limits<float>::infinity();

		Nz::Vector3f mousePos(float(event.x), float(event.y), 0.f);
		for (const WidgetBox& entry : m_widgetBoxes)
		{
			const Nz::Boxf& box = entry.box;

			if (box.Contains(mousePos))
			{
				float area = box.width * box.height;
				if (area < bestEntryArea)
				{
					bestEntry = &entry;
					bestEntryArea = area;
				}
			}
		}

		if (bestEntry)
		{
			if (m_hoveredWidget != bestEntry)
			{
				if (m_hoveredWidget)
					m_hoveredWidget->widget->OnMouseExit();

				m_hoveredWidget = bestEntry;
				m_hoveredWidget->widget->OnMouseEnter();

				if (m_cursorController)
					m_cursorController->UpdateCursor(Nz::Cursor::Get(m_hoveredWidget->cursor));
			}

			int x = static_cast<int>(std::round(event.x - m_hoveredWidget->box.x));
			int y = static_cast<int>(std::round(event.y - m_hoveredWidget->box.y));

			bestEntry->widget->OnMouseMoved(x, y, event.deltaX, event.deltaY);
		}
		else if (m_hoveredWidget)
		{
			m_hoveredWidget->widget->OnMouseExit();
			m_hoveredWidget = nullptr;

			if (m_cursorController)
				m_cursorController->UpdateCursor(Nz::Cursor::Get(Nz::SystemCursor_Default));
		}
	}

	void Canvas::OnMouseLeft(const Nz::EventHandler* /*eventHandler*/)
	{
		if (m_hoveredWidget)
		{
			m_hoveredWidget->widget->OnMouseExit();
			m_hoveredWidget = nullptr;
		}
	}

	void Canvas::OnKeyPressed(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner)
			m_keyboardOwner->OnKeyPressed(event);
	}

	void Canvas::OnKeyReleased(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& event)
	{
		if (m_keyboardOwner)
			m_keyboardOwner->OnKeyReleased(event);
	}

	void Canvas::OnTextEntered(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::TextEvent& event)
	{
		if (m_keyboardOwner)
			m_keyboardOwner->OnTextEntered(event.character, event.repeated);
	}
}
