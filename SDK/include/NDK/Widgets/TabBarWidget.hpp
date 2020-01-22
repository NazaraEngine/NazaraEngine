// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_TABBARWIDGET_HPP
#define NDK_WIDGETS_TABBARWIDGET_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Ndk
{
	class NDK_API TabBarWidget : public BaseWidget
	{
		public:
			TabBarWidget(BaseWidget* parent);
			TabBarWidget(const TabBarWidget&) = delete;
			TabBarWidget(TabBarWidget&&) = default;
			~TabBarWidget() = default;

			std::size_t AddTab(const Nz::String& text);

			inline void DisableAutoHide();
			void EnableAutoHide(bool enable);

			inline bool IsAutoHideEnabled() const;

			void SelectTab(std::size_t tabIndex);

			TabBarWidget& operator=(const TabBarWidget&) = delete;
			TabBarWidget& operator=(TabBarWidget&&) = default;

			NazaraSignal(OnTabChanged, TabBarWidget* /*emitter*/, std::size_t /*index*/);

			static constexpr std::size_t NoSelection = std::numeric_limits<std::size_t>::max();

		private:
			std::size_t GetHoveredTab(float x, float y) const;

			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseExit() override;
			void OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/) override;

			inline bool ShouldHide() const;

			struct TabData
			{
				Nz::SpriteRef backgroundSprite;
				Nz::TextSpriteRef textSprite;
				float origin;
			};

			std::size_t m_hoveredTab;
			std::size_t m_selectedTab;
			std::vector<TabData> m_tabs;
			EntityHandle m_entity;
			bool m_shouldAutoHide;
			float m_height;
	};
}

#include <NDK/Widgets/TabBarWidget.inl>

#endif // NDK_WIDGETS_TABBARWIDGET_HPP
