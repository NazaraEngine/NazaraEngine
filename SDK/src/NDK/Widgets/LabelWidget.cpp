// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/LabelWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	LabelWidget::LabelWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);

		Layout();
	}

	void LabelWidget::ResizeToContent()
	{
		SetContentSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}
}
