// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	LabelWidget::LabelWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		auto materialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
		materialPass->EnableFlag(MaterialPassFlag::Transparent);
		materialPass->EnableDepthBuffer(true);
		materialPass->EnableDepthWrite(false);
		materialPass->EnableBlending(true);
		materialPass->SetBlendEquation(BlendEquation::Add, BlendEquation::Add);
		materialPass->SetBlendFunc(BlendFunc::SrcAlpha, BlendFunc::InvSrcAlpha, BlendFunc::One, BlendFunc::Zero);

		auto material = std::make_shared<Material>();
		material->AddPass("ForwardPass", std::move(materialPass));

		m_textSprite = std::make_shared<TextSprite>(std::move(material));

		m_textEntity = CreateEntity();
		GetRegistry().emplace<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite, GetCanvas()->GetRenderMask());
		GetRegistry().emplace<NodeComponent>(m_textEntity).SetParent(this);

		Layout();
	}
}
