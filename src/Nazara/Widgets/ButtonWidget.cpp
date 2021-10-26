// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>

namespace Nz
{
	ButtonWidget::ButtonWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_color { 74, 74, 74 },
	m_cornerColor { 180, 180, 180 },
	m_hoverColor { 128, 128, 128 },
	m_hoverCornerColor { 180, 180, 180 },
	m_pressColor { 180, 180, 180 },
	m_pressCornerColor { 74, 74, 74 }
	{
		entt::registry& registry = GetRegistry();

		m_gradientSprite = std::make_shared<Sprite>();
		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::RightBottom, m_cornerColor);
		m_gradientSprite->SetMaterial(Nz::Material::New("Basic2D"));

		m_gradientEntity = CreateEntity();
		registry.emplace<NodeComponent>(m_gradientEntity).SetParent(this);
		registry.emplace<GraphicsComponent>(m_gradientEntity).AttachRenderable(m_gradientSprite);

		m_textSprite = std::make_shared<TextSprite>();

		m_textEntity = CreateEntity();
		registry.emplace<NodeComponent>(m_textEntity).SetParent(this);
		registry.emplace<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite);

		Layout();
	}

	void ButtonWidget::Layout()
	{
		BaseWidget::Layout();

		Nz::Vector2f size = GetSize();
		m_gradientSprite->SetSize(size);

		entt::registry& registry = GetRegistry();

		Nz::Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_textEntity).SetPosition(size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f);
	}

	void ButtonWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			m_gradientSprite->SetColor(m_pressColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner::LeftBottom, m_pressCornerColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner::RightBottom, m_pressCornerColor);
			m_gradientSprite->SetTexture(m_pressTexture, false);
		}
	}

	void ButtonWidget::OnMouseButtonRelease(int /*x*/, int /*y*/, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			m_gradientSprite->SetColor(m_hoverColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner::LeftBottom, m_hoverCornerColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner::RightBottom, m_hoverCornerColor);
			m_gradientSprite->SetTexture(m_hoverTexture, false);

			OnButtonTrigger(this);
		}
	}

	void ButtonWidget::OnMouseEnter()
	{
		m_gradientSprite->SetColor(m_hoverColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::LeftBottom, m_hoverCornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::RightBottom, m_hoverCornerColor);
		m_gradientSprite->SetTexture(m_hoverTexture, false);
	}

	void ButtonWidget::OnMouseExit()
	{
		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::RightBottom, m_cornerColor);
		m_gradientSprite->SetTexture(m_texture, false);
	}
}

#endif
