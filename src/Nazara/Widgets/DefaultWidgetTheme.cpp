// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/DefaultWidgetTheme.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Widgets/SimpleWidgetStyles.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 s_defaultThemeButtonImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/Button.png.h>
		};

		const UInt8 s_defaultThemeButtonHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ButtonHovered.png.h>
		};

		const UInt8 s_defaultThemeButtonPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ButtonPressed.png.h>
		};

		const UInt8 s_defaultThemeButtonPressedHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ButtonPressedHovered.png.h>
		};

		const UInt8 s_defaultThemeCheckboxBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxBackground.png.h>
		};

		const UInt8 s_defaultThemeCheckboxBackgroundHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxBackgroundHovered.png.h>
		};

		const UInt8 s_defaultThemeCheckboxCheckImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxCheck.png.h>
		};

		const UInt8 s_defaultThemeCheckboxTristateImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/CheckboxTristate.png.h>
		};

		const UInt8 s_defaultThemeHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/Hovered.png.h>
		};

		const UInt8 s_defaultThemeScrollbarHorizontalBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarBackgroundHorizontal.png.h>
		};

		const UInt8 s_defaultThemeScrollbarVerticalBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarBackgroundVertical.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowDownImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowDown.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowDownHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowDownHovered.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowDownPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowDownPressed.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowLeftImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowLeft.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowLeftHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowLeftHovered.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowLeftPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowLeftPressed.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowRightImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowRight.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowRightHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowRightHovered.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowRightPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowRightPressed.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowUpImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowUp.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowUpHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowUpHovered.png.h>
		};

		const UInt8 s_defaultThemeScrollbarArrowUpPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarArrowUpPressed.png.h>
		};

		const UInt8 s_defaultThemeScrollbarCenterImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarCenter.png.h>
		};
		
		const UInt8 s_defaultThemeScrollbarGrabbedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarCenterGrabbed.png.h>
		};

		const UInt8 s_defaultThemeScrollbarHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/ScrollbarCenterHovered.png.h>
		};
	}

	DefaultWidgetTheme::DefaultWidgetTheme()
	{
		TextureParams texParams;
		texParams.renderDevice = Graphics::Instance()->GetRenderDevice();
		texParams.loadFormat = PixelFormat::RGBA8_SRGB;

		auto CreateMaterialFromTexture = [&](std::shared_ptr<Texture> texture)
		{
			std::shared_ptr<MaterialInstance> material = Widgets::Instance()->GetTransparentMaterial()->Clone();
			material->SetTextureProperty("BaseColorMap", std::move(texture));

			return material;
		};

		m_hoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeHoveredImage, sizeof(s_defaultThemeHoveredImage), texParams));

		// Button materials
		m_buttonMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeButtonImage, sizeof(s_defaultThemeButtonImage), texParams));
		m_buttonHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeButtonHoveredImage, sizeof(s_defaultThemeButtonHoveredImage), texParams));
		m_buttonPressedMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeButtonPressedImage, sizeof(s_defaultThemeButtonPressedImage), texParams));
		m_buttonPressedHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeButtonPressedHoveredImage, sizeof(s_defaultThemeButtonPressedHoveredImage), texParams));

		// Checkbox materials
		m_checkboxBackgroundMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeCheckboxBackgroundImage, sizeof(s_defaultThemeCheckboxBackgroundImage), texParams));
		m_checkboxBackgroundHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeCheckboxBackgroundHoveredImage, sizeof(s_defaultThemeCheckboxBackgroundHoveredImage), texParams));
		m_checkboxCheckMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeCheckboxCheckImage, sizeof(s_defaultThemeCheckboxCheckImage), texParams));
		m_checkboxTristateMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeCheckboxTristateImage, sizeof(s_defaultThemeCheckboxTristateImage), texParams));

		// Scrollbar materials
		m_scrollbarBackgroundHorizontalMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarHorizontalBackgroundImage, sizeof(s_defaultThemeScrollbarHorizontalBackgroundImage), texParams));
		m_scrollbarBackgroundVerticalMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarVerticalBackgroundImage, sizeof(s_defaultThemeScrollbarVerticalBackgroundImage), texParams));

		// Config
		m_config.scrollbarButtonCornerSize = 0.f;
		m_config.scrollbarButtonCornerTexcoords = 0.f;

		m_scrollbarButtonMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarCenterImage, sizeof(s_defaultThemeScrollbarCenterImage), texParams));
		m_scrollbarButtonGrabbedMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarGrabbedImage, sizeof(s_defaultThemeScrollbarGrabbedImage), texParams));
		m_scrollbarButtonHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarHoveredImage, sizeof(s_defaultThemeScrollbarHoveredImage), texParams));

		m_config.scrollbarButtonDownMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowDownImage, sizeof(s_defaultThemeScrollbarArrowDownImage), texParams));
		m_config.scrollbarButtonDownHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowDownHoveredImage, sizeof(s_defaultThemeScrollbarArrowDownHoveredImage), texParams));
		m_config.scrollbarButtonDownPressedMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowDownPressedImage, sizeof(s_defaultThemeScrollbarArrowDownPressedImage), texParams));

		m_config.scrollbarButtonLeftMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowLeftImage, sizeof(s_defaultThemeScrollbarArrowLeftImage), texParams));
		m_config.scrollbarButtonLeftHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowLeftHoveredImage, sizeof(s_defaultThemeScrollbarArrowLeftHoveredImage), texParams));
		m_config.scrollbarButtonLeftPressedMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowLeftPressedImage, sizeof(s_defaultThemeScrollbarArrowLeftPressedImage), texParams));

		m_config.scrollbarButtonRightMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowRightImage, sizeof(s_defaultThemeScrollbarArrowRightImage), texParams));
		m_config.scrollbarButtonRightHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowRightHoveredImage, sizeof(s_defaultThemeScrollbarArrowRightHoveredImage), texParams));
		m_config.scrollbarButtonRightPressedMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowRightPressedImage, sizeof(s_defaultThemeScrollbarArrowRightPressedImage), texParams));

		m_config.scrollbarButtonUpMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowUpImage, sizeof(s_defaultThemeScrollbarArrowUpImage), texParams));
		m_config.scrollbarButtonUpHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowUpHoveredImage, sizeof(s_defaultThemeScrollbarArrowUpHoveredImage), texParams));
		m_config.scrollbarButtonUpPressedMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(s_defaultThemeScrollbarArrowUpPressedImage, sizeof(s_defaultThemeScrollbarArrowUpPressedImage), texParams));
	}

	std::unique_ptr<ButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ButtonWidget* buttonWidget) const
	{
		SimpleButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.cornerSize = 20.f;
		styleConfig.cornerTexCoords = 20.f / 128.f;
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

	std::unique_ptr<LabelWidgetStyle> DefaultWidgetTheme::CreateStyle(LabelWidget* labelWidget) const
	{
		return std::make_unique<SimpleLabelWidgetStyle>(labelWidget, Widgets::Instance()->GetTransparentMaterial());
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
