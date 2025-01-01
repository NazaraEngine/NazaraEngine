// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP
#define NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetStyleFactory.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class ScrollbarWidget;

	class NAZARA_WIDGETS_API ScrollAreaWidget : public BaseWidget
	{
		public:
			using StyleFactory = WidgetStyleFactory<ScrollAreaWidget, ScrollAreaWidgetStyle>;

			ScrollAreaWidget(BaseWidget* parent, BaseWidget* content, const StyleFactory& styleFactory = nullptr);
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

			void OnChildPreferredSizeUpdated(const BaseWidget* child) override;
			bool OnMouseWheelMoved(int x, int y, float delta) override;
			void OnVisibilityUpdated(bool isVisible) override;

			std::unique_ptr<ScrollAreaWidgetStyle> m_style;
			BaseWidget* m_content;
			ScrollbarWidget* m_verticalScrollbar;
			bool m_isPerformingLayout;
			bool m_isScrollbarEnabled;
			bool m_hasScrollbar;
	};
}

#include <Nazara/Widgets/ScrollAreaWidget.inl>

#endif // NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP
