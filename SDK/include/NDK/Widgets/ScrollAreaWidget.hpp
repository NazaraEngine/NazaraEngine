// Copyright (C) 2019 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_SCROLLAREAWIDGET_HPP
#define NDK_WIDGETS_SCROLLAREAWIDGET_HPP

#include <NDK/ClientPrerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Ndk
{
	class NDK_CLIENT_API ScrollAreaWidget : public BaseWidget
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
			enum class ScrollBarStatus
			{
				Grabbed,
				Hovered,
				None
			};

			Nz::Rectf GetScrollbarRect() const;

			void Layout() override;

			void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseExit() override;
			void OnMouseMoved(int x, int y, int deltaX, int deltaY) override;
			void OnMouseWheelMoved(int x, int y, float delta) override;

			void UpdateScrollbarStatus(ScrollBarStatus status);

			BaseWidget* m_content;
			EntityHandle m_scrollbarBackgroundEntity;
			EntityHandle m_scrollbarEntity;
			Nz::SpriteRef m_scrollbarBackgroundSprite;
			Nz::SpriteRef m_scrollbarSprite;
			Nz::Vector2i m_grabbedDelta;
			ScrollBarStatus m_scrollbarStatus;
			bool m_isScrollbarEnabled;
			bool m_hasScrollbar;
			float m_scrollRatio;
	};
}

#include <NDK/Widgets/ScrollAreaWidget.inl>

#endif // NDK_WIDGETS_SCROLLAREAWIDGET_HPP
