// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/DefaultWidgetTheme.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 ButtonImage[] = {
			#include <Nazara/Widgets/Resources/DefaultStyle/Button.png.h>
		};

		const UInt8 ButtonPressedImage[] = {
			#include <Nazara/Widgets/Resources/DefaultStyle/ButtonPressed.png.h>
		};
	}

	DefaultWidgetTheme::DefaultWidgetTheme()
	{
		TextureParams texParams;
		texParams.renderDevice = Graphics::Instance()->GetRenderDevice();
		texParams.loadFormat = PixelFormat::RGBA8_SRGB;

		// Button material
		{
			std::shared_ptr<MaterialPass> buttonMaterialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
			buttonMaterialPass->EnableDepthBuffer(true);
			buttonMaterialPass->EnableDepthWrite(false);

			m_buttonMaterial = std::make_shared<Material>();
			m_buttonMaterial->AddPass("ForwardPass", buttonMaterialPass);

			BasicMaterial buttonBasicMat(*buttonMaterialPass);
			buttonBasicMat.SetDiffuseMap(Texture::LoadFromMemory(ButtonImage, sizeof(ButtonImage), texParams));
		}

		// Button (pressed) material
		{
			std::shared_ptr<MaterialPass> buttonMaterialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
			buttonMaterialPass->EnableDepthBuffer(true);
			buttonMaterialPass->EnableDepthWrite(false);

			m_pressedButtonMaterial = std::make_shared<Material>();
			m_pressedButtonMaterial->AddPass("ForwardPass", buttonMaterialPass);

			BasicMaterial buttonBasicMat(*buttonMaterialPass);
			buttonBasicMat.SetDiffuseMap(Texture::LoadFromMemory(ButtonPressedImage, sizeof(ButtonPressedImage), texParams));
		}
	}

	std::unique_ptr<ButtonWidgetStyle> DefaultWidgetTheme::CreateStyle(ButtonWidget* buttonWidget) const
	{
		return std::make_unique<DefaultButtonWidgetStyle>(buttonWidget, m_buttonMaterial, m_pressedButtonMaterial);
	}

	DefaultButtonWidgetStyle::DefaultButtonWidgetStyle(ButtonWidget* buttonWidget, std::shared_ptr<Material> defaultMaterial, std::shared_ptr<Material> pressedMaterial) :
	ButtonWidgetStyle(buttonWidget),
	m_defaultMaterial(std::move(defaultMaterial)),
	m_pressedMaterial(std::move(pressedMaterial))
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		m_sprite = std::make_shared<SlicedSprite>(m_defaultMaterial);

		m_gradientEntity = CreateEntity();
		registry.emplace<NodeComponent>(m_gradientEntity).SetParent(buttonWidget);
		registry.emplace<GraphicsComponent>(m_gradientEntity).AttachRenderable(m_sprite, renderMask);

		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		m_textEntity = CreateEntity();
		registry.emplace<NodeComponent>(m_textEntity).SetParent(buttonWidget);
		registry.emplace<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite, renderMask);
	}

	void DefaultButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_sprite->SetSize(size);

		entt::registry& registry = GetRegistry();

		Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_textEntity).SetPosition(size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f);
	}

	void DefaultButtonWidgetStyle::OnHoverBegin()
	{
	}

	void DefaultButtonWidgetStyle::OnHoverEnd()
	{
		m_sprite->SetMaterial(m_defaultMaterial);
	}

	void DefaultButtonWidgetStyle::OnPress()
	{
		m_sprite->SetMaterial(m_pressedMaterial);
	}

	void DefaultButtonWidgetStyle::OnRelease()
	{
		m_sprite->SetMaterial(m_defaultMaterial);
	}

	void DefaultButtonWidgetStyle::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);
	}
}
