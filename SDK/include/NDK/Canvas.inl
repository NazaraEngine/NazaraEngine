// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Canvas.hpp>
#include <Nazara/Platform/Cursor.hpp>

namespace Ndk
{
	inline Canvas::Canvas(WorldHandle world, Nz::EventHandler& eventHandler, Nz::CursorControllerHandle cursorController) :
	m_hoveredWidget(InvalidCanvasIndex),
	m_keyboardOwner(InvalidCanvasIndex),
	m_cursorController(cursorController),
	m_world(std::move(world))
	{
		m_canvas = this;
		m_widgetParent = nullptr;

		// Register ourselves as a widget to handle cursor change
		RegisterToCanvas();

		// Connect to every meaningful event
		m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, this, &Canvas::OnEventKeyPressed);
		m_keyReleasedSlot.Connect(eventHandler.OnKeyReleased, this, &Canvas::OnEventKeyReleased);
		m_mouseButtonPressedSlot.Connect(eventHandler.OnMouseButtonPressed, this, &Canvas::OnEventMouseButtonPressed);
		m_mouseButtonReleasedSlot.Connect(eventHandler.OnMouseButtonReleased, this, &Canvas::OnEventMouseButtonRelease);
		m_mouseMovedSlot.Connect(eventHandler.OnMouseMoved, this, &Canvas::OnEventMouseMoved);
		m_mouseLeftSlot.Connect(eventHandler.OnMouseLeft, this, &Canvas::OnEventMouseLeft);
		m_textEnteredSlot.Connect(eventHandler.OnTextEntered, this, &Canvas::OnEventTextEntered);

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

	inline void Canvas::ClearKeyboardOwner(std::size_t canvasIndex)
	{
		if (m_keyboardOwner == canvasIndex)
			SetKeyboardOwner(InvalidCanvasIndex);
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
		if (m_cursorController && m_hoveredWidget == index)
			m_cursorController->UpdateCursor(Nz::Cursor::Get(entry.cursor));
	}

	inline void Canvas::SetKeyboardOwner(std::size_t canvasIndex)
	{
		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetBoxes[m_keyboardOwner].widget->OnFocusLost();

		m_keyboardOwner = canvasIndex;

		if (m_keyboardOwner != InvalidCanvasIndex)
			m_widgetBoxes[m_keyboardOwner].widget->OnFocusReceived();
	}
}
