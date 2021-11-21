// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	LabelWidget::LabelWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		m_textEntity = CreateEntity();
		GetRegistry().emplace<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite, GetCanvas()->GetRenderMask());
		GetRegistry().emplace<NodeComponent>(m_textEntity).SetParent(this);

		Layout();
	}
}
