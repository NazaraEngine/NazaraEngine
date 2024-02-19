// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/SimpleWidgetStyles.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Widgets/AbstractLabelWidget.hpp>
#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/CheckboxWidget.hpp>
#include <Nazara/Widgets/ImageButtonWidget.hpp>
#include <Nazara/Widgets/ProgressBarWidget.hpp>
#include <Nazara/Widgets/ScrollAreaWidget.hpp>
#include <Nazara/Widgets/ScrollbarButtonWidget.hpp>
#include <Nazara/Widgets/ScrollbarWidget.hpp>
#include <Nazara/Widgets/Widgets.hpp>

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
		corner.size = Vector2f(config.cornerSize);
		corner.textureCoords = Vector2f(config.cornerTexCoords);

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
		registry.get<NodeComponent>(m_textEntity).SetPosition({ size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f });
	}

	void SimpleButtonWidgetStyle::OnHoverBegin()
	{
		UpdateMaterial(true, m_isPressed);
		m_isHovered = true;
	}

	void SimpleButtonWidgetStyle::OnHoverEnd()
	{
		UpdateMaterial(false, m_isPressed);
		m_isHovered = false;
	}

	void SimpleButtonWidgetStyle::OnPress()
	{
		UpdateMaterial(m_isHovered, true);
		m_isPressed = true;
	}

	void SimpleButtonWidgetStyle::OnRelease()
	{
		UpdateMaterial(m_isHovered, false);
		m_isPressed = false;
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

	/************************************************************************/

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
		corner.size = Vector2f(config.backgroundCornerSize);
		corner.textureCoords = Vector2f(config.backgroundCornerTexCoords);

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

		GetRegistry().get<NodeComponent>(m_checkEntity).SetPosition({ size.x / 2.f - checkSize.x / 2.f, size.y / 2.f - checkSize.y / 2.f });
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

	/************************************************************************/

	SimpleImageButtonWidgetStyle::SimpleImageButtonWidgetStyle(ImageButtonWidget* imageButtonWidget, StyleConfig config) :
	ImageButtonWidgetStyle(imageButtonWidget, 1),
	m_isHovered(false),
	m_isPressed(false)
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner hoveredCorner;
		hoveredCorner.size = Vector2f(config.hoveredCornerSize, config.hoveredCornerSize);
		hoveredCorner.textureCoords = Vector2f(config.hoveredCornerTexCoords, config.hoveredCornerTexCoords);

		m_hoveredSprite = std::make_shared<SlicedSprite>(config.hoveredMaterial);
		m_hoveredSprite->SetCorners(hoveredCorner, hoveredCorner);

		float imageCornerSize = imageButtonWidget->GetCornerSize();
		float imageCornerTexCoords = imageButtonWidget->GetCornerTexCoords();

		SlicedSprite::Corner corner;
		corner.size = Vector2f(imageCornerSize, imageCornerSize);
		corner.textureCoords = Vector2f(imageCornerTexCoords, imageCornerTexCoords);

		m_sprite = std::make_shared<SlicedSprite>(imageButtonWidget->GetMaterial());
		m_sprite->SetCorners(corner, corner);
		m_sprite->SetTextureCoords(imageButtonWidget->GetTextureCoords());

		m_entity = CreateGraphicsEntity();

		GraphicsComponent& gfxComponent = registry.get<GraphicsComponent>(m_entity);
		gfxComponent.AttachRenderable(m_sprite, renderMask);
	}

	void SimpleImageButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_hoveredSprite->SetSize(size);
		m_sprite->SetSize(size);
	}

	void SimpleImageButtonWidgetStyle::OnHoverBegin()
	{
		Update(true, m_isPressed);
		m_isHovered = true;
	}

	void SimpleImageButtonWidgetStyle::OnHoverEnd()
	{
		Update(false, m_isPressed);
		m_isHovered = false;
	}

	void SimpleImageButtonWidgetStyle::OnPress()
	{
		Update(m_isHovered, true);
		m_isPressed = true;
	}

	void SimpleImageButtonWidgetStyle::OnRelease()
	{
		Update(m_isHovered, false);
		m_isPressed = false;
	}

	void SimpleImageButtonWidgetStyle::OnUpdate()
	{
		ImageButtonWidget* owner = GetOwnerWidget<ImageButtonWidget>();

		// If a hovering material was added while we're being hovered, we need to detach the hovering sprite
		if (owner->GetHoveredMaterial())
		{
			GraphicsComponent& gfxComponent = GetRegistry().get<GraphicsComponent>(m_entity);
			gfxComponent.DetachRenderable(m_hoveredSprite);
		}

		m_sprite->SetTextureCoords(owner->GetTextureCoords());

		Update(m_isHovered, m_isPressed);
	}

	void SimpleImageButtonWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_sprite->UpdateRenderLayer(baseRenderLayer);
		m_hoveredSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	void SimpleImageButtonWidgetStyle::Update(bool hovered, bool pressed)
	{
		ImageButtonWidget* owner = GetOwnerWidget<ImageButtonWidget>();

		if (pressed)
		{
			if (const auto& pressedMaterial = owner->GetPressedMaterial())
			{
				m_sprite->SetColor(owner->GetColor());
				m_sprite->SetMaterial(pressedMaterial);
			}
			else
			{
				m_sprite->SetColor(owner->GetColor() * Nz::Color::FromRGB8(120, 120, 120));
				m_sprite->SetMaterial(owner->GetMaterial());
			}
		}
		else
		{
			m_sprite->SetColor(owner->GetColor());
			m_sprite->SetMaterial(owner->GetMaterial());
		}

		if (hovered)
		{
			if (const auto& hoveredMaterial = owner->GetHoveredMaterial())
			{
				if (!pressed)
					m_sprite->SetMaterial(hoveredMaterial);
			}
			else
			{
				if (!pressed)
					m_sprite->SetMaterial(owner->GetMaterial());

				if (!m_isHovered)
				{
					GraphicsComponent& gfxComponent = GetRegistry().get<GraphicsComponent>(m_entity);
					gfxComponent.AttachRenderable(m_hoveredSprite, GetRenderMask());
				}
			}
		}
		else
		{
			if (!pressed)
				m_sprite->SetMaterial(owner->GetMaterial());

			if (m_isHovered)
			{
				GraphicsComponent& gfxComponent = GetRegistry().get<GraphicsComponent>(m_entity);
				gfxComponent.DetachRenderable(m_hoveredSprite);
			}
		}
	}

	/************************************************************************/

	SimpleLabelWidgetStyle::SimpleLabelWidgetStyle(AbstractLabelWidget* labelWidget, std::shared_ptr<MaterialInstance> material, std::shared_ptr<MaterialInstance> hoveredMaterial) :
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
		registry.get<NodeComponent>(m_entity).SetPosition({ size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f });
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

	/************************************************************************/

	SimpleProgressBarWidgetStyle::SimpleProgressBarWidgetStyle(ProgressBarWidget* progressBarWidget, StyleConfig config) :
	ProgressBarWidgetStyle(progressBarWidget, 2),
	m_backgroundMaterial(std::move(config.backgroundMaterial)),
	m_progressBarBeginColor(config.progressBarBeginColor),
	m_progressBarEndColor(config.progressBarEndColor),
	m_barOffset(config.barOffset)
	{
		assert(m_backgroundMaterial);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner backgroundCorner;
		backgroundCorner.size = Vector2f(config.backgroundCornerSize, config.backgroundCornerSize);
		backgroundCorner.textureCoords = Vector2f(config.backgroundCornerTexCoords, config.backgroundCornerTexCoords);

		m_backgroundSprite = std::make_shared<SlicedSprite>(m_backgroundMaterial);
		m_backgroundSprite->SetCorners(backgroundCorner, backgroundCorner);

		m_backgroundEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_backgroundEntity).AttachRenderable(m_backgroundSprite, renderMask);

		m_progressBarSprite = std::make_shared<Sprite>(Widgets::Instance()->GetTransparentMaterial());
		m_progressBarSprite->SetCornerColor(RectCorner::LeftBottom, m_progressBarBeginColor);
		m_progressBarSprite->SetCornerColor(RectCorner::LeftTop, m_progressBarBeginColor);
		m_progressBarSprite->SetCornerColor(RectCorner::RightBottom, m_progressBarEndColor);
		m_progressBarSprite->SetCornerColor(RectCorner::RightTop, m_progressBarEndColor);

		m_barEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_barEntity).AttachRenderable(m_progressBarSprite, renderMask);
		registry.get<NodeComponent>(m_barEntity).SetPosition({ m_barOffset, m_barOffset });
	}

	void SimpleProgressBarWidgetStyle::Layout(const Vector2f& size)
	{
		float fraction = GetOwnerWidget<ProgressBarWidget>()->GetFraction();
		float width = std::max(fraction * (size.x - m_barOffset * 2.f), 0.f);

		Color endColor = Lerp(m_progressBarBeginColor, m_progressBarEndColor, fraction);
		m_progressBarSprite->SetCornerColor(RectCorner::RightBottom, endColor);
		m_progressBarSprite->SetCornerColor(RectCorner::RightTop, endColor);

		m_backgroundSprite->SetSize(size);
		m_progressBarSprite->SetSize({ width, size.y - m_barOffset * 2.f });
	}

	void SimpleProgressBarWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_backgroundSprite->UpdateRenderLayer(baseRenderLayer);
		m_progressBarSprite->UpdateRenderLayer(baseRenderLayer + 1);
	}

	/************************************************************************/

	SimpleScrollAreaWidgetStyle::SimpleScrollAreaWidgetStyle(ScrollAreaWidget* scrollAreaWidget) :
	ScrollAreaWidgetStyle(scrollAreaWidget, 0)
	{
	}

	void SimpleScrollAreaWidgetStyle::Layout(const Vector2f& /*size*/)
	{
	}

	void SimpleScrollAreaWidgetStyle::UpdateRenderLayer(int /*baseRenderLayer*/)
	{
	}


	SimpleScrollbarWidgetStyle::SimpleScrollbarWidgetStyle(ScrollbarWidget* scrollBarWidget, StyleConfig config) :
	ScrollbarWidgetStyle(scrollBarWidget, 1),
	m_config(std::move(config))
	{
		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		m_backgroundScrollbarSprite = std::make_shared<Sprite>((scrollBarWidget->GetOrientation() == ScrollbarOrientation::Horizontal) ? m_config.backgroundHorizontalMaterial : m_config.backgroundVerticalMaterial);

		m_backgroundScrollbarSpriteEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_backgroundScrollbarSpriteEntity).AttachRenderable(m_backgroundScrollbarSprite, renderMask);

		m_scrollbarSpriteEntity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_scrollbarSpriteEntity).AttachRenderable(m_scrollbarSprite, renderMask);
	}

	void SimpleScrollbarWidgetStyle::Layout(const Vector2f& size)
	{
		m_backgroundScrollbarSprite->SetSize(size);
	}

	void SimpleScrollbarWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_backgroundScrollbarSprite->UpdateRenderLayer(baseRenderLayer);
	}

	/************************************************************************/

	SimpleScrollbarButtonWidgetStyle::SimpleScrollbarButtonWidgetStyle(ScrollbarButtonWidget* scrollbarButtonWidget, StyleConfig config) :
	ScrollbarButtonWidgetStyle(scrollbarButtonWidget, 1),
	m_hoveredMaterial(std::move(config.hoveredMaterial)),
	m_material(std::move(config.material)),
	m_pressedMaterial(std::move(config.grabbedMaterial)),
	m_pressedHoveredMaterial(std::move(config.grabbedHoveredMaterial)),
	m_isHovered(false),
	m_isPressed(false)
	{
		assert(m_material);

		auto& registry = GetRegistry();
		UInt32 renderMask = GetRenderMask();

		SlicedSprite::Corner corner;
		corner.size = Vector2f(config.cornerSize);
		corner.textureCoords = Vector2f(config.cornerTexCoords);

		m_sprite = std::make_shared<SlicedSprite>(m_material);
		m_sprite->SetCorners(corner, corner);

		m_entity = CreateGraphicsEntity();
		registry.get<GraphicsComponent>(m_entity).AttachRenderable(m_sprite, renderMask);
	}

	void SimpleScrollbarButtonWidgetStyle::Layout(const Vector2f& size)
	{
		m_sprite->SetSize(size);
	}

	void SimpleScrollbarButtonWidgetStyle::OnHoverBegin()
	{
		Update(true, m_isPressed);
		m_isHovered = true;
	}

	void SimpleScrollbarButtonWidgetStyle::OnHoverEnd()
	{
		Update(false, m_isPressed);
		m_isHovered = false;
	}

	void SimpleScrollbarButtonWidgetStyle::OnGrab()
	{
		Update(m_isHovered, true);
		m_isPressed = true;
	}

	void SimpleScrollbarButtonWidgetStyle::OnRelease()
	{
		Update(m_isHovered, false);
		m_isPressed = false;
	}

	void SimpleScrollbarButtonWidgetStyle::UpdateRenderLayer(int baseRenderLayer)
	{
		m_sprite->UpdateRenderLayer(baseRenderLayer);
	}

	void SimpleScrollbarButtonWidgetStyle::Update(bool hovered, bool pressed)
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
}
