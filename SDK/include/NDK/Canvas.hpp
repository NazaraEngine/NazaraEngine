// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_CANVAS_HPP
#define NDK_CANVAS_HPP

#include <NDK/ClientPrerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Platform/CursorController.hpp>
#include <Nazara/Platform/EventHandler.hpp>

namespace Ndk
{
	class NDK_CLIENT_API Canvas : public BaseWidget
	{
		friend BaseWidget;

		public:
			struct Padding;

			inline Canvas(WorldHandle world, Nz::EventHandler& eventHandler, Nz::CursorControllerHandle cursorController);
			Canvas(const Canvas&) = delete;
			Canvas(Canvas&&) = delete;
			inline ~Canvas();

			inline const WorldHandle& GetWorld() const;

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
			Nz::CursorControllerHandle m_cursorController;
			WorldHandle m_world;
	};
}

#include <NDK/Canvas.inl>

#endif // NDK_CANVAS_HPP
