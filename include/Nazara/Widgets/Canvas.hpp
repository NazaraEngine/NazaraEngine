// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_CANVAS_HPP
#define NAZARA_WIDGETS_CANVAS_HPP

#include <Nazara/Platform/CursorController.hpp>
#include <Nazara/Platform/EventHandler.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <entt/entity/registry.hpp>
#include <bitset>

namespace Nz
{
	class NAZARA_WIDGETS_API Canvas : public BaseWidget
	{
		friend BaseWidget;

		public:
			Canvas(entt::registry& registry, EventHandler& eventHandler, CursorControllerHandle cursorController, UInt32 renderMask, int initialRenderLayer = 0);
			Canvas(const Canvas&) = delete;
			Canvas(Canvas&&) = delete;
			inline ~Canvas();

			inline entt::registry& GetRegistry();
			inline const entt::registry& GetRegistry() const;
			inline UInt32 GetRenderMask() const;

			Canvas& operator=(const Canvas&) = delete;
			Canvas& operator=(Canvas&&) = delete;

			NazaraSignal(OnUnhandledKeyPressed, const EventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnUnhandledKeyReleased, const EventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);

		protected:
			inline void ClearKeyboardOwner(std::size_t canvasIndex);
			inline void ClearMouseOwner(std::size_t canvasIndex);

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
			template<typename F> void DispatchEvent(std::size_t widgetIndex, F&& functor);
			
			void OnEventMouseButtonPressed(const EventHandler* eventHandler, const WindowEvent::MouseButtonEvent& event);
			void OnEventMouseButtonRelease(const EventHandler* eventHandler, const WindowEvent::MouseButtonEvent& event);
			void OnEventMouseEntered(const EventHandler* eventHandler);
			void OnEventMouseLeft(const EventHandler* eventHandler);
			void OnEventMouseMoved(const EventHandler* eventHandler, const WindowEvent::MouseMoveEvent& event);

			void OnEventMouseWheelMoved(const EventHandler* eventHandler, const WindowEvent::MouseWheelEvent& event);
			void OnEventKeyPressed(const EventHandler* eventHandler, const WindowEvent::KeyEvent& event);
			void OnEventKeyReleased(const EventHandler* eventHandler, const WindowEvent::KeyEvent& event);
			void OnEventTextEntered(const EventHandler* eventHandler, const WindowEvent::TextEvent& event);
			void OnEventTextEdited(const EventHandler* eventHandler, const WindowEvent::EditEvent& event);

			void UpdateHoveredWidget(int x, int y);

			struct WidgetEntry
			{
				BaseWidget* widget;
				Boxf box;
				SystemCursor cursor;
			};

			NazaraSlot(EventHandler, OnKeyPressed, m_keyPressedSlot);
			NazaraSlot(EventHandler, OnKeyReleased, m_keyReleasedSlot);
			NazaraSlot(EventHandler, OnMouseButtonPressed, m_mouseButtonPressedSlot);
			NazaraSlot(EventHandler, OnMouseButtonReleased, m_mouseButtonReleasedSlot);
			NazaraSlot(EventHandler, OnMouseEntered, m_mouseEnteredSlot);
			NazaraSlot(EventHandler, OnMouseLeft, m_mouseLeftSlot);
			NazaraSlot(EventHandler, OnMouseMoved, m_mouseMovedSlot);
			NazaraSlot(EventHandler, OnMouseWheelMoved, m_mouseWheelMovedSlot);
			NazaraSlot(EventHandler, OnTextEntered, m_textEnteredSlot);
			NazaraSlot(EventHandler, OnTextEdited, m_textEditedSlot);

			CursorControllerHandle m_cursorController;
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
