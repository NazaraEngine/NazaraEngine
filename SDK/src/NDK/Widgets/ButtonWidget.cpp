// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/ButtonWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	ButtonWidget::ButtonWidget(const WorldHandle& world, BaseWidget* parent) :
	BaseWidget(world, parent)
	{
		m_gradientSprite = Nz::Sprite::New();
		m_gradientSprite->SetColor(Nz::Color(74, 74, 74));
		m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, Nz::Color(180, 180, 180));
		m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, Nz::Color(180, 180, 180));
		m_gradientSprite->SetMaterial(Nz::Material::New("Basic2D"));

		m_gradientEntity = CreateEntity();
		m_gradientEntity->AddComponent<NodeComponent>().SetParent(this);
		m_gradientEntity->AddComponent<GraphicsComponent>().Attach(m_gradientSprite);
		m_gradientEntity->GetComponent<GraphicsComponent>().Attach(m_borderSprite, Nz::Matrix4f::Translate(Nz::Vector2f(-1.f, -1.f)), -1);

		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite, 1);
	}

	void ButtonWidget::ResizeToContent()
	{
		SetContentSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}

	void ButtonWidget::Layout()
	{
		const Nz::Vector2f& contentSize = GetContentSize();

		m_gradientSprite->SetSize(contentSize);

		Nz::Boxf textBox = m_textEntity->GetComponent<GraphicsComponent>().GetBoundingVolume().aabb;
		m_textEntity->GetComponent<NodeComponent>().SetPosition(contentSize.x / 2 - textBox.width / 2, contentSize.y / 2 - textBox.height / 2);
	}
}
