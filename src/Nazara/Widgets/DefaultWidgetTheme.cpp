// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/DefaultWidgetTheme.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Widgets/SimpleWidgetStyles.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <span>

namespace Nz
{
	namespace
	{
		constexpr UInt8 s_defaultThemeButtonImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/Button.png.h>
		};

		constexpr UInt8 s_defaultThemeButtonHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ButtonHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeButtonPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ButtonPressed.png.h>
		};

		constexpr UInt8 s_defaultThemeButtonPressedHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ButtonPressedHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeCheckboxBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxBackground.png.h>
		};

		constexpr UInt8 s_defaultThemeCheckboxBackgroundHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxBackgroundHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeCheckboxCheckImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxCheck.png.h>
		};

		constexpr UInt8 s_defaultThemeCheckboxTristateImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxTristate.png.h>
		};

		constexpr UInt8 s_defaultThemeHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/Hovered.png.h>
		};

		constexpr UInt8 s_defaultThemeProgressBarBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ProgressBarBackground.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarHorizontalBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarBackgroundHorizontal.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarVerticalBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarBackgroundVertical.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowDownImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowDown.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowDownHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowDownHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowDownPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowDownPressed.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowLeftImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowLeft.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowLeftHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowLeftHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowLeftPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowLeftPressed.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowRightImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowRight.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowRightHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowRightHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowRightPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowRightPressed.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowUpImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowUp.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowUpHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowUpHovered.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarArrowUpPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowUpPressed.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarCenterImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarCenter.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarGrabbedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarCenterGrabbed.png.h>
		};

		constexpr UInt8 s_defaultThemeScrollbarHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarCenterHovered.png.h>
		};
	}

	DefaultWidgetTheme::DefaultWidgetTheme()
	{
		auto CreateMaterialFromTexture = [&](std::span<const UInt8> textureMemory)
		{
			std::shared_ptr<MaterialInstance> material = Widgets::Instance()->GetTransparentMaterial()->Clone();
			material->SetTextureProperty("BaseColorMap", TextureAsset::OpenFromMemory(textureMemory.data(), textureMemory.size(), {.sRGB = true}));

			return material;
		};

		m_hoveredMaterial = CreateMaterialFromTexture(s_defaultThemeHoveredImage);

		// Button materials
		m_buttonMaterial = CreateMaterialFromTexture(s_defaultThemeButtonImage);
		m_buttonHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeButtonHoveredImage);
		m_buttonPressedMaterial = CreateMaterialFromTexture(s_defaultThemeButtonPressedImage);
		m_buttonPressedHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeButtonPressedHoveredImage);

		// Checkbox materials
		m_checkboxBackgroundMaterial = CreateMaterialFromTexture(s_defaultThemeCheckboxBackgroundImage);
		m_checkboxBackgroundHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeCheckboxBackgroundHoveredImage);
		m_checkboxCheckMaterial = CreateMaterialFromTexture(s_defaultThemeCheckboxCheckImage);
		m_checkboxTristateMaterial = CreateMaterialFromTexture(s_defaultThemeCheckboxTristateImage);

		// ProgressBar materials
		m_progressBarMaterial = CreateMaterialFromTexture(s_defaultThemeProgressBarBackgroundImage);

		// Scrollbar materials
		m_scrollbarBackgroundHorizontalMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarHorizontalBackgroundImage);
		m_scrollbarBackgroundVerticalMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarVerticalBackgroundImage);

		// Config
		m_config.scrollbarButtonCornerSize = 0.f;
		m_config.scrollbarButtonCornerTexcoords = 0.f;

		m_scrollbarButtonMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarCenterImage);
		m_scrollbarButtonGrabbedMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarGrabbedImage);
		m_scrollbarButtonHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarHoveredImage);

		m_config.scrollbarButtonDownMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowDownImage);
		m_config.scrollbarButtonDownHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowDownHoveredImage);
		m_config.scrollbarButtonDownPressedMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowDownPressedImage);

		m_config.scrollbarButtonLeftMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowLeftImage);
		m_config.scrollbarButtonLeftHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowLeftHoveredImage);
		m_config.scrollbarButtonLeftPressedMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowLeftPressedImage);

		m_config.scrollbarButtonRightMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowRightImage);
		m_config.scrollbarButtonRightHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowRightHoveredImage);
		m_config.scrollbarButtonRightPressedMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowRightPressedImage);

		m_config.scrollbarButtonUpMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowUpImage);
		m_config.scrollbarButtonUpHoveredMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowUpHoveredImage);
		m_config.scrollbarButtonUpPressedMaterial = CreateMaterialFromTexture(s_defaultThemeScrollbarArrowUpPressedImage);
	}

	std::unique_ptr<ButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ButtonWidget* buttonWidget) const
	{
		SimpleButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.cornerSize = 20.f;
		styleConfig.cornerTexCoords = 20.f / 44.f;
		styleConfig.hoveredMaterial = m_buttonHoveredMaterial;
		styleConfig.material = m_buttonMaterial;
		styleConfig.pressedHoveredMaterial = m_buttonPressedHoveredMaterial;
		styleConfig.pressedMaterial = m_buttonPressedMaterial;

		return std::make_unique<SimpleButtonWidgetStyle>(buttonWidget, styleConfig);
	}

	std::unique_ptr<CheckboxWidgetStyle> DefaultWidgetTheme::CreateStyle(CheckboxWidget* checkboxWidget) const
	{
		SimpleCheckboxWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundCornerSize = 10.f;
		styleConfig.backgroundCornerTexCoords = 10.f / 64.f;
		styleConfig.backgroundHoveredMaterial = m_checkboxBackgroundHoveredMaterial;
		styleConfig.backgroundMaterial = m_checkboxBackgroundMaterial;
		styleConfig.checkMaterial = m_checkboxCheckMaterial;
		styleConfig.tristateMaterial = m_checkboxTristateMaterial;

		return std::make_unique<SimpleCheckboxWidgetStyle>(checkboxWidget, styleConfig);
	}

	std::unique_ptr<ImageButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ImageButtonWidget* imageButtonWidget) const
	{
		SimpleImageButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.hoveredCornerSize = 8.f;
		styleConfig.hoveredCornerTexCoords = 8.f / 64.f;
		styleConfig.hoveredMaterial = m_hoveredMaterial;

		return std::make_unique<SimpleImageButtonWidgetStyle>(imageButtonWidget, styleConfig);
	}

	std::unique_ptr<LabelWidgetStyle> DefaultWidgetTheme::CreateStyle(AbstractLabelWidget* labelWidget) const
	{
		return std::make_unique<SimpleLabelWidgetStyle>(labelWidget, Widgets::Instance()->GetTransparentMaterial());
	}

	std::unique_ptr<ProgressBarWidgetStyle> DefaultWidgetTheme::CreateStyle(ProgressBarWidget* progressBarWidget) const
	{
		SimpleProgressBarWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundCornerSize = 16.f;
		styleConfig.backgroundCornerTexCoords = 16.f / 64.f;
		styleConfig.backgroundMaterial = m_progressBarMaterial;
		styleConfig.barOffset = 12.f;
		styleConfig.progressBarBeginColor = Nz::Color::DarkGreen();
		styleConfig.progressBarEndColor = Nz::Color::Green();

		return std::make_unique<SimpleProgressBarWidgetStyle>(progressBarWidget, styleConfig);
	}

	std::unique_ptr<ScrollAreaWidgetStyle> DefaultWidgetTheme::CreateStyle(ScrollAreaWidget* scrollAreaWidget) const
	{
		return std::make_unique<SimpleScrollAreaWidgetStyle>(scrollAreaWidget);
	}

	std::unique_ptr<ScrollbarWidgetStyle> DefaultWidgetTheme::CreateStyle(ScrollbarWidget* scrollBarWidget) const
	{
		SimpleScrollbarWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundHorizontalMaterial = m_scrollbarBackgroundHorizontalMaterial;
		styleConfig.backgroundVerticalMaterial = m_scrollbarBackgroundVerticalMaterial;

		return std::make_unique<SimpleScrollbarWidgetStyle>(scrollBarWidget, styleConfig);
	}

	std::unique_ptr<ScrollbarButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ScrollbarButtonWidget* scrollbarButtonWidget) const
	{
		SimpleScrollbarButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.cornerSize = 16.f;
		styleConfig.cornerTexCoords = 16.f / 64.f;
		styleConfig.grabbedMaterial = m_scrollbarButtonGrabbedMaterial;
		styleConfig.hoveredMaterial = m_scrollbarButtonHoveredMaterial;
		styleConfig.material = m_scrollbarButtonMaterial;

		return std::make_unique<SimpleScrollbarButtonWidgetStyle>(scrollbarButtonWidget, styleConfig);
	}

}
