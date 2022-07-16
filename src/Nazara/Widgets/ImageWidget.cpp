// Copyright (C) 2022 Samy Bensaid
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ImageWidget.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	ImageWidget::ImageWidget(BaseWidget* parent, std::shared_ptr<Material> material) :
	BaseWidget(parent)
	{
		m_sprite = std::make_shared<Sprite>(Widgets::Instance()->GetTransparentMaterial());

		auto& registry = GetRegistry();

		m_entity = CreateEntity();

		auto& gfxComponent = registry.emplace<GraphicsComponent>(m_entity, IsVisible());
		gfxComponent.AttachRenderable(m_sprite, GetCanvas()->GetRenderMask());

		auto& nodeComponent = registry.emplace<NodeComponent>(m_entity);
		nodeComponent.SetParent(this);

		SetMaterial(std::move(material));
	}

	void ImageWidget::Layout()
	{
		BaseWidget::Layout();

		m_sprite->SetSize(GetSize());
	}
}
