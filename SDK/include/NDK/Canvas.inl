// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Canvas.hpp>
#include <Nazara/Utility/Cursor.hpp>

namespace Ndk
{
	inline Canvas::Canvas(WorldHandle world, Nz::EventHandler& eventHandler, Nz::CursorControllerHandle cursorController) :
	m_cursorController(cursorController),
	m_hoveredWidget(nullptr),
	m_keyboardOwner(nullptr),
	m_world(std::move(world))
	{
		m_canvas = this;
		m_widgetParent = nullptr;

		// Register ourselves as a widget to handle cursor change
		RegisterToCanvas();

		// Connect to every meaningful event
		m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, this, &Canvas::OnKeyPressed);
		m_keyReleasedSlot.Connect(eventHandler.OnKeyReleased, this, &Canvas::OnKeyReleased);
		m_mouseButtonPressedSlot.Connect(eventHandler.OnMouseButtonPressed, this, &Canvas::OnMouseButtonPressed);
		m_mouseButtonReleasedSlot.Connect(eventHandler.OnMouseButtonReleased, this, &Canvas::OnMouseButtonRelease);
		m_mouseMovedSlot.Connect(eventHandler.OnMouseMoved, this, &Canvas::OnMouseMoved);
		m_mouseLeftSlot.Connect(eventHandler.OnMouseLeft, this, &Canvas::OnMouseLeft);
		m_textEnteredSlot.Connect(eventHandler.OnTextEntered, this, &Canvas::OnTextEntered);

		// Disable padding by default
		SetPadding(0.f, 0.f, 0.f, 0.f);
	}

	inline Canvas::~Canvas()
	{
		// Destroy children explicitly because they signal us when getting destroyed, and that can't happen after our own destruction
		DestroyChildren();

		// Prevent our parent from trying to call us
		m_canvasIndex = InvalidCanvasIndex;
	}

	inline const WorldHandle& Canvas::GetWorld() const
	{
		return m_world;
	}

	inline void Canvas::NotifyWidgetBoxUpdate(std::size_t index)
	{
		WidgetBox& entry = m_widgetBoxes[index];

		Nz::Vector3f pos = entry.widget->GetPosition();
		Nz::Vector2f size = entry.widget->GetContentSize();

		entry.box.Set(pos.x, pos.y, pos.z, size.x, size.y, 1.f);
	}

	inline void Canvas::NotifyWidgetCursorUpdate(std::size_t index)
	{
		WidgetBox& entry = m_widgetBoxes[index];

		entry.cursor = entry.widget->GetCursor();
		if (m_cursorController && m_hoveredWidget == &entry)
			m_cursorController->UpdateCursor(Nz::Cursor::Get(entry.cursor));
	}

	inline void Ndk::Canvas::SetKeyboardOwner(BaseWidget* widget)
	{
		m_keyboardOwner = widget;
	}
}
