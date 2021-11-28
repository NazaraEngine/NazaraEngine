// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_DEFAULTWIDGETTHEME_HPP
#define NAZARA_WIDGETS_DEFAULTWIDGETTHEME_HPP

#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class DefaultDefaultButtonWidgetStyle;

	class NAZARA_WIDGETS_API DefaultWidgetTheme : public WidgetTheme
	{
		public:
			DefaultWidgetTheme();
			DefaultWidgetTheme(const DefaultWidgetTheme&) = delete;
			DefaultWidgetTheme(DefaultWidgetTheme&&) = default;
			~DefaultWidgetTheme() = default;

			std::unique_ptr<ButtonWidgetStyle> CreateStyle(ButtonWidget* buttonWidget) const override;

			DefaultWidgetTheme& operator=(const DefaultWidgetTheme&) = delete;
			DefaultWidgetTheme& operator=(DefaultWidgetTheme&&) = default;

		private:
			std::shared_ptr<Material> m_buttonMaterial;
			std::shared_ptr<Material> m_pressedButtonMaterial;
	};

	class NAZARA_WIDGETS_API DefaultButtonWidgetStyle : public ButtonWidgetStyle
	{
		public:
			DefaultButtonWidgetStyle(ButtonWidget* buttonWidget, std::shared_ptr<Material> defaultMaterial, std::shared_ptr<Material> pressedMaterial);
			DefaultButtonWidgetStyle(const DefaultButtonWidgetStyle&) = delete;
			DefaultButtonWidgetStyle(DefaultButtonWidgetStyle&&) = default;
			~DefaultButtonWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnPress() override;
			void OnRelease() override;

			void UpdateText(const AbstractTextDrawer& drawer) override;

			DefaultButtonWidgetStyle& operator=(const DefaultButtonWidgetStyle&) = delete;
			DefaultButtonWidgetStyle& operator=(DefaultButtonWidgetStyle&&) = default;

		private:
			std::shared_ptr<Material> m_defaultMaterial;
			std::shared_ptr<Material> m_pressedMaterial;
			std::shared_ptr<SlicedSprite> m_sprite;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_textEntity;
			entt::entity m_gradientEntity;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_DEFAULTWIDGETTHEME_HPP
