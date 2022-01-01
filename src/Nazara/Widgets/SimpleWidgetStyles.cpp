// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/SimpleWidgetStyles.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/CheckboxWidget.hpp>
#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	SimpleButtonWidgetStyle::SimpleButtonWidgetStyle(ButtonWidget* buttonWidget, StyleConfig config) :
	ButtonWidgetStyle(buttonWidget, 2),
	m_hoveredMaterial(std::move(config.hoveredMaterial)),
	m_material(std::move(config.material)),
	m_pressedMaterial(std::move(config.pressedMaterial)),
	m_pressedHoveredMaterial(std::move(config.pressedHoveredMaterial)),
	m_isHovered(false),
	m_isPressed(false)
	{
		assert(m_material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner corner;
		corner.size.Set(config.cornerSize);
		corner.textureCoords.Set(config.cornerTexCoords);

		m_sprite = std::make_shared<SlicedSprite>(m_material);
		m_sprite->SetCorners(corner, corner);

		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		m_spriteEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_spriteEntity).AttachRenderable(m_sprite, renderMask);

		m_textEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_textEntity).AttachRenderable(m_textSprite, renderMask);
	}

	void SimpleButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_sprite->SetSize(size);

		entt::registry& registry = GetRegistry();

		Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_textEntity).SetPosition(size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f);
	}

	void SimpleButtonWidgetStyle::OnHoverBegin()
	{
		m_isHovered = true;
		UpdateMaterial(m_isHovered, m_isPressed);
	}

	void SimpleButtonWidgetStyle::OnHoverEnd()
	{
		m_isHovered = false;
		UpdateMaterial(m_isHovered, m_isPressed);
	}

	void SimpleButtonWidgetStyle::OnPress()
	{
		m_isPressed = true;
		UpdateMaterial(m_isHovered, m_isPressed);
	}

	void SimpleButtonWidgetStyle::OnRelease()
	{
		m_isPressed = false;
		UpdateMaterial(m_isHovered, m_isPressed);
	}

	void SimpleButtonWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_sprite->UpdateRenderLayer(baseRenderLayer);
		m_textSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	void SimpleButtonWidgetStyle::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);
	}

	void SimpleButtonWidgetStyle::UpdateMaterial(bool hovered, bool pressed)
	{
		if (pressed && hovered && m_pressedHoveredMaterial)
			m_sprite->SetMaterial(m_pressedHoveredMaterial);
		else if (pressed && m_pressedMaterial)
			m_sprite->SetMaterial(m_pressedMaterial);
		else if (hovered && m_hoveredMaterial)
			m_sprite->SetMaterial(m_hoveredMaterial);
		else
			m_sprite->SetMaterial(m_material);
	}


	SimpleCheckboxWidgetStyle::SimpleCheckboxWidgetStyle(CheckboxWidget* buttonWidget, StyleConfig config) :
	CheckboxWidgetStyle(buttonWidget, 2),
	m_checkMaterial(std::move(config.checkMaterial)),
	m_hoveredMaterial(std::move(config.backgroundHoveredMaterial)),
	m_material(std::move(config.backgroundMaterial)),
	m_tristateMaterial(std::move(config.tristateMaterial)),
	m_isHovered(false)
	{
		assert(m_material);
		assert(m_checkMaterial);
		assert(m_tristateMaterial);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner corner;
		corner.size.Set(config.backgroundCornerSize);
		corner.textureCoords.Set(config.backgroundCornerTexCoords);

		m_backgroundSprite = std::make_shared<SlicedSprite>(m_material);
		m_backgroundSprite->SetCorners(corner, corner);

		m_checkSprite = std::make_shared<Sprite>(m_checkMaterial);

		m_backgroundEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_backgroundEntity).AttachRenderable(m_backgroundSprite, renderMask);

		m_checkEntity = CreateGraphicsEntity();
		{
			auto& gfxComponent = registry.get<GraphicsComponent>(m_checkEntity);
			gfxComponent.AttachRenderable(m_checkSprite, renderMask);
			gfxComponent.Hide();
		}
	}

	void SimpleCheckboxWidgetStyle::Layout(const Vector2f& size)
	{
		m_backgroundSprite->SetSize(size);

		Vector2f checkSize = size * 0.66f;
		m_checkSprite->SetSize(checkSize);

		GetRegistry().get<NodeComponent>(m_checkEntity).SetPosition(size.x / 2.f - checkSize.x / 2.f, size.y / 2.f - checkSize.y / 2.f);
	}

	void SimpleCheckboxWidgetStyle::OnHoverBegin()
	{
		m_isHovered = true;
		UpdateMaterial(m_isHovered);
	}

	void SimpleCheckboxWidgetStyle::OnHoverEnd()
	{
		m_isHovered = false;
		UpdateMaterial(m_isHovered);
	}

	void SimpleCheckboxWidgetStyle::OnNewState(CheckboxState newState)
	{
		switch (newState)
		{
			case CheckboxState::Unchecked:
			{
				GetRegistry().get<GraphicsComponent>(m_checkEntity).Hide();
				break;
			}

			case CheckboxState::Tristate:
			case CheckboxState::Checked:
			{
				m_checkSprite->SetMaterial((newState == CheckboxState::Checked) ? m_checkMaterial : m_tristateMaterial);
				GetRegistry().get<GraphicsComponent>(m_checkEntity).Show();
				break;
			}
		}
	}

	void SimpleCheckboxWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_backgroundSprite->UpdateRenderLayer(baseRenderLayer);
		m_checkSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	void SimpleCheckboxWidgetStyle::UpdateMaterial(bool hovered)
	{
		if (hovered && m_hoveredMaterial)
			m_backgroundSprite->SetMaterial(m_hoveredMaterial);
		else
			m_backgroundSprite->SetMaterial(m_material);
	}


	SimpleLabelWidgetStyle::SimpleLabelWidgetStyle(LabelWidget* labelWidget, std::shared_ptr<Material> material, std::shared_ptr<Material> hoveredMaterial) :
	LabelWidgetStyle(labelWidget, 1),
	m_hoveredMaterial(std::move(hoveredMaterial)),
	m_material(std::move(material))
	{
		assert(m_material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		m_textSprite = std::make_shared<TextSprite>(m_material);

		m_entity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_entity).AttachRenderable(m_textSprite, renderMask);
	}

	void SimpleLabelWidgetStyle::Layout(const Vector2f& size)
	{
		entt::registry& registry = GetRegistry();

		Boxf textBox = m_textSprite->GetAABB();
		registry.get<NodeComponent>(m_entity).SetPosition(size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f);
	}

	void SimpleLabelWidgetStyle::OnHoverBegin()
	{
		UpdateMaterial(true);
	}

	void SimpleLabelWidgetStyle::OnHoverEnd()
	{
		UpdateMaterial(false);
	}

	void SimpleLabelWidgetStyle::UpdateMaterial(bool hovered)
	{
		if (hovered && m_hoveredMaterial)
			m_textSprite->SetMaterial(m_hoveredMaterial);
		else
			m_textSprite->SetMaterial(m_material);
	}

	void SimpleLabelWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_textSprite->UpdateRenderLayer(baseRenderLayer);
	}

	void SimpleLabelWidgetStyle::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);
	}
}
