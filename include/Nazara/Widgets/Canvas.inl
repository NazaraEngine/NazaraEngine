// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Cursor.hpp>

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

	inline bool Canvas::ClearKeyboardOwner(std::size_t canvasIndex)
	{
		if (m_keyboardOwner != canvasIndex)
			return false;

		SetKeyboardOwner(InvalidCanvasIndex);
		return true;
	}

	inline bool Canvas::ClearMouseOwner(std::size_t canvasIndex)
	{
		if (m_mouseOwner != canvasIndex)
			return false;

		SetMouseOwner(InvalidCanvasIndex);
		return true;
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
		entry.pos = Vector2f(entry.widget->GetGlobalPosition());
		entry.rect = entry.widget->GetScissorRect();
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
			std::size_t previousOwner = m_keyboardOwner;

			m_keyboardOwner = canvasIndex;

			if (previousOwner != InvalidCanvasIndex)
				m_widgetEntries[previousOwner].widget->OnFocusLost();

			if (m_keyboardOwner != InvalidCanvasIndex)
			{
				m_widgetEntries[m_keyboardOwner].widget->OnFocusReceived();

				if (previousOwner == InvalidCanvasIndex)
					m_textInputController->StartTextInput();
			}
			else if (previousOwner != InvalidCanvasIndex)
				m_textInputController->StopTextInput();
		}
	}

	inline void Canvas::SetMouseOwner(std::size_t canvasIndex)
	{
		m_mouseOwner = canvasIndex;
	}
}
