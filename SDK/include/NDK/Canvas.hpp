// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_CANVAS_HPP
#define NDK_CANVAS_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Utility/EventHandler.hpp>

namespace Ndk
{
	class NDK_API Canvas : public BaseWidget
	{
		friend BaseWidget;

		public:
			struct Padding;

			inline Canvas(WorldHandle world, Nz::EventHandler& eventHandler);
			Canvas(const Canvas&) = delete;
			Canvas(Canvas&&) = delete;
			inline ~Canvas();

			inline const WorldHandle& GetWorld() const;

			void ResizeToContent() override;

			Canvas& operator=(const Canvas&) = delete;
			Canvas& operator=(Canvas&&) = delete;

		protected:
			void NotifyWidgetUpdate(std::size_t index);

			std::size_t RegisterWidget(BaseWidget* widget);

			inline void SetKeyboardOwner(BaseWidget* widget);

			void UnregisterWidget(std::size_t index);

		private:
			void OnMouseButtonPressed(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseButtonEvent& event);
			void OnMouseButtonRelease(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseButtonEvent& event);
			void OnMouseMoved(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseMoveEvent& event);
			void OnMouseLeft(const Nz::EventHandler* eventHandler);
			void OnKeyPressed(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::KeyEvent& event);
			void OnKeyReleased(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::KeyEvent& event);
			void OnTextEntered(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::TextEvent& event);

			struct WidgetBox
			{
				BaseWidget* widget;
				Nz::Boxf box;
			};

			NazaraSlot(Nz::EventHandler, OnKeyPressed, m_keyPressedSlot);
			NazaraSlot(Nz::EventHandler, OnKeyReleased, m_keyReleasedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseButtonPressed, m_mouseButtonPressedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseButtonReleased, m_mouseButtonReleasedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseMoved, m_mouseMovedSlot);
			NazaraSlot(Nz::EventHandler, OnMouseLeft, m_mouseLeftSlot);
			NazaraSlot(Nz::EventHandler, OnTextEntered, m_textEnteredSlot);

			std::vector<WidgetBox> m_widgetBoxes;
			const WidgetBox* m_hoveredWidget;
			BaseWidget* m_keyboardOwner;
			WorldHandle m_world;
	};
}

#include <NDK/Canvas.inl>

#endif // NDK_CANVAS_HPP
