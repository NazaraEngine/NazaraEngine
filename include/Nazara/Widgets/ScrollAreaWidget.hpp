// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
	class NAZARA_WIDGETS_API ScrollAreaWidget : public BaseWidget
	{
		public:
			ScrollAreaWidget(BaseWidget* parent, BaseWidget* content);
			ScrollAreaWidget(const ScrollAreaWidget&) = delete;
			ScrollAreaWidget(ScrollAreaWidget&&) = default;
			~ScrollAreaWidget() = default;

			void EnableScrollbar(bool enable);

			inline float GetScrollHeight() const;
			inline float GetScrollRatio() const;

			inline bool HasScrollbar() const;
			inline bool IsScrollbarEnabled() const;
			inline bool IsScrollbarVisible() const;

			inline void ScrollToHeight(float height);
			void ScrollToRatio(float ratio);

			ScrollAreaWidget& operator=(const ScrollAreaWidget&) = delete;
			ScrollAreaWidget& operator=(ScrollAreaWidget&&) = default;

		private:
			Nz::Rectf GetScrollbarRect() const;

			void Layout() override;

			void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseExit() override;
			void OnMouseMoved(int x, int y, int deltaX, int deltaY) override;
			void OnMouseWheelMoved(int x, int y, float delta) override;

			std::unique_ptr<ScrollAreaWidgetStyle> m_style;
			BaseWidget* m_content;
			EntityHandle m_scrollbarBackgroundEntity;
			EntityHandle m_scrollbarEntity;
			Nz::SpriteRef m_scrollbarBackgroundSprite;
			Nz::SpriteRef m_scrollbarSprite;
			Nz::Vector2i m_grabbedDelta;
			bool m_isGrabbed;
			bool m_isScrollbarEnabled;
			bool m_hasScrollbar;
			float m_scrollRatio;
	};
}

#include <Nazara/Widgets/ScrollAreaWidget.inl>

#endif // NAZARA_WIDGETS_SCROLLAREAWIDGET_HPP
