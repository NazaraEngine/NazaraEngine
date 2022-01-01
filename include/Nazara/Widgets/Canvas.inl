// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline Canvas::~Canvas()
	{
		// Destroy children explicitly because they signal us when getting destroyed, and that can't happen after our own destruction
		DestroyChildren();

		// Prevent our parent from trying to call us
		m_canvasIndex = InvalidCanvasIndex;
	}

	inline entt::registry& Canvas::GetRegistry()
	{
		return m_registry;
	}

	inline const entt::registry& Canvas::GetRegistry() const
	{
		return m_registry;
	}

	inline UInt32 Canvas::GetRenderMask() const
	{
		return m_renderMask;
	}

	inline void Canvas::ClearKeyboardOwner(std::size_t canvasIndex)
	{
		if (m_keyboardOwner == canvasIndex)
			SetKeyboardOwner(InvalidCanvasIndex);
	}

	inline void Canvas::ClearMouseOwner(std::size_t canvasIndex)
	{
		if (m_mouseOwner == canvasIndex)
			SetMouseOwner(InvalidCanvasIndex);
	}

	inline bool Canvas::IsKeyboardOwner(std::size_t canvasIndex) const
	{
		return m_keyboardOwner == canvasIndex;
	}

	inline bool Canvas::IsMouseOwner(std::size_t canvasIndex) const
	{
		return m_mouseOwner == canvasIndex;
	}

	inline std::size_t Canvas::GetMouseEventTarget() const
	{
		if (m_mouseOwner != InvalidCanvasIndex)
			return m_mouseOwner;
		else
			return m_hoveredWidget;
	}

	inline void Canvas::NotifyWidgetBoxUpdate(std::size_t index)
	{
		WidgetEntry& entry = m_widgetEntries[index];

		Nz::Vector3f pos = entry.widget->GetPosition(Nz::CoordSys::Global);
		Nz::Vector2f size = entry.widget->GetSize();

		entry.box.Set(pos.x, pos.y, pos.z, size.x, size.y, 1.f);
	}

	inline void Canvas::NotifyWidgetCursorUpdate(std::size_t index)
	{
		WidgetEntry& entry = m_widgetEntries[index];

		entry.cursor = entry.widget->GetCursor();
		if (m_cursorController && m_hoveredWidget == index)
			m_cursorController->UpdateCursor(Nz::Cursor::Get(entry.cursor));
	}

	inline void Canvas::SetKeyboardOwner(std::size_t canvasIndex)
	{
		if (m_keyboardOwner != canvasIndex)
		{
			if (m_keyboardOwner != InvalidCanvasIndex)
				m_widgetEntries[m_keyboardOwner].widget->OnFocusLost();

			m_keyboardOwner = canvasIndex;

			if (m_keyboardOwner != InvalidCanvasIndex)
				m_widgetEntries[m_keyboardOwner].widget->OnFocusReceived();
		}
	}

	inline void Canvas::SetMouseOwner(std::size_t canvasIndex)
	{
		m_mouseOwner = canvasIndex;
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
