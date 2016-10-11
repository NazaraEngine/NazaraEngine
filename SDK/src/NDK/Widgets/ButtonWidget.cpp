// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/ButtonWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	ButtonWidget::ButtonWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_gradientSprite = Nz::Sprite::New();
		m_gradientSprite->SetColor(Nz::Color(74, 74, 74));
		m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, Nz::Color(180, 180, 180));
		m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, Nz::Color(180, 180, 180));
		m_gradientSprite->SetMaterial(Nz::Material::New("Basic2D"));

		m_gradientEntity = CreateEntity();
		m_gradientEntity->AddComponent<NodeComponent>().SetParent(this);
		m_gradientEntity->AddComponent<GraphicsComponent>().Attach(m_gradientSprite);

		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite, 1);

		Layout();
	}

	void ButtonWidget::ResizeToContent()
	{
		SetContentSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}

	void ButtonWidget::Layout()
	{
		BaseWidget::Layout();

		const Nz::Vector2f& contentSize = GetContentSize();

		m_gradientSprite->SetSize(contentSize);

		Nz::Boxf textBox = m_textEntity->GetComponent<GraphicsComponent>().GetBoundingVolume().aabb;
		m_textEntity->GetComponent<NodeComponent>().SetPosition(contentSize.x / 2 - textBox.width / 2, contentSize.y / 2 - textBox.height / 2);
	}

	void ButtonWidget::OnMouseButtonRelease(int /*x*/, int /*y*/, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
			OnButtonTrigger(this);
	}

	void ButtonWidget::OnMouseEnter()
	{
		m_gradientSprite->SetColor(Nz::Color(128, 128, 128));
	}

	void ButtonWidget::OnMouseMoved(int x, int y, int deltaX, int deltaY)
	{
	}

	void ButtonWidget::OnMouseExit()
	{
		m_gradientSprite->SetColor(Nz::Color(74, 74, 74));
	}
}
