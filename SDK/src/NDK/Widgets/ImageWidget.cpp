// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/ImageWidget.hpp>
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

	void ImageWidget::ResizeToContent()
	{
		Nz::Vector3ui textureSize = m_sprite->GetMaterial()->GetDiffuseMap()->GetSize();
		SetSize({ static_cast<float>(textureSize.x), static_cast<float>(textureSize.y) });
	}

	void ImageWidget::Layout()
	{
		BaseWidget::Layout();
		Nz::Vector2f origin = GetContentOrigin();
		Nz::Vector2f contentSize = GetContentSize();

		m_entity->GetComponent<NodeComponent>().SetPosition(origin);
		m_sprite->SetSize(contentSize);
	}
}
