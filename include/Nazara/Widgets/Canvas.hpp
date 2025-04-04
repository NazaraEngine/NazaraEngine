// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_CANVAS_HPP
#define NAZARA_WIDGETS_CANVAS_HPP

#include <Nazara/Platform/CursorController.hpp>
#include <Nazara/Platform/TextInputController.hpp>
#include <Nazara/Platform/WindowEventHandler.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <entt/entity/registry.hpp>
#include <bitset>

namespace Nz
{
	class Window;

	class NAZARA_WIDGETS_API Canvas : public BaseWidget
	{
		friend BaseWidget;

		public:
			Canvas(entt::registry& registry, Window& window, UInt32 renderMask, int initialRenderLayer = 0);
			Canvas(entt::registry& registry, WindowEventHandler& eventHandler, CursorControllerHandle cursorController, TextInputControllerHandle textInputController, UInt32 renderMask, int initialRenderLayer = 0);
			Canvas(const Canvas&) = delete;
			Canvas(Canvas&&) = delete;
			inline ~Canvas();

			inline void ClearKeyboardFocus();
			inline void ClearMouseFocus();

			inline entt::registry& GetRegistry();
			inline const entt::registry& GetRegistry() const;
			inline UInt32 GetRenderMask() const;

			Canvas& operator=(const Canvas&) = delete;
			Canvas& operator=(Canvas&&) = delete;

			NazaraSignal(OnUnhandledMouseButtonPressed, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnUnhandledMouseButtonReleased, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnUnhandledMouseMoved, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseMoveEvent& /*event*/);
			NazaraSignal(OnUnhandledMouseWheelMoved, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseWheelEvent& /*event*/);
			NazaraSignal(OnUnhandledKeyPressed, const WindowEventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnUnhandledKeyReleased, const WindowEventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);

		protected:
			inline bool ClearKeyboardOwner(std::size_t canvasIndex);
			inline bool ClearMouseOwner(std::size_t canvasIndex);

			inline bool IsKeyboardOwner(std::size_t canvasIndex) const;
			inline bool IsMouseOwner(std::size_t canvasIndex) const;

			inline std::size_t GetMouseEventTarget() const;

			inline void NotifyWidgetBoxUpdate(std::size_t index);
			inline void NotifyWidgetCursorUpdate(std::size_t index);

			std::size_t RegisterWidget(BaseWidget* widget);

			inline void SetKeyboardOwner(std::size_t canvasIndex);
			inline void SetMouseOwner(std::size_t canvasIndex);

			void UnregisterWidget(std::size_t index);

		private:
			template<typename F> bool DispatchEvent(std::size_t widgetIndex, F&& functor);

			void OnEventMouseButtonPressed(const WindowEventHandler* eventHandler, const WindowEvent::MouseButtonEvent& event);
			void OnEventMouseButtonRelease(const WindowEventHandler* eventHandler, const WindowEvent::MouseButtonEvent& event);
			void OnEventMouseEntered(const WindowEventHandler* eventHandler);
			void OnEventMouseLeft(const WindowEventHandler* eventHandler);
			void OnEventMouseMoved(const WindowEventHandler* eventHandler, const WindowEvent::MouseMoveEvent& event);

			void OnEventMouseWheelMoved(const WindowEventHandler* eventHandler, const WindowEvent::MouseWheelEvent& event);
			void OnEventKeyPressed(const WindowEventHandler* eventHandler, const WindowEvent::KeyEvent& event);
			void OnEventKeyReleased(const WindowEventHandler* eventHandler, const WindowEvent::KeyEvent& event);
			void OnEventTextEntered(const WindowEventHandler* eventHandler, const WindowEvent::TextEvent& event);
			void OnEventTextEdited(const WindowEventHandler* eventHandler, const WindowEvent::EditEvent& event);

			void UpdateHoveredWidget(float x, float y);

			struct WidgetEntry
			{
				BaseWidget* widget;
				Rectf rect;
				SystemCursor cursor;
				Vector2f pos;
			};

			NazaraSlot(WindowEventHandler, OnKeyPressed, m_keyPressedSlot);
			NazaraSlot(WindowEventHandler, OnKeyReleased, m_keyReleasedSlot);
			NazaraSlot(WindowEventHandler, OnMouseButtonPressed, m_mouseButtonPressedSlot);
			NazaraSlot(WindowEventHandler, OnMouseButtonReleased, m_mouseButtonReleasedSlot);
			NazaraSlot(WindowEventHandler, OnMouseEntered, m_mouseEnteredSlot);
			NazaraSlot(WindowEventHandler, OnMouseLeft, m_mouseLeftSlot);
			NazaraSlot(WindowEventHandler, OnMouseMoved, m_mouseMovedSlot);
			NazaraSlot(WindowEventHandler, OnMouseWheelMoved, m_mouseWheelMovedSlot);
			NazaraSlot(WindowEventHandler, OnTextEntered, m_textEnteredSlot);
			NazaraSlot(WindowEventHandler, OnTextEdited, m_textEditedSlot);

			CursorControllerHandle m_cursorController;
			TextInputControllerHandle m_textInputController;
			UInt32 m_renderMask;
			std::bitset<Mouse::ButtonCount> m_mouseOwnerButtons;
			std::size_t m_keyboardOwner;
			std::size_t m_hoveredWidget;
			std::size_t m_mouseOwner;
			std::vector<WidgetEntry> m_widgetEntries;
			entt::registry& m_registry;
	};
}

#include <Nazara/Widgets/Canvas.inl>

#endif // NAZARA_WIDGETS_CANVAS_HPP
