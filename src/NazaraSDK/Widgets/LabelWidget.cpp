// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/LabelWidget.hpp>
#include <NazaraSDK/Components/GraphicsComponent.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>

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
}
