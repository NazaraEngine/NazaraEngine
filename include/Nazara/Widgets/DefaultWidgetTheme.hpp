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
			std::unique_ptr<TextAreaWidgetStyle> CreateStyle(AbstractTextAreaWidget* textAreaWidget) const override;

			DefaultWidgetTheme& operator=(const DefaultWidgetTheme&) = delete;
			DefaultWidgetTheme& operator=(DefaultWidgetTheme&&) = default;

		private:
			Rectf RectToCoords(const Rectf& rect) const;

			std::shared_ptr<MaterialInstance> m_material;
			Rectf m_buttonRect;
			Rectf m_buttonHoveredRect;
			Rectf m_buttonPressedHoveredRect;
			Rectf m_buttonPressedRect;
			Rectf m_checkboxBackgroundRect;
			Rectf m_checkboxBackgroundDisabledRect;
			Rectf m_checkboxBackgroundHoveredRect;
			Rectf m_checkboxCheckRect;
			Rectf m_checkboxTristateRect;
			Rectf m_hoveredRect;
			Rectf m_progressBarRect;
			Rectf m_scrollbarBackgroundHorizontalRect;
			Rectf m_scrollbarBackgroundVerticalRect;
			Rectf m_scrollbarButtonRect;
			Rectf m_scrollbarButtonDisabledRect;
			Rectf m_scrollbarButtonHoveredRect;
			Rectf m_scrollbarButtonGrabbedRect;
			Rectf m_scrollbarButtonDownRect;
			Rectf m_scrollbarButtonDownHoveredRect;
			Rectf m_scrollbarButtonDownPressedRect;
			Rectf m_scrollbarButtonLeftRect;
			Rectf m_scrollbarButtonLeftHoveredRect;
			Rectf m_scrollbarButtonLeftPressedRect;
			Rectf m_scrollbarButtonRightRect;
			Rectf m_scrollbarButtonRightHoveredRect;
			Rectf m_scrollbarButtonRightPressedRect;
			Rectf m_scrollbarButtonUpRect;
			Rectf m_scrollbarButtonUpHoveredRect;
			Rectf m_scrollbarButtonUpPressedRect;
			Rectf m_textBoxRect;
			Vector2f m_imageSize;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_DEFAULTWIDGETTHEME_HPP
