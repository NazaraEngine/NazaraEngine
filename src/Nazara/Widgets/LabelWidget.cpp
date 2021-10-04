// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>

namespace Nz
{
	LabelWidget::LabelWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		auto materialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
		materialPass->EnableFlag(Nz::MaterialPassFlag::Transparent);
		materialPass->EnableDepthBuffer(true);
		materialPass->EnableDepthWrite(false);
		materialPass->EnableBlending(true);
		materialPass->SetBlendEquation(Nz::BlendEquation::Add, Nz::BlendEquation::Add);
		materialPass->SetBlendFunc(Nz::BlendFunc::SrcAlpha, Nz::BlendFunc::InvSrcAlpha, Nz::BlendFunc::One, Nz::BlendFunc::Zero);

		auto material = std::make_shared<Material>();
		material->AddPass("ForwardPass", std::move(materialPass));

		m_textSprite = std::make_shared<TextSprite>(std::move(material));

		m_textEntity = CreateEntity();
		GetRegistry().emplace<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite);
		GetRegistry().emplace<NodeComponent>(m_textEntity).SetParent(this);

		Layout();
	}
}
