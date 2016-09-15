// Copyright (C) 2015 Jérôme Leclercq
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
			~Canvas() = default;

			inline const WorldHandle& GetWorld() const;

			void ResizeToContent() override;

			Canvas& operator=(const Canvas&) = delete;
			Canvas& operator=(Canvas&&) = delete;

		protected:
			void Layout() override;

			void NotifyWidgetUpdate(std::size_t index);
			std::size_t RegisterWidget(BaseWidget* widget);
			void UnregisterWidget(std::size_t index);

		private:
			void OnMouseMoved(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseMoveEvent& event);
		
			struct WidgetBox
			{
				BaseWidget* widget;
				Nz::Boxf box;
			};

			NazaraSlot(Nz::EventHandler, OnMouseMoved, m_mouseMovedSlot);

			std::vector<WidgetBox> m_widgetBoxes;
			BaseWidget* m_hoveredWidget;
			WorldHandle m_world;
	};
}

#include <NDK/Canvas.inl>

#endif // NDK_CANVAS_HPP
