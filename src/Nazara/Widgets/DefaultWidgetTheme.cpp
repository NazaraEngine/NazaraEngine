// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/DefaultWidgetTheme.hpp>
#include <Nazara/Core/GuillotineImageAtlas.hpp>
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

		constexpr UInt8 s_defaultThemeTextAreaBackgroundImage[] = {
			#include <Nazara/Widgets/Resources/DefaultTheme/TextArea.png.h>
		};
	}

	DefaultWidgetTheme::DefaultWidgetTheme()
	{
		GuillotineImageAtlas widgetAtlas(PixelFormat::RGBA8, 512);

		auto InsertWidgetImage = [&](std::span<const UInt8> textureMemory)
		{
			std::shared_ptr<Image> image = Image::LoadFromMemory(textureMemory.data(), textureMemory.size());
			Vector2ui imageSize = Vector2ui(image->GetSize());

			Rectui rect;
			rect.width = imageSize.x + 2;
			rect.height = imageSize.y + 2;

			widgetAtlas.Insert(*image, &rect, nullptr, nullptr);

			// cancel padding
			rect.x += 1;
			rect.y += 1;
			rect.width -= 2;
			rect.height -= 2;

			return Rectf(rect);
		};

		m_hoveredRect = InsertWidgetImage(s_defaultThemeHoveredImage);

		// Button materials
		m_buttonRect = InsertWidgetImage(s_defaultThemeButtonImage);
		m_buttonHoveredRect = InsertWidgetImage(s_defaultThemeButtonHoveredImage);
		m_buttonPressedRect = InsertWidgetImage(s_defaultThemeButtonPressedImage);
		m_buttonPressedHoveredRect = InsertWidgetImage(s_defaultThemeButtonPressedHoveredImage);

		// Checkbox materials
		m_checkboxBackgroundRect = InsertWidgetImage(s_defaultThemeCheckboxBackgroundImage);
		m_checkboxBackgroundHoveredRect = InsertWidgetImage(s_defaultThemeCheckboxBackgroundHoveredImage);
		m_checkboxCheckRect = InsertWidgetImage(s_defaultThemeCheckboxCheckImage);
		m_checkboxTristateRect = InsertWidgetImage(s_defaultThemeCheckboxTristateImage);

		// ProgressBar materials
		m_progressBarRect = InsertWidgetImage(s_defaultThemeProgressBarBackgroundImage);

		// Scrollbar materials
		m_scrollbarBackgroundHorizontalRect = InsertWidgetImage(s_defaultThemeScrollbarHorizontalBackgroundImage);
		m_scrollbarBackgroundVerticalRect = InsertWidgetImage(s_defaultThemeScrollbarVerticalBackgroundImage);

		// TextArea
		m_textBoxRect = InsertWidgetImage(s_defaultThemeTextAreaBackgroundImage);

		// Config
		m_scrollbarButtonRect = InsertWidgetImage(s_defaultThemeScrollbarCenterImage);
		m_scrollbarButtonGrabbedRect = InsertWidgetImage(s_defaultThemeScrollbarGrabbedImage);
		m_scrollbarButtonHoveredRect = InsertWidgetImage(s_defaultThemeScrollbarHoveredImage);

		m_scrollbarButtonDownRect = InsertWidgetImage(s_defaultThemeScrollbarArrowDownImage);
		m_scrollbarButtonDownHoveredRect = InsertWidgetImage(s_defaultThemeScrollbarArrowDownHoveredImage);
		m_scrollbarButtonDownPressedRect = InsertWidgetImage(s_defaultThemeScrollbarArrowDownPressedImage);

		m_scrollbarButtonLeftRect = InsertWidgetImage(s_defaultThemeScrollbarArrowLeftImage);
		m_scrollbarButtonLeftHoveredRect = InsertWidgetImage(s_defaultThemeScrollbarArrowLeftHoveredImage);
		m_scrollbarButtonLeftPressedRect = InsertWidgetImage(s_defaultThemeScrollbarArrowLeftPressedImage);

		m_scrollbarButtonRightRect = InsertWidgetImage(s_defaultThemeScrollbarArrowRightImage);
		m_scrollbarButtonRightHoveredRect = InsertWidgetImage(s_defaultThemeScrollbarArrowRightHoveredImage);
		m_scrollbarButtonRightPressedRect = InsertWidgetImage(s_defaultThemeScrollbarArrowRightPressedImage);

		m_scrollbarButtonUpRect = InsertWidgetImage(s_defaultThemeScrollbarArrowUpImage);
		m_scrollbarButtonUpHoveredRect = InsertWidgetImage(s_defaultThemeScrollbarArrowUpHoveredImage);
		m_scrollbarButtonUpPressedRect = InsertWidgetImage(s_defaultThemeScrollbarArrowUpPressedImage);

		Image* widgetAtlasImage = SafeCast<Image*>(widgetAtlas.GetLayer(0));
		m_imageSize = Vector2f(Vector2ui32(widgetAtlasImage->GetSize()));

		m_material = Widgets::Instance()->GetTransparentMaterial()->Clone();
		m_material->SetTextureProperty("BaseColorMap", TextureAsset::CreateFromImage(*widgetAtlasImage, { .sRGB = true }));
	}

	std::unique_ptr<ButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ButtonWidget* buttonWidget) const
	{
		SimpleButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.coords = RectToCoords(m_buttonRect);
		styleConfig.cornerSize = 20.f;
		styleConfig.cornerTexCoords = 20.f / 44.f;
		styleConfig.hoveredCoords = RectToCoords(m_buttonHoveredRect);
		styleConfig.material = m_material;
		styleConfig.pressedHoveredCoords = RectToCoords(m_buttonPressedHoveredRect);
		styleConfig.pressedCoords = RectToCoords(m_buttonPressedRect);

		return std::make_unique<SimpleButtonWidgetStyle>(buttonWidget, std::move(styleConfig));
	}

	std::unique_ptr<CheckboxWidgetStyle> DefaultWidgetTheme::CreateStyle(CheckboxWidget* checkboxWidget) const
	{
		SimpleCheckboxWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundCornerSize = 10.f;
		styleConfig.backgroundCornerTexCoords = 10.f / 64.f;
		styleConfig.backgroundHoveredCoords = RectToCoords(m_checkboxBackgroundHoveredRect);
		styleConfig.backgroundCoords = RectToCoords(m_checkboxBackgroundRect);
		styleConfig.checkCoords = RectToCoords(m_checkboxCheckRect);
		styleConfig.material = m_material;
		styleConfig.tristateCoords = RectToCoords(m_checkboxTristateRect);

		return std::make_unique<SimpleCheckboxWidgetStyle>(checkboxWidget, std::move(styleConfig));
	}

	std::unique_ptr<ImageButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ImageButtonWidget* imageButtonWidget) const
	{
		SimpleImageButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.hoveredCornerSize = 8.f;
		styleConfig.hoveredCornerTexCoords = 8.f / 64.f;
		styleConfig.hoveredCoords = RectToCoords(m_hoveredRect);
		styleConfig.material = m_material;

		return std::make_unique<SimpleImageButtonWidgetStyle>(imageButtonWidget, std::move(styleConfig));
	}

	std::unique_ptr<LabelWidgetStyle> DefaultWidgetTheme::CreateStyle(AbstractLabelWidget* labelWidget) const
	{
		SimpleLabelWidgetStyle::StyleConfig styleConfig;
		styleConfig.material = Widgets::Instance()->GetTransparentMaterial();

		return std::make_unique<SimpleLabelWidgetStyle>(labelWidget, std::move(styleConfig));
	}

	std::unique_ptr<ProgressBarWidgetStyle> DefaultWidgetTheme::CreateStyle(ProgressBarWidget* progressBarWidget) const
	{
		SimpleProgressBarWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundCornerSize = 16.f;
		styleConfig.backgroundCornerTexCoords = 16.f / 64.f;
		styleConfig.backgroundCoords = RectToCoords(m_progressBarRect);
		styleConfig.barOffset = 12.f;
		styleConfig.material = m_material;
		styleConfig.progressBarBeginColor = Nz::Color::DarkGreen();
		styleConfig.progressBarEndColor = Nz::Color::Green();

		return std::make_unique<SimpleProgressBarWidgetStyle>(progressBarWidget, std::move(styleConfig));
	}

	std::unique_ptr<ScrollAreaWidgetStyle> DefaultWidgetTheme::CreateStyle(ScrollAreaWidget* scrollAreaWidget) const
	{
		return std::make_unique<SimpleScrollAreaWidgetStyle>(scrollAreaWidget);
	}

	std::unique_ptr<ScrollbarWidgetStyle> DefaultWidgetTheme::CreateStyle(ScrollbarWidget* scrollBarWidget) const
	{
		SimpleScrollbarWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundHorizontalCoords = RectToCoords(m_scrollbarBackgroundHorizontalRect);
		styleConfig.backgroundVerticalCoords = RectToCoords(m_scrollbarBackgroundVerticalRect);
		styleConfig.buttonCornerSize = 0.f;
		styleConfig.buttonCornerTexcoords = 0.f;
		styleConfig.buttonDownHoveredCoords = RectToCoords(m_scrollbarButtonDownHoveredRect);
		styleConfig.buttonDownPressedCoords = RectToCoords(m_scrollbarButtonDownPressedRect);
		styleConfig.buttonDownCoords = RectToCoords(m_scrollbarButtonDownRect);
		styleConfig.buttonLeftHoveredCoords = RectToCoords(m_scrollbarButtonLeftHoveredRect);
		styleConfig.buttonLeftPressedCoords = RectToCoords(m_scrollbarButtonLeftPressedRect);
		styleConfig.buttonLeftCoords = RectToCoords(m_scrollbarButtonLeftRect);
		styleConfig.buttonRightHoveredCoords = RectToCoords(m_scrollbarButtonRightHoveredRect);
		styleConfig.buttonRightPressedCoords = RectToCoords(m_scrollbarButtonRightPressedRect);
		styleConfig.buttonRightCoords = RectToCoords(m_scrollbarButtonRightRect);
		styleConfig.buttonUpHoveredCoords = RectToCoords(m_scrollbarButtonUpHoveredRect);
		styleConfig.buttonUpPressedCoords = RectToCoords(m_scrollbarButtonUpPressedRect);
		styleConfig.buttonUpCoords = RectToCoords(m_scrollbarButtonUpRect);
		styleConfig.material = m_material;

		return std::make_unique<SimpleScrollbarWidgetStyle>(scrollBarWidget, std::move(styleConfig));
	}

	std::unique_ptr<ScrollbarButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ScrollbarButtonWidget* scrollbarButtonWidget) const
	{
		SimpleScrollbarButtonWidgetStyle::StyleConfig styleConfig;
		styleConfig.cornerSize = 16.f;
		styleConfig.cornerTexCoords = 16.f / 64.f;
		styleConfig.grabbedCoords = RectToCoords(m_scrollbarButtonGrabbedRect);
		styleConfig.hoveredCoords = RectToCoords(m_scrollbarButtonHoveredRect);
		styleConfig.coords = RectToCoords(m_scrollbarButtonRect);
		styleConfig.material = m_material;

		return std::make_unique<SimpleScrollbarButtonWidgetStyle>(scrollbarButtonWidget, std::move(styleConfig));
	}

	std::unique_ptr<TextAreaWidgetStyle> DefaultWidgetTheme::CreateStyle(AbstractTextAreaWidget* textAreaWidget) const
	{
		SimpleTextAreaWidgetStyle::StyleConfig styleConfig;
		styleConfig.backgroundCornerSize = 20.f;
		styleConfig.backgroundCornerTexCoords = 20.f / 44.f;
		styleConfig.backgroundCoords = RectToCoords(m_textBoxRect);
		styleConfig.insertionCursorColor = Color::Black();
		styleConfig.material = m_material;
		styleConfig.padding = { 10.f, 10.f };
		styleConfig.selectionCursorColor = Color(0.f, 0.f, 0.f, 0.2f);
		styleConfig.selectionCursorColorNoFocus = Color(0.5f, 0.5f, 0.5f, 0.2f);

		return std::make_unique<SimpleTextAreaWidgetStyle>(textAreaWidget, std::move(styleConfig));
	}

	Rectf DefaultWidgetTheme::RectToCoords(const Rectf& rect) const
	{
		Rectf coords;
		coords.x = rect.x / m_imageSize.x;
		coords.y = rect.y / m_imageSize.y;
		coords.width = rect.width / m_imageSize.x;
		coords.height = rect.height / m_imageSize.y;

		return coords;
	}
}
