// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/DefaultWidgetTheme.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Widgets/SimpleWidgetStyles.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 ButtonImage[] = {
			#include <Nazara/Widgets/Resources/DefaultStyle/Button.png.h>
		};

		const UInt8 ButtonHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultStyle/ButtonHovered.png.h>
		};

		const UInt8 ButtonPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultStyle/ButtonPressed.png.h>
		};

		const UInt8 ButtonPressedHoveredImage[] = {
			#include <Nazara/Widgets/Resources/DefaultStyle/ButtonPressedHovered.png.h>
		};
	}

	DefaultWidgetTheme::DefaultWidgetTheme()
	{
		TextureParams texParams;
		texParams.renderDevice = Graphics::Instance()->GetRenderDevice();
		texParams.loadFormat = PixelFormat::RGBA8_SRGB;

		auto CreateMaterialFromTexture = [](std::shared_ptr<Texture> texture)
		{
			std::shared_ptr<MaterialPass> buttonMaterialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
			buttonMaterialPass->EnableDepthBuffer(true);
			buttonMaterialPass->EnableDepthWrite(false);

			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->AddPass("ForwardPass", buttonMaterialPass);

			BasicMaterial buttonBasicMat(*buttonMaterialPass);
			buttonBasicMat.SetDiffuseMap(texture);

			return material;
		};
		
		// Button material
		m_buttonMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(ButtonImage, sizeof(ButtonImage), texParams));
		m_hoveredButtonMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(ButtonHoveredImage, sizeof(ButtonHoveredImage), texParams));
		m_pressedButtonMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(ButtonPressedImage, sizeof(ButtonPressedImage), texParams));
		m_pressedHoveredMaterial = CreateMaterialFromTexture(Texture::LoadFromMemory(ButtonPressedHoveredImage, sizeof(ButtonPressedHoveredImage), texParams));
	}

	std::unique_ptr<ButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ButtonWidget* buttonWidget) const
	{
		return std::make_unique<SimpleButtonWidgetStyle>(buttonWidget, m_buttonMaterial, m_hoveredButtonMaterial, m_pressedButtonMaterial, m_pressedHoveredMaterial);
	}

	std::unique_ptr<LabelWidgetStyle> DefaultWidgetTheme::CreateStyle(LabelWidget* buttonWidget) const
	{
		return std::make_unique<SimpleLabelWidgetStyle>(buttonWidget, Widgets::Instance()->GetTransparentMaterial());
	}
}
