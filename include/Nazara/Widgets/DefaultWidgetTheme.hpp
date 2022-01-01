// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
	class NAZARA_WIDGETS_API DefaultWidgetTheme : public WidgetTheme
	{
		public:
			DefaultWidgetTheme();
			DefaultWidgetTheme(const DefaultWidgetTheme&) = delete;
			DefaultWidgetTheme(DefaultWidgetTheme&&) = default;
			~DefaultWidgetTheme() = default;

			std::unique_ptr<ButtonWidgetStyle> CreateStyle(ButtonWidget* buttonWidget) const override;
			std::unique_ptr<CheckboxWidgetStyle> CreateStyle(CheckboxWidget* buttonWidget) const override;
			std::unique_ptr<LabelWidgetStyle> CreateStyle(LabelWidget* buttonWidget) const override;

			DefaultWidgetTheme& operator=(const DefaultWidgetTheme&) = delete;
			DefaultWidgetTheme& operator=(DefaultWidgetTheme&&) = default;

		private:
			std::shared_ptr<Material> m_buttonMaterial;
			std::shared_ptr<Material> m_buttonHoveredMaterial;
			std::shared_ptr<Material> m_buttonPressedHoveredMaterial;
			std::shared_ptr<Material> m_buttonPressedMaterial;
			std::shared_ptr<Material> m_checkboxBackgroundMaterial;
			std::shared_ptr<Material> m_checkboxBackgroundHoveredMaterial;
			std::shared_ptr<Material> m_checkboxCheckMaterial;
			std::shared_ptr<Material> m_checkboxTristateMaterial;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_DEFAULTWIDGETTHEME_HPP
