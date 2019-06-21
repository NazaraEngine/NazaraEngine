// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/ImageWidget.hpp>
#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>

namespace Ndk
{
	ImageWidget::ImageWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_entity = CreateEntity();
		m_entity->AddComponent<NodeComponent>();
		auto& gfx = m_entity->AddComponent<GraphicsComponent>();

		m_sprite = Nz::Sprite::New();
		gfx.Attach(m_sprite);
	}

	const Nz::TextureRef& ImageWidget::GetTexture() const
	{
		Nz::PhongLightingMaterial phongMat(m_sprite->GetMaterial());
		return phongMat.GetDiffuseMap();
	}

	void ImageWidget::SetTexture(const Nz::TextureRef& texture)
	{
		m_sprite->SetTexture(texture, false);

		Nz::PhongLightingMaterial phongMat(m_sprite->GetMaterial());

		Nz::Vector2f textureSize = Nz::Vector2f(Nz::Vector2ui(phongMat.GetDiffuseMap()->GetSize()));
		SetMinimumSize(textureSize);
		SetPreferredSize(textureSize);
	}

	void ImageWidget::Layout()
	{
		BaseWidget::Layout();

		m_sprite->SetSize(GetSize());
	}
}
