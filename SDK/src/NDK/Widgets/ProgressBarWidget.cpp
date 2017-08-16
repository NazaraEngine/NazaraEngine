// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>
#include <NDK/Widgets/ProgressBarWidget.hpp>
#include <Nazara/Utility/Image.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>

namespace Ndk
{
	float ProgressBarWidget::s_borderScale { 16.f };
	Nz::Color ProgressBarWidget::s_borderColor { Nz::Color::Black };
	Nz::Color ProgressBarWidget::s_backgroundColor { Nz::Color { 225, 225, 225 } };
	Nz::Color ProgressBarWidget::s_backgroundCornerColor { Nz::Color { 255, 255, 255 } };
	Nz::Color ProgressBarWidget::s_barColor { Nz::Color { 0, 225, 0 } };
	Nz::Color ProgressBarWidget::s_barCornerColor { Nz::Color { 220, 255, 220 } };

	ProgressBarWidget::ProgressBarWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_textColor { Nz::Color::Black },
	m_textMargin { 16.f },
	m_value { 0u }
	{
		m_borderSprite = Nz::Sprite::New(Nz::Material::New("Basic2D"));
		m_backgroundSprite = Nz::Sprite::New(Nz::Material::New("Basic2D"));
		m_barSprite = Nz::Sprite::New(Nz::Material::New("Basic2D"));

		m_borderSprite->SetColor(s_borderColor);

		m_backgroundSprite->SetColor(s_backgroundColor);
		m_backgroundSprite->SetCornerColor(Nz::RectCorner_LeftTop, s_backgroundCornerColor);
		m_backgroundSprite->SetCornerColor(Nz::RectCorner_RightTop, s_backgroundCornerColor);
		m_backgroundSprite->SetCornerColor(Nz::RectCorner_LeftBottom, s_backgroundColor);
		m_backgroundSprite->SetCornerColor(Nz::RectCorner_RightBottom, s_backgroundColor);

		m_barSprite->SetColor(s_barColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_LeftTop, s_barCornerColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_RightTop, s_barCornerColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_LeftBottom, s_barColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_RightBottom, s_barColor);


		m_borderEntity = CreateEntity();
		m_borderEntity->AddComponent<NodeComponent>().SetParent(this);
		m_borderEntity->AddComponent<GraphicsComponent>().Attach(m_borderSprite);

		m_barEntity = CreateEntity();
		m_barEntity->AddComponent<NodeComponent>().SetParent(this);
		GraphicsComponent& graphics = m_barEntity->AddComponent<GraphicsComponent>();

		graphics.Attach(m_backgroundSprite, 1);
		graphics.Attach(m_barSprite, 2);


		m_textSprite = Nz::TextSprite::New();
		m_textEntity = CreateEntity();

		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);

		UpdateText();
		Layout();
	}

	void ProgressBarWidget::Layout()
	{
		Nz::Vector2f origin = GetContentOrigin();
		Nz::Vector2f size = GetContentSize();
		Nz::Vector2f progressBarSize = size;

		if (IsTextEnabled())
		{
			Nz::Vector3f textSize = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
			m_textEntity->GetComponent<NodeComponent>().SetPosition(origin.x + size.x - textSize.x, origin.y + size.y / 2.f - textSize.y);

			progressBarSize -= { textSize.x + m_textMargin, 0.f };
		}

		m_borderSprite->SetSize(progressBarSize);
		Nz::Vector2f borderSize = GetProgressBarBorderSize();

		m_backgroundSprite->SetSize(progressBarSize - (borderSize * 2.f));
		m_barSprite->SetSize((progressBarSize.x - (borderSize.x * 2.f)) / 100.f * static_cast<float>(m_value), progressBarSize.y - (borderSize.y * 2.f));

		m_borderEntity->GetComponent<NodeComponent>().SetPosition(origin.x, origin.y);
		m_barEntity->GetComponent<NodeComponent>().SetPosition(origin.x + borderSize.x, origin.y + borderSize.y);
	}
}
