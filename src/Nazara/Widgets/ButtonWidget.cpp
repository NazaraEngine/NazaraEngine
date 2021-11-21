// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

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
		UInt32 renderMask = GetCanvas()->GetRenderMask();

		m_gradientMaterialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());

		auto gradientMaterial = std::make_shared<Material>();
		gradientMaterial->AddPass("ForwardPass", m_gradientMaterialPass);

		m_gradientSprite = std::make_shared<Sprite>(gradientMaterial);
		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(RectCorner::LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(RectCorner::RightBottom, m_cornerColor);

		m_gradientEntity = CreateEntity();
		registry.emplace<NodeComponent>(m_gradientEntity).SetParent(this);
		registry.emplace<GraphicsComponent>(m_gradientEntity).AttachRenderable(m_gradientSprite, renderMask);

		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		m_textEntity = CreateEntity();
		registry.emplace<NodeComponent>(m_textEntity).SetParent(this);
		registry.emplace<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite, renderMask);

		Layout();
	}

	void ButtonWidget::Layout()
	{
		BaseWidget::Layout();

		Vector2f size = GetSize();
		m_gradientSprite->SetSize(size);

		entt::registry& registry = GetRegistry();

		Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_textEntity).SetPosition(size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f);
	}

	void ButtonWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_gradientSprite->SetColor(m_pressColor);
			m_gradientSprite->SetCornerColor(RectCorner::LeftBottom, m_pressCornerColor);
			m_gradientSprite->SetCornerColor(RectCorner::RightBottom, m_pressCornerColor);
		}
	}

	void ButtonWidget::OnMouseButtonRelease(int /*x*/, int /*y*/, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_gradientSprite->SetColor(m_hoverColor);
			m_gradientSprite->SetCornerColor(RectCorner::LeftBottom, m_hoverCornerColor);
			m_gradientSprite->SetCornerColor(RectCorner::RightBottom, m_hoverCornerColor);

			OnButtonTrigger(this);
		}
	}

	void ButtonWidget::OnMouseEnter()
	{
		m_gradientSprite->SetColor(m_hoverColor);
		m_gradientSprite->SetCornerColor(RectCorner::LeftBottom, m_hoverCornerColor);
		m_gradientSprite->SetCornerColor(RectCorner::RightBottom, m_hoverCornerColor);
	}

	void ButtonWidget::OnMouseExit()
	{
		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(RectCorner::LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(RectCorner::RightBottom, m_cornerColor);
	}
}
