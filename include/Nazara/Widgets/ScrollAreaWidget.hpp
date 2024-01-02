// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP
#define NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class ScrollbarWidget;

	class NAZARA_WIDGETS_API ScrollAreaWidget : public BaseWidget
	{
		public:
			ScrollAreaWidget(BaseWidget* parent, BaseWidget* content);
			ScrollAreaWidget(const ScrollAreaWidget&) = delete;
			ScrollAreaWidget(ScrollAreaWidget&&) = delete;
			~ScrollAreaWidget() = default;

			void EnableScrollbar(bool enable);

			float GetScrollHeight() const;
			float GetScrollRatio() const;

			inline bool HasScrollbar() const;
			inline bool IsScrollbarEnabled() const;
			inline bool IsScrollbarVisible() const;

			inline void ScrollToHeight(float height);
			void ScrollToRatio(float ratio);

			ScrollAreaWidget& operator=(const ScrollAreaWidget&) = delete;
			ScrollAreaWidget& operator=(ScrollAreaWidget&&) = delete;

		private:
			void Layout() override;

			bool OnMouseWheelMoved(int x, int y, float delta) override;

			std::unique_ptr<ScrollAreaWidgetStyle> m_style;
			BaseWidget* m_content;
			ScrollbarWidget* m_horizontalScrollbar;
			bool m_isScrollbarEnabled;
			bool m_hasScrollbar;
	};
}

#include <Nazara/Widgets/ScrollAreaWidget.inl>

#endif // NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP
