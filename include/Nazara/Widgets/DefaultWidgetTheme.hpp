// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

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
			std::unique_ptr<CheckboxWidgetStyle> CreateStyle(CheckboxWidget* checkboxWidget) const override;
			std::unique_ptr<ImageButtonWidgetStyle> CreateStyle(ImageButtonWidget* imageButtonWidget) const override;
			std::unique_ptr<LabelWidgetStyle> CreateStyle(AbstractLabelWidget* labelWidget) const override;
			std::unique_ptr<ProgressBarWidgetStyle> CreateStyle(ProgressBarWidget* progressBarWidget) const override;
			std::unique_ptr<ScrollAreaWidgetStyle> CreateStyle(ScrollAreaWidget* scrollAreaWidget) const override;
			std::unique_ptr<ScrollbarWidgetStyle> CreateStyle(ScrollbarWidget* scrollbarWidget) const override;
			std::unique_ptr<ScrollbarButtonWidgetStyle> CreateStyle(ScrollbarButtonWidget* scrollbarButtonWidget) const override;

			DefaultWidgetTheme& operator=(const DefaultWidgetTheme&) = delete;
			DefaultWidgetTheme& operator=(DefaultWidgetTheme&&) = default;

		private:
			std::shared_ptr<MaterialInstance> m_buttonMaterial;
			std::shared_ptr<MaterialInstance> m_buttonHoveredMaterial;
			std::shared_ptr<MaterialInstance> m_buttonPressedHoveredMaterial;
			std::shared_ptr<MaterialInstance> m_buttonPressedMaterial;
			std::shared_ptr<MaterialInstance> m_checkboxBackgroundMaterial;
			std::shared_ptr<MaterialInstance> m_checkboxBackgroundHoveredMaterial;
			std::shared_ptr<MaterialInstance> m_checkboxCheckMaterial;
			std::shared_ptr<MaterialInstance> m_checkboxTristateMaterial;
			std::shared_ptr<MaterialInstance> m_hoveredMaterial;
			std::shared_ptr<MaterialInstance> m_progressBarMaterial;
			std::shared_ptr<MaterialInstance> m_scrollbarBackgroundHorizontalMaterial;
			std::shared_ptr<MaterialInstance> m_scrollbarBackgroundVerticalMaterial;
			std::shared_ptr<MaterialInstance> m_scrollbarButtonMaterial;
			std::shared_ptr<MaterialInstance> m_scrollbarButtonHoveredMaterial;
			std::shared_ptr<MaterialInstance> m_scrollbarButtonGrabbedMaterial;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_DEFAULTWIDGETTHEME_HPP
