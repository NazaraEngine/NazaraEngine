// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CANVAS_HPP
#define NAZARA_CANVAS_HPP

#include <Nazara/Platform/CursorController.hpp>
#include <Nazara/Platform/EventHandler.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <entt/entity/registry.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API Canvas : public BaseWidget
	{
		friend BaseWidget;

		public:
			inline Canvas(entt::registry& registry, Nz::EventHandler& eventHandler, Nz::CursorControllerHandle cursorController);
			Canvas(const Canvas&) = delete;
			Canvas(Canvas&&) = delete;
			inline ~Canvas();

			inline entt::registry& GetRegistry();
			inline const entt::registry& GetRegistry() const;

			Canvas& operator=(const Canvas&) = delete;
			Canvas& operator=(Canvas&&) = delete;

			NazaraSignal(OnUnhandledKeyPressed, const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnUnhandledKeyReleased, const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::KeyEvent& /*event*/);

		protected:
			inline void ClearKeyboardOwner(std::size_t canvasIndex);

			inline bool IsKeyboardOwner(std::size_t canvasIndex) const;

			inline void NotifyWidgetBoxUpdate(std::size_t index);
			inline void NotifyWidgetCursorUpdate(std::size_t index);

			std::size_t RegisterWidget(BaseWidget* widget);

			inline void SetKeyboardOwner(std::size_t canvasIndex);

			void UnregisterWidget(std::size_t index);

		private:
			void OnEventMouseButtonPressed(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseButtonEvent& event);
			void OnEventMouseButtonRelease(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseButtonEvent& event);
			void OnEventMouseLeft(const Nz::EventHandler* eventHandler);
			void OnEventMouseMoved(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseMoveEvent& event);
			void OnEventMouseWheelMoved(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseWheelEvent& event);
			void OnEventKeyPressed(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::KeyEvent& event);
			void OnEventKeyReleased(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::KeyEvent& event);
			void OnEventTextEntered(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::TextEvent& event);
			void OnEventTextEdited(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::EditEvent& event);

			struct WidgetEntry
			{
				BaseWidget* widget;
				Nz::Boxf box;
				Nz::SystemCursor cursor;
			};

			NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
			NazaraSlot(Nz::EventHandler, OnKeyReleased, m_keyReleasedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseButtonPressed, m_mouseButtonPressedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseButtonReleased, m_mouseButtonReleasedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseLeft, m_mouseLeftSlot);
			NazaraSlot(Nz::EventHandler, OnMouseMoved, m_mouseMovedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseWheelMoved, m_mouseWheelMovedSlot);
			NazaraSlot(Nz::EventHandler, OnTextEntered, m_textEnteredSlot);
			NazaraSlot(Nz::EventHandler, OnTextEdited, m_textEditedSlot);

			std::size_t m_keyboardOwner;
			std::size_t m_hoveredWidget;
			std::vector<WidgetEntry> m_widgetEntries;
			entt::registry& m_registry;
			Nz::CursorControllerHandle m_cursorController;
	};
}

#include <Nazara/Widgets/Canvas.inl>

#endif // NDK_CANVAS_HPP
