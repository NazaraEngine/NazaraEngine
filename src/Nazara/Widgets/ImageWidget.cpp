// Copyright (C) 2024 Samy Bensaid
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ImageWidget.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>

namespace Nz
{
	ImageWidget::ImageWidget(BaseWidget* parent, std::shared_ptr<MaterialInstance> material) :
	BaseWidget(parent)
	{
		m_sprite = std::make_shared<Sprite>(std::move(material));

		auto& registry = GetRegistry();

		m_entity = CreateGraphicsEntity();

		auto& gfxComponent = registry.get<GraphicsComponent>(m_entity);
		gfxComponent.AttachRenderable(m_sprite, GetCanvas()->GetRenderMask());
	}

	void ImageWidget::Layout()
	{
		BaseWidget::Layout();

		m_sprite->SetSize(GetSize());
	}
}
